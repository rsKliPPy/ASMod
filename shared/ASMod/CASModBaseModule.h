#ifndef ASMOD_CASMODBASEMODULE_H
#define ASMOD_CASMODBASEMODULE_H

#include "IASModModule.h"

/**
*	Base class for ASMod modules.
*/
class CASModBaseModule : public IASModModule
{
public:
	CASModBaseModule() = default;
	virtual ~CASModBaseModule() = default;

	bool Initialize( const CreateInterfaceFn* pFactories, const size_t uiNumFactories,
					 IASEnvironment& environment,
					 enginefuncs_t* pEngineFuncs,
					 globalvars_t* pGlobals,
					 meta_globals_t* pMetaGlobals,
					 gamedll_funcs_t* pGamedllFuncs,
					 mutil_funcs_t* pMetaUtilFuncs ) override;

	bool Shutdown() override;

	/**
	*	@return The Angelscript environment.
	*/
	IASEnvironment& GetEnvironment() { return *m_pEnvironment; }

protected:
	IASEnvironment* m_pEnvironment = nullptr;

private:
	CASModBaseModule( const CASModBaseModule& ) = delete;
	CASModBaseModule& operator=( const CASModBaseModule& ) = delete;
};

#endif //ASMOD_CASMODBASEMODULE_H
