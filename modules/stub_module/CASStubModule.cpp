#include "interface.h"

#include <Angelscript/util/IASLogger.h>

#include "ASMod/IASEnvironment.h"

#include "CASStubModule.h"

EXPOSE_SINGLE_INTERFACE( CASStubModule, IASModModule, IASMODMODULE_NAME );

const char* CASStubModule::GetName() const
{
	return "stub_module";
}

bool CASStubModule::Initialize( IASEnvironment& environment )
{
	m_pEnvironment = &environment;

	m_pEnvironment->GetLogger()->Diagnostic( "Initializing module\n" );

	return true;
}

bool CASStubModule::Shutdown()
{
	m_pEnvironment->GetLogger()->Diagnostic( "Shutting down module\n" );

	m_pEnvironment = nullptr;

	return true;
}
