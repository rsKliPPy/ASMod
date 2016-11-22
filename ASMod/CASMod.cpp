#include <extdll.h>			// always
#include <meta_api.h>

#include <Angelscript/util/ASLogging.h>
#include <Angelscript/util/CASFileLogger.h>

#include "ASMod/IASModModule.h"
#include "CASModModuleInfo.h"

#include "keyvalues/Keyvalues.h"

#include "CASMod.h"

CASMod g_ASMod;

bool CASMod::Initialize()
{
	LOG_MESSAGE( PLID, "Initializing AngelScript Mod Loader" );

	{
		const char* pszPath = gpMetaUtilFuncs->pfnGetPluginPath( PLID );

		if( !pszPath )
		{
			LOG_ERROR( PLID, "Couldn't get plugin path" );
			return false;
		}

		UTIL_SafeStrncpy( m_szLoaderDir, pszPath, sizeof( m_szLoaderDir ) );
		UTIL_FixSlashes( m_szLoaderDir );

		//The path ends with dlls/ASMod.ext, so strip that part. -Solokiller

		{
			char* pszEnd = strrchr( m_szLoaderDir, FILESYSTEM_PATH_SEPARATOR_CHAR );

			//If this can't be found, something is very wrong since there should always be at least 3 separators (addons/plugin_name/dlls/lib.ext)
			if( !pszEnd )
			{
				LOG_ERROR( PLID, "Error while parsing plugin path \"%s\"", pszPath );
				return false;
			}

			*pszEnd = '\0';

			pszEnd = strrchr( m_szLoaderDir, FILESYSTEM_PATH_SEPARATOR_CHAR );

			if( !pszEnd )
			{
				LOG_ERROR( PLID, "Error while parsing plugin path \"%s\"", pszPath );
				return false;
			}

			*pszEnd = '\0';
		}

		//Should never be an empty string.
		if( !( *m_szLoaderDir ) )
		{
			LOG_ERROR( PLID, "Plugin path \"%s\" is invalid", pszPath );
			return false;
		}
	}

	if( !LoadConfig( ASMOD_CONFIG_FILENAME, false ) )
		return false;

	if( !LoadGameModule() )
		return false;

	if( !SetupEnvironment() )
		return false;

	if( !LoadModules() )
		return false;

	return true;
}

void CASMod::Shutdown()
{
	LOG_MESSAGE( PLID, "Shutting down AngelScript Mod Loader" );

	UnloadModules();

	if( m_pLogger )
	{
		if( m_bUsingLocalLogger )
		{
			//TODO: need to make loggers reference counted.
			as::SetLogger( nullptr );
			m_bUsingLocalLogger = false;
		}

		m_pLogger = nullptr;
	}

	if( UsingLocalEnvironment() )
	{
		//If we're handling the engine locally, shut it down and release it.
		auto pEngine = m_Environment.GetScriptEngine();

		if( pEngine )
		{
			pEngine->ShutDownAndRelease();
		}

		m_Environment = std::move( CASSimpleEnvironment() );

		m_bUsingLocalEnvironment = false;
	}

	if( m_hGame != nullptr )
	{
		Sys_UnloadModule( m_hGame );
		m_hGame = nullptr;
		m_pGameFactory = nullptr;
	}
}

IBaseInterface* CASMod::QueryGameFactory( const char* pszName, int* pReturnCode )
{
	IBaseInterface* pInstance = nullptr;

	if( m_pGameFactory != nullptr )
	{
		pInstance = m_pGameFactory( pszName, pReturnCode );
	}
	else
	{
		if( m_hGame == nullptr )
		{
			LOG_ERROR( PLID, "Treid to query game factory with no game loaded!" );
		}

		if( pReturnCode )
			*pReturnCode = IFACE_FAILED;
	}

	return pInstance;
}

bool CASMod::LoadConfig( const char* pszConfigFilename, const bool bOptional )
{
	LOG_MESSAGE( PLID, "Loading configuration" );

	if( !pszConfigFilename || !( *pszConfigFilename ) )
	{
		LOG_ERROR( PLID, "CASMod::LoadConfig: Invalid filename" );
		return false;
	}

	char szConfigFilename[ PATH_MAX ];

	{
		const auto result = snprintf( szConfigFilename, sizeof( szConfigFilename ), "%s/%s", GetLoaderDirectory(), pszConfigFilename );

		if( !PrintfSuccess( result, sizeof( szConfigFilename ) ) )
		{
			LOG_ERROR( PLID, "Error while formatting configuration filename" );
			return false;
		}
	}

	UTIL_FixSlashes( szConfigFilename );

	kv::Parser parser( szConfigFilename );

	if( !parser.HasInputData() )
	{
		if( !bOptional )
		{
			LOG_ERROR( PLID, "Config file \"%s\" is required and could not be opened for reading", pszConfigFilename );
		}

		return false;
	}

	//Convert escape sequences.
	parser.SetEscapeSeqConversion( GetEscapeSeqConversion() );

	const auto parseResult = parser.Parse();

	if( parseResult != kv::Parser::ParseResult::SUCCESS )
	{
		LOG_ERROR( PLID, "Error while parsing config \"%s\": %s", pszConfigFilename, kv::Parser::ParseResultToString( parseResult ) );
		return false;
	}

	auto pConfig = parser.GetKeyvalues()->FindFirstChild<kv::Block>( "ASModConfig" );

	if( pConfig )
	{
		ApplyConfig( *pConfig );

		LOG_MESSAGE( PLID, "Config \"%s\" loaded", pszConfigFilename );
	}
	else
	{
		LOG_ERROR( PLID, "Config \"%s\" does not contain configuration data for ASMod", pszConfigFilename );
	}

	return true;
}

