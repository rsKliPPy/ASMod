#include <extdll.h>
#include <meta_api.h>

#include "Module.h"

#include "CASModBaseModule.h"

bool CASModBaseModule::Initialize( const CreateInterfaceFn* UNREFERENCED( pFactories ), const size_t UNREFERENCED( uiNumFactories ),
								   IASEnvironment& environment,
								   enginefuncs_t* UNREFERENCED( pEngineFuncs ),
								   globalvars_t* UNREFERENCED( pGlobals ),
								meta_globals_t* UNREFERENCED( pMetaGlobals ),
								gamedll_funcs_t* UNREFERENCED( pGamedllFuncs ),
								mutil_funcs_t* UNREFERENCED( pMetaUtilFuncs ) )
{
	g_pModule = this;

	m_pEnvironment = &environment;

	return true;
}

bool CASModBaseModule::Shutdown()
{
	m_pEnvironment = nullptr;

	g_pModule = nullptr;

	return true;
}