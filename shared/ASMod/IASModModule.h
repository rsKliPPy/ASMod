#ifndef ASMOD_IASMODMODULE_H
#define ASMOD_IASMODMODULE_H

#include "interface.h"

class IASEnvironment;

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
	virtual bool Initialize( IASEnvironment& environment ) = 0;

	/**
	*	Shuts down the module.
	*	@return Whether shutdown succeeded.
	*/
	virtual bool Shutdown() = 0;
};

/**
*	Interface name.
*/
#define IASMODMODULE_NAME "IASModModuleV001"

#endif //ASMOD_IASMODMODULE_H