void CASMod::ApplyConfig( kv::Block& block )
{
	auto pLoader = block.FindFirstChild<kv::Block>( "loader" );

	if( pLoader )
	{
		auto pForceLocalEnvironment = pLoader->FindFirstChild<kv::KV>( "forceLocalEnvironment" );

		if( pForceLocalEnvironment )
		{
			m_bForceLocalEnvironment = atoi( pForceLocalEnvironment->GetValue().CStr() ) != 0;
		}
	}
}

bool CASMod::LoadGameModule()
{
	LOG_MESSAGE( PLID, "Loading game module" );

	const char* pszLibPath = GET_GAME_INFO( PLID, GINFO_REALDLL_FULLPATH );

	if( !pszLibPath )
	{
		LOG_ERROR( PLID, "Failed to retrieve real game path" );
		return false;
	}

	m_hGame = Sys_LoadModule( pszLibPath );

	if( m_hGame == nullptr )
	{
		LOG_ERROR( PLID, "Failed to load game module at \"%s\"", pszLibPath );
		return false;
	}

	//Very old mods won't have the factory function, so make sure it's an optional check.
	m_pGameFactory = Sys_GetFactory( m_hGame );

	LOG_MESSAGE( PLID, "Loaded game module factory: %s", m_pGameFactory ? "yes" : "no" );

	return true;
}

bool CASMod::SetupEnvironment()
{
	LOG_MESSAGE( PLID, "Setting up environment" );

	//Reset to empty.
	m_Environment = std::move( CASSimpleEnvironment() );

	bool bGotEnvironment = false;

	if( m_pLogger )
	{
		as::SetLogger( nullptr );
		m_pLogger = nullptr;
	}

	m_bUsingLocalLogger = false;

	if( !m_bForceLocalEnvironment )
	{
		if( HasGameFactory() )
		{
			LOG_MESSAGE( PLID, "Querying game for environment" );

			auto pEnvironment = static_cast<IASEnvironment*>( QueryGameFactory( IASENVIRONMENT_NAME ) );

			if( pEnvironment )
			{
				m_Environment = std::move( CASSimpleEnvironment( *pEnvironment ) );

				bGotEnvironment = true;
				m_bUsingLocalEnvironment = false;
			}
			else
			{
				LOG_MESSAGE( PLID, "Game didn't provide an environment" );
			}
		}
		else
		{
			LOG_MESSAGE( PLID, "Game didn't provide a factory" );
		}
	}
	else
	{
		LOG_MESSAGE( PLID, "Forcing use of local environment" );
	}

	if( !bGotEnvironment )
	{
		LOG_MESSAGE( PLID, "Using local environment" );

		m_Environment.SetAllocFunc( ::operator new );
		m_Environment.SetFreeFunc( ::operator delete );

		asSetGlobalMemoryFunctions( m_Environment.GetAllocFunc(), m_Environment.GetFreeFunc() );

		m_Environment.SetScriptEngine( asCreateScriptEngine() );

		//TODO: configure engine.

		m_bUsingLocalEnvironment = true;
	}

	m_pLogger = m_Environment.GetLogger();

	//Provide a logger if the game didn't.
	if( !m_pLogger )
	{
		char szLogPath[ PATH_MAX ];

		const auto result = snprintf( szLogPath, sizeof( szLogPath ), "%s/logs/LASMod", gpMetaUtilFuncs->pfnGetGameInfo( PLID, GINFO_GAMEDIR ) );

		m_pLogger = new CASFileLogger( szLogPath, CASFileLogger::Flag::USE_DATESTAMP | CASFileLogger::Flag::USE_TIMESTAMP | CASFileLogger::Flag::OUTPUT_LOG_LEVEL );

		m_bUsingLocalLogger = true;

		m_Environment.SetLogger( m_pLogger );
	}

	return m_Environment.IsValid();
}

