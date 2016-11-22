#include <memory>

#include <extdll.h>
#include <meta_api.h>

#include "interface.h"

#include "ASMod/IASModModule.h"

#include "CASModModuleInfo.h"

CASModModuleInfo::~CASModModuleInfo()
{
	if( m_pModule )
	{
		LOG_ERROR( PLID, "Module interface is still loaded in destructor!" );
		m_pModule = nullptr;
	}

	if( m_hHandle )
	{
		LOG_ERROR( PLID, "Module is still loaded in destructor!" );
		Sys_UnloadModule( m_hHandle );
		m_hHandle = nullptr;
	}
}

CASModModuleInfo::CASModModuleInfo( CASModModuleInfo&& other )
{
	std::swap( m_hHandle, other.m_hHandle );
	std::swap( m_pModule, other.m_pModule );
}

CASModModuleInfo& CASModModuleInfo::operator=( CASModModuleInfo&& other )
{
	if( this != &other )
	{
		std::swap( m_hHandle, other.m_hHandle );
		std::swap( m_pModule, other.m_pModule );
	}

	return *this;
}

bool CASModModuleInfo::Load( const char* pszFilename )
{
	if( IsLoaded() )
	{
		LOG_ERROR( PLID, "CASModModuleInfo::Load: Already loaded!" );
		return false;
	}

	//Note: std::default_delete asserts due to CSysModule not being defined, so the deleter type is required.
	std::unique_ptr<CSysModule, void( *)( CSysModule* )> handle(
		Sys_LoadModule( pszFilename ),
		Sys_UnloadModule
	);

	if( !handle )
	{
		LOG_ERROR( PLID, "CASModModuleInfo::Load: Failed to load module \"%s\"", pszFilename );
		return false;
	}

	auto pFactory = Sys_GetFactory( handle.get() );

	if( !pFactory )
	{
		LOG_ERROR( PLID, "CASModModuleInfo::Load: Failed to get factory from module \"%s\"", pszFilename );
		return false;
	}

	m_pModule = static_cast<IASModModule*>( pFactory( IASMODMODULE_NAME, nullptr ) );

	if( !m_pModule )
	{
		LOG_ERROR( PLID, "CASModModuleInfo::Load: Failed to get module interface from module \"%s\"", pszFilename );
		return false;
	}

	m_hHandle = handle.release();

	LOG_MESSAGE( PLID, "Loaded module \"%s\"", pszFilename );

	return true;
}

bool CASModModuleInfo::Shutdown()
{
	if( !m_pModule )
	{
		LOG_ERROR( PLID, "CASModModuleInfo::Shutdown::Can't shutdown null module!" );
		return false;
	}

	const auto bSuccess = m_pModule->Shutdown();

	if( !bSuccess )
	{
		LOG_ERROR( PLID, "Module \"%s\" encountered an error while shutting down", m_pModule->GetName() );
	}

	return bSuccess;
}

bool CASModModuleInfo::Unload()
{
	if( m_hHandle == nullptr )
	{
		LOG_ERROR( PLID, "CASModModuleInfo::Unload: Can't unload empty module!" );
		return false;
	}

	m_pModule = nullptr;

	Sys_UnloadModule( m_hHandle );
	m_hHandle = nullptr;

	return true;
}
