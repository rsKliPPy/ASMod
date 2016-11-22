#include <cstdint>

#include <extdll.h>
#include <meta_api.h>

#include "keyvalues/Keyvalues.h"

#include "CASMod.h"

#include "SvenCoopSupport.h"

namespace sc
{
template<typename TYPE>
bool ParseAddress( const char* pszAddress, const char* pszName, TYPE& pOutPointer )
{
	pOutPointer = nullptr;

	long long iValue = strtoll( pszAddress, nullptr, 16 );

	auto pPointer = reinterpret_cast<TYPE>( iValue );

	if( !pPointer )
	{
		LOG_ERROR( PLID, "Memory address \"%p\" for \"%s\" is invalid", pPointer, pszName );
		return false;
	}

	pOutPointer = pPointer;

	return true;
}

template<typename TYPE>
TYPE OffsetAddress( TYPE address, ptrdiff_t offset )
{
	return reinterpret_cast<TYPE>( reinterpret_cast<uint8_t*>( address ) + offset );
}

CSvenCoopSupport::CSvenCoopSupport( const char* pszConfigFilename )
{
	LOG_MESSAGE( PLID, "Loading Sven Co-op support config \"%s\"", pszConfigFilename );
	char szPath[ PATH_MAX ];

	const auto result = snprintf( szPath, sizeof( szPath ), "%s/%s", g_ASMod.GetLoaderDirectory(), pszConfigFilename );

	if( !PrintfSuccess( result, sizeof( szPath ) ) )
	{
		LOG_ERROR( PLID, "Error while formatting config filename" );
		return;
	}

	kv::Parser parser( szPath );

	if( !parser.HasInputData() )
	{
		LOG_ERROR( PLID, "Couldn't read from config" );
		return;
	}

	parser.SetEscapeSeqConversion( GetEscapeSeqConversion() );

	const auto parseResult = parser.Parse();

	if( parseResult != kv::Parser::ParseResult::SUCCESS )
	{
		LOG_ERROR( PLID, "Error while parsing config: %s", kv::Parser::ParseResultToString( parseResult ) );
		return;
	}

	auto pConfig = parser.GetKeyvalues()->FindFirstChild<kv::Block>( "ASModSCSupport" );

	if( !pConfig )
	{
		LOG_ERROR( PLID, "Config does not contain Sven Co-op support data \"ASModSCSupport\"" );
		return;
	}

	//Don't need to parse out addresses for Linux.
#ifdef WIN32
	auto pPlatform = pConfig->FindFirstChild<kv::Block>( PLATFORM );

	if( !pPlatform )
	{
		LOG_ERROR( PLID, "Sven Co-op support config does not contain support for platform \"%s\"", PLATFORM );
		return;
	}

	auto pAlloc = pPlatform->FindFirstChild<kv::KV>( "allocFunc" );
	auto pFree = pPlatform->FindFirstChild<kv::KV>( "freeFunc" );
	auto pManager = pPlatform->FindFirstChild<kv::KV>( "managerFunc" );

	if( !pAlloc || !pFree || !pManager )
	{
		LOG_ERROR( PLID, "Missing function address for one or more required functions" );
		return;
	}

	if( !ParseAddress( pAlloc->GetValue().CStr(), pAlloc->GetKey().CStr(), m_AllocFunc ) ||
		!ParseAddress( pFree->GetValue().CStr(), pFree->GetKey().CStr(), m_FreeFunc ) ||
		!ParseAddress( pManager->GetValue().CStr(), pManager->GetKey().CStr(), m_ManagerFunc ) )
	{
		return;
	}

	//On Windows, the module handle is also the base address. - Solokiller
	const auto offset = reinterpret_cast<ptrdiff_t>( g_ASMod.GetGameModuleHandle() );

	//Now offset the addresses to their actual address.
	m_AllocFunc = OffsetAddress( m_AllocFunc, offset );
	m_FreeFunc = OffsetAddress( m_FreeFunc, offset );
	m_ManagerFunc = OffsetAddress( m_ManagerFunc, offset );
#else
	//On Linux we can just dlsym what we need.
	//These are actually operator new and operator delete
	m_AllocFunc = reinterpret_cast<asALLOCFUNC_t>( dlsym( g_ASMod.GetGameModuleHandle(), "_Znwj" ) );
	m_FreeFunc = reinterpret_cast<asFREEFUNC_t>( dlsym( g_ASMod.GetGameModuleHandle(), "_ZdlPv" ) );
	m_ManagerFunc = reinterpret_cast<ManagerFunc>( dlsym( g_ASMod.GetGameModuleHandle(), "_ZN16CASServerManager11GetInstanceEv" ) );
#endif

	if( !m_AllocFunc ||
		!m_FreeFunc ||
		!m_ManagerFunc )
	{
		LOG_ERROR( PLID, "Failed to retrieve one or more required functions" );
		return;
	}

	//Test code for the allocator functions.
	//int* pData = reinterpret_cast<int*>( m_AllocFunc( sizeof( int ) ) );
	//*pData = 1234;
	//m_FreeFunc( pData );

	LOG_MESSAGE( PLID, "Successfully parsed Sven Co-op support config" );

	m_bLoadedConfig = true;
}

bool CSvenCoopSupport::GetScriptEngine( asIScriptEngine*& pOutScriptEngine )
{
	pOutScriptEngine = nullptr;

	if( !m_ManagerFunc )
	{
		LOG_ERROR( PLID, "CSvenCoopSupport::GetScriptEngine: No manager function!" );
		return false;
	}

	auto pManager = m_ManagerFunc();

	if( !pManager )
	{
		LOG_ERROR( PLID, "CSvenCoopSupport::GetScriptEngine: No manager!" );
		return false;
	}

	pOutScriptEngine = pManager->m_pScriptEngine;

	if( !pOutScriptEngine )
	{
		LOG_ERROR( PLID, "CSvenCoopSupport::GetScriptEngine: No script engine!" );
		return false;
	}

	return true;
}
}