bool CASMod::LoadModules()
{
	LOG_MESSAGE( PLID, "Loading ASMod modules" );

	const char* pszModulesFilename = ASMOD_MODULES_FILENAME;
	const bool bOptional = false;

	if( !pszModulesFilename || !( *pszModulesFilename ) )
	{
		LOG_ERROR( PLID, "CASMod::LoadModules: Invalid filename" );
		return false;
	}

	char szModulesFilename[ PATH_MAX ];

	{
		const auto result = snprintf( szModulesFilename, sizeof( szModulesFilename ), "%s/%s", GetLoaderDirectory(), pszModulesFilename );

		if( !PrintfSuccess( result, sizeof( szModulesFilename ) ) )
		{
			LOG_ERROR( PLID, "Error while formatting modules filename" );
			return false;
		}
	}

	UTIL_FixSlashes( szModulesFilename );

	kv::Parser parser( szModulesFilename );

	if( !parser.HasInputData() )
	{
		if( !bOptional )
		{
			LOG_ERROR( PLID, "Modules file \"%s\" is required and could not be opened for reading", pszModulesFilename );
		}

		return false;
	}

	//Convert escape sequences.
	parser.SetEscapeSeqConversion( GetEscapeSeqConversion() );

	const auto parseResult = parser.Parse();

	if( parseResult != kv::Parser::ParseResult::SUCCESS )
	{
		LOG_ERROR( PLID, "Error while parsing modules file \"%s\": %s", pszModulesFilename, kv::Parser::ParseResultToString( parseResult ) );
		return false;
	}

	auto pModules = parser.GetKeyvalues()->FindFirstChild<kv::Block>( "ASModModules" );

	if( pModules )
	{
		if( LoadModulesFromBlock( *pModules ) )
		{
			LOG_MESSAGE( PLID, "Modules loaded from \"%s\"", pszModulesFilename );
		}
		else
		{
			return false;
		}
	}
	else
	{
		LOG_ERROR( PLID, "Modules file \"%s\" does not contain a module list for ASMod", pszModulesFilename );
	}

	return true;
}

bool CASMod::LoadModulesFromBlock( kv::Block& block )
{
	auto pList = block.FindFirstChild<kv::Block>( "moduleList" );

	if( !pList )
	{
		LOG_ERROR( PLID, "Couldn't find a \"moduleList\" entry in the modules file" );
		return false;
	}

	size_t uiLoaded = 0;

	for( const auto pNode : pList->GetChildren() )
	{
		if( pNode->GetType() != kv::NodeType::BLOCK )
		{
			LOG_DEVELOPER( PLID, "Encountered non-block node \"%s\", ignoring", pNode->GetKey().CStr() );
			continue;
		}

		if( pNode->GetKey() != "module" )
		{
			LOG_DEVELOPER( PLID, "Encountered block with unsupported name \"%s\", should be \"module\", ignoring", pNode->GetKey().CStr() );
			continue;
		}

		auto pModule = static_cast<kv::Block*>( pNode );

		if( LoadModuleFromBlock( *pModule ) )
			++uiLoaded;
	}

	LOG_MESSAGE( PLID, "Loaded %u modules, %u total", uiLoaded, m_Modules.size() );

	return true;
}

bool CASMod::LoadModuleFromBlock( kv::Block& block )
{
	auto pModulePath = block.FindFirstChild<kv::KV>( "path" );

	if( !pModulePath )
	{
		LOG_ERROR( PLID, "Couldn't find \"path\" key in module block" );
		return false;
	}

	char szModuleFilename[ PATH_MAX ];

	{
		const auto result = snprintf( szModuleFilename, sizeof( szModuleFilename ), "%s/%s/%s%s", 
									  GetLoaderDirectory(), ASMOD_MODULES_DIR, pModulePath->GetValue().CStr(), PLATFORM_DLEXT );

		if( !PrintfSuccess( result, sizeof( szModuleFilename ) ) )
		{
			LOG_ERROR( PLID, "An error occurred while formatting the module filename" );
			return false;
		}
	}

	CASModModuleInfo info;

	if( !info.Load( szModuleFilename ) )
	{
		return false;
	}

	if( !info.GetModule()->Initialize( GetEnvironment() ) )
	{
		LOG_ERROR( PLID, "Failed to initialize module \"%s\"", info.GetModule()->GetName() );
		return false;
	}

	m_Modules.emplace_back( std::move( info ) );

	return true;
}

void CASMod::UnloadModules()
{
	for( auto& module : m_Modules )
	{
		if( !module.Shutdown() )
		{
			LOG_ERROR( PLID, "Error while shutting down module \"%s\"", module.GetModule()->GetName() );
		}

		module.Unload();
	}

	m_Modules.clear();
}
