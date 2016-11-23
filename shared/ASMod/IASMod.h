#ifndef ASMOD_IASMOD_H
#define ASMOD_IASMOD_H

#include "interface.h"

class IASEnvironment;

/**
*	Interface to ASMod.
*/
class IASMod : public IBaseInterface
{
public:
	/**
	*	The loader directory. This is the base directory for this Metamod plugin, and where we'll start to look for configs & modules.
	*/
	virtual const char* GetLoaderDirectory() const  = 0;

	/**
	*	@return The game's module handle.
	*/
	virtual CSysModule* GetGameModuleHandle()  = 0;

	/**
	*	@return Whether the game factory is available.
	*/
	virtual bool HasGameFactory() const = 0;

	/**
	*	@return Whether we're using a local environment.
	*/
	virtual bool UsingLocalEnvironment() const = 0;

	/**
	*	@return The Angelscript environment.
	*/
	virtual IASEnvironment& GetEnvironment() = 0;

	/**
	*	Queries the game factory. If no factory was loaded, returns null and sets pReturnCode to IFACE_FAILED if it was provided.
	*	@see HasGameFactory
	*/
	virtual IBaseInterface* QueryGameFactory( const char* pszName, int* pReturnCode = nullptr ) = 0;
};

/**
*	Interface name for IASMod.
*/
#define IASMOD_NAME "IASModV001"

#endif //ASMOD_IASMOD_H
