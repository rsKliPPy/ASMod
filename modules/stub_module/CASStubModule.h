#ifndef STUB_MODULE_CASSTUBMODULE_H
#define STUB_MODULE_CASSTUBMODULE_H

#include "ASMod/CASModBaseModule.h"

class CASStubModule : public CASModBaseModule
{
public:
	typedef CASStubModule ThisClass;
	typedef CASModBaseModule BaseClass;

public:
	CASStubModule() = default;
	~CASStubModule() = default;

	const char* GetName() const override final;

	bool Initialize( const CreateInterfaceFn* pFactories, const size_t uiNumFactories ) override;

	bool Shutdown() override;

private:
	CASStubModule( const CASStubModule& ) = delete;
	CASStubModule& operator=( const CASStubModule& ) = delete;
};

#endif //STUB_MODULE_CASSTUBMODULE_H
