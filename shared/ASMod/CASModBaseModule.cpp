#include <cassert>

#include <extdll.h>
#include <meta_api.h>

#include "interface.h"

#include "ASMod/IASMod.h"
#include "ASMod/IASEnvironment.h"

#include "InterfaceHelpers.h"
#include "ASMod/CreateInterface_api.h"

#include "Module.h"

#include "CASModBaseModule.h"

bool CASModBaseModule::Initialize( const CreateInterfaceFn* pFactories, const size_t uiNumFactories )
{
	g_pModule = this;

	m_pASMod = IFACE_CreateFromList<IASMod*>( pFactories, uiNumFactories, IASMOD_NAME );

	//Can't log anything yet, so this is a silent error.
	if( !m_pASMod )
	{
		assert( false );
		return false;
	}

	m_pEnvironment = &m_pASMod->GetEnvironment();

	auto pEngFuncs = IFACE_CreateCStyleFromList<enginefuncs_t*>( pFactories, uiNumFactories, ENGINEFUNCS_T_NAME );

	if( !pEngFuncs )
	{
		GetEnvironment().GetLogger()->Critical( "Couldn't get engine functions \"%s\" from ASMod!\n", ENGINEFUNCS_T_NAME );
		return false;
	}

	memcpy( &g_engfuncs, pEngFuncs, sizeof( g_engfuncs ) );

	gpGlobals = IFACE_CreateCStyleFromList<globalvars_t*>( pFactories, uiNumFactories, GLOBALVARS_T_NAME );

	gpMetaGlobals = IFACE_CreateCStyleFromList<meta_globals_t*>( pFactories, uiNumFactories, META_GLOBALS_T_NAME );

	gpGamedllFuncs = IFACE_CreateCStyleFromList<gamedll_funcs_t*>( pFactories, uiNumFactories, GAMEDLL_FUNCS_T_NAME );

	gpMetaUtilFuncs = IFACE_CreateCStyleFromList<mutil_funcs_t*>( pFactories, uiNumFactories, MUTIL_FUNCS_T_NAME );

	if( !gpGlobals || !gpMetaGlobals || !gpGamedllFuncs || !gpMetaUtilFuncs )
	{
		GetEnvironment().GetLogger()->Critical( "Couldn't get one or more interfaces from ASMod!\n" );
		return false;
	}

	return true;
}

bool CASModBaseModule::Shutdown()
{
	gpMetaUtilFuncs = nullptr;
	gpGamedllFuncs = nullptr;
	gpMetaGlobals = nullptr;
	gpGlobals = nullptr;
	memset( &g_engfuncs, 0, sizeof( g_engfuncs ) );

	m_pEnvironment = nullptr;

	m_pASMod = nullptr;

	g_pModule = nullptr;

	return true;
}