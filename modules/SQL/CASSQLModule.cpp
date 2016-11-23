#include <extdll.h>
#include <meta_api.h>

#include "interface.h"

#include <Angelscript/util/IASLogger.h>
#include <Angelscript/wrapper/CASContext.h>
#include <Angelscript/ScriptAPI/SQL/CASSQLThreadPool.h>
#include <Angelscript/add_on/scriptstdstring.h>

#include "ASMod/IASEnvironment.h"

#include "Module.h"

#include "ASHLSQL.h"

#include "utility/CString.h"

#include "CASSQLModule.h"

CASSQLModule g_Module;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CASSQLModule, IASModModule, IASMODMODULE_NAME, g_Module );

std::string CStringToStdString( const CString* pszString )
{
	return pszString->CStr();
}

const char* CASSQLModule::GetName() const
{
	return "SQL";
}

bool CASSQLModule::Initialize( const CreateInterfaceFn* pFactories, const size_t uiNumFactories )
{
	if( !BaseClass::Initialize( pFactories, uiNumFactories ) )
		return false;

	CVAR_REGISTER( &as_mysql_config );

	auto& scriptEngine = *m_pEnvironment->GetScriptEngine();

	//TODO: temporary
	RegisterStdString( &scriptEngine, false );
	scriptEngine.RegisterObjectMethod(
		"string", "stdString opImplConv() const", asFUNCTION( CStringToStdString ), asCALL_CDECL_OBJFIRST );
	//TODO: SQL api uses std::string, should be CString
	RegisterScriptHLSQL( scriptEngine );

	return true;
}

bool CASSQLModule::Shutdown()
{
	g_pSQLThreadPool->Stop( false );

	m_pEnvironment = nullptr;

	return BaseClass::Shutdown();
}

void CASSQLModule::Think()
{
	CASOwningContext ctx( *GetEnvironment().GetScriptEngine() );
	g_pSQLThreadPool->ProcessQueue( *ctx.GetContext() );
}
