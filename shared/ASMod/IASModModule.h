#ifndef ASMOD_IASMODMODULE_H
#define ASMOD_IASMODMODULE_H

#include "interface.h"

class IASEnvironment;
typedef struct enginefuncs_s enginefuncs_t;
struct globalvars_t;
typedef struct meta_globals_s meta_globals_t;
struct gamedll_funcs_t;
typedef struct meta_util_funcs_s mutil_funcs_t;

/**
*	Represents an ASMod Module.
*/
class IASModModule : public IBaseInterface
{
public:

	/**
	*	@return The name of this module.
	*/
	virtual const char* GetName() const = 0;

	/**
	*	Initializes the module.
	*	@param environment Environment.
	*	@return Whether initialization succeeded.
	*/
	virtual bool Initialize( IASEnvironment& environment,
							 enginefuncs_t* pEngineFuncs,
							 globalvars_t* pGlobals,
							 meta_globals_t* pMetaGlobals,
							 gamedll_funcs_t* pGamedllFuncs,
							 mutil_funcs_t* pMetaUtilFuncs ) = 0;

	/**
	*	Shuts down the module.
	*	@return Whether shutdown succeeded.
	*/
	virtual bool Shutdown() = 0;

	/**
	*	Called every frame.
	*/
	virtual void Think() {}
};

/**
*	Interface name.
*/
#define IASMODMODULE_NAME "IASModModuleV001"

#endif //ASMOD_IASMODMODULE_H
