#include <extdll.h>
#include <meta_api.h>

#include "interface.h"

#include <Angelscript/util/IASLogger.h>

#include "ASMod/IASEnvironment.h"

#include "CASStubModule.h"

EXPOSE_SINGLE_INTERFACE( CASStubModule, IASModModule, IASMODMODULE_NAME );

const char* CASStubModule::GetName() const
{
	return "stub_module";
}

bool CASStubModule::Initialize( IASEnvironment& environment,
								enginefuncs_t* pEngineFuncs,
								globalvars_t* pGlobals,
								meta_globals_t* pMetaGlobals,
								gamedll_funcs_t* pGamedllFuncs,
								mutil_funcs_t* pMetaUtilFuncs )
{
	m_pEnvironment = &environment;

	memcpy( &g_engfuncs, pEngineFuncs, sizeof( g_engfuncs ) );
	gpGlobals = pGlobals;

	gpMetaGlobals = pMetaGlobals;
	gpGamedllFuncs = pGamedllFuncs;
	gpMetaUtilFuncs = pMetaUtilFuncs;

	m_pEnvironment->GetLogger()->Diagnostic( "Initializing module\n" );

	return true;
}

bool CASStubModule::Shutdown()
{
	m_pEnvironment->GetLogger()->Diagnostic( "Shutting down module\n" );

	m_pEnvironment = nullptr;

	return true;
}
