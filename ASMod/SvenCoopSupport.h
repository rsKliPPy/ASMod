#ifndef ASMOD_SVENCOOPSUPPORT_H
#define ASMOD_SVENCOOPSUPPORT_H

#include <angelscript.h>

namespace sc
{
/**
*	Dummy version of Sven Co-op's Angelscript implementation factory.
*/
class IASImplementationFactory
{
public:
	virtual ~IASImplementationFactory() = 0;
};

/**
*	Dummy version of Sven Co-op's Angelscript manager to access its data members.
*/
class CASManager
{
public:
	//Has virtual functions.
	virtual ~CASManager() = 0;

	//These 3 member variables.
	bool	m_fBuildComplete;
	bool	m_fDidMapScriptCompilationSucceed;
	bool	m_fIsReady;

	IASImplementationFactory* m_pImplFactory;

	//The million dollar address.
	asIScriptEngine* m_pScriptEngine;

	//We don't need the rest.
};

/**
*	Provides support for Sven Co-op's Angelscript API.
*/
class CSvenCoopSupport final
{
private:
	using ManagerFunc = CASManager* ( * )();

public:
	/**
	*	Loads the configuration for Sven Co-op for the current platform from the given config file.
	*/
	CSvenCoopSupport( const char* pszConfigFilename );

	/**
	*	@return Whether the config was loaded successfully.
	*/
	bool IsConfigLoaded() const { return m_bLoadedConfig; }

	asALLOCFUNC_t GetAllocFunc() { return m_AllocFunc; }

	asFREEFUNC_t GetFreeFunc() { return m_FreeFunc; }

	asALLOCFUNC_t GetArrayAllocFunc() { return m_ArrayAllocFunc; }

	asFREEFUNC_t GetArrayFreeFunc() { return m_ArrayFreeFunc; }

	/**
	*	Queries the server for its script engine.
	*	@param[ out ] pOutScriptEngine Pointer to the script engine, or null if it could not be retrieved.
	*	@return Whether the script engine was successfully retrieved.
	*/
	bool GetScriptEngine( asIScriptEngine*& pOutScriptEngine );

private:
	bool m_bLoadedConfig = false;

	asALLOCFUNC_t m_AllocFunc = nullptr;
	asFREEFUNC_t m_FreeFunc = nullptr;
	asALLOCFUNC_t m_ArrayAllocFunc = nullptr;
	asFREEFUNC_t m_ArrayFreeFunc = nullptr;
	ManagerFunc m_ManagerFunc = nullptr;

private:
	CSvenCoopSupport( const CSvenCoopSupport& ) = delete;
	CSvenCoopSupport& operator=( const CSvenCoopSupport& ) = delete;
};
}

#endif //ASMOD_SVENCOOPSUPPORT_H
