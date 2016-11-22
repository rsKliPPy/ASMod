#ifndef STUB_MODULE_CASSTUBMODULE_H
#define STUB_MODULE_CASSTUBMODULE_H

#include "ASMod/IASModModule.h"

class CASStubModule : public IASModModule
{
public:
	CASStubModule() = default;
	~CASStubModule() = default;

	const char* GetName() const override final;

	bool Initialize( IASEnvironment& environment ) override;

	bool Shutdown() override;

	/**
	*	@return The environment.
	*/
	IASEnvironment& GetEnvironment() { return *m_pEnvironment; }

private:
	IASEnvironment* m_pEnvironment = nullptr;

private:
	CASStubModule( const CASStubModule& ) = delete;
	CASStubModule& operator=( const CASStubModule& ) = delete;
};

#endif //STUB_MODULE_CASSTUBMODULE_H
