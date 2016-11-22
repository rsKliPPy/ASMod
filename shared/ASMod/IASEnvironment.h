#ifndef ASMOD_IASENVIRONMENT_H
#define ASMOD_IASENVIRONMENT_H

#include "interface.h"

#include <angelscript.h>

#include <Angelscript/util/IASLogger.h>

/**
*	Represents an Angelscript environment that can be used as a base by the loader.
*/
class IASEnvironment : public IBaseInterface
{
public:

	/**
	*	@return The Angelscript engine.
	*/
	virtual asIScriptEngine* GetScriptEngine() = 0;
	
	/**
	*	@return The allocation function.
	*/
	virtual asALLOCFUNC_t GetAllocFunc() = 0;

	/**
	*	@return The free function.
	*/
	virtual asFREEFUNC_t GetFreeFunc() = 0;

	/**
	*	@return The array allocation function.
	*/
	virtual asALLOCFUNC_t GetArrayAllocFunc() = 0;

	/**
	*	@return The array free function.
	*/
	virtual asFREEFUNC_t GetArrayFreeFunc() = 0;

	/**
	*	@return The logger provided by the environment. Can be null.
	*/
	virtual IASLogger* GetLogger() = 0;
};

/**
*	Interface name.
*/
#define IASENVIRONMENT_NAME "IASEnvironmentV001"

#endif //ASMOD_IASENVIRONMENT_H
