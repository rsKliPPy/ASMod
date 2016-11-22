#ifndef STUB_MODULE_CASSQLMODULE_H
#define STUB_MODULE_CASSQLMODULE_H

#include "ASMod/IASModModule.h"

class CASSQLModule : public IASModModule
{
public:
	CASSQLModule() = default;
	~CASSQLModule() = default;

	const char* GetName() const override final;

	bool Initialize( IASEnvironment& environment,
					 enginefuncs_t* pEngineFuncs,
					 globalvars_t* pGlobals,
					 meta_globals_t* pMetaGlobals,
					 gamedll_funcs_t* pGamedllFuncs,
					 mutil_funcs_t* pMetaUtilFuncs ) override;

	bool Shutdown() override;

	/**
	*	@return The environment.
	*/
	IASEnvironment& GetEnvironment() { return *m_pEnvironment; }

	void Think() override;

private:
	IASEnvironment* m_pEnvironment = nullptr;

private:
	CASSQLModule( const CASSQLModule& ) = delete;
	CASSQLModule& operator=( const CASSQLModule& ) = delete;
};

#endif //STUB_MODULE_CASSQLMODULE_H
