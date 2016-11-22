#include <extdll.h>
#include <meta_api.h>

#include "interface.h"

#include <Angelscript/util/IASLogger.h>
#include <Angelscript/wrapper/CASContext.h>
#include <Angelscript/ScriptAPI/SQL/CASSQLThreadPool.h>

#include "ASMod/IASEnvironment.h"

#include "Module.h"

#include "ASHLSQL.h"

#include "CASSQLModule.h"

EXPOSE_SINGLE_INTERFACE( CASSQLModule, IASModModule, IASMODMODULE_NAME );

const char* CASSQLModule::GetName() const
{
	return "SQL";
}

bool CASSQLModule::Initialize( IASEnvironment& environment,
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

	CVAR_REGISTER( &as_mysql_config );

	auto& scriptEngine = *m_pEnvironment->GetScriptEngine();

	RegisterScriptHLSQL( scriptEngine );

	return true;
}

bool CASSQLModule::Shutdown()
{
	g_pSQLThreadPool->Stop( false );

	m_pEnvironment = nullptr;

	return true;
}

void CASSQLModule::Think()
{
	CASOwningContext ctx( *GetEnvironment().GetScriptEngine() );
	g_pSQLThreadPool->ProcessQueue( *ctx.GetContext() );
}
