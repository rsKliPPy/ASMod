#ifndef ASMOD_CASMOD_H
#define ASMOD_CASMOD_H

#include <vector>

#include "interface.h"

#include "osdep.h"

//TODO: clean up - Solokiller
#undef VOID

#include <Angelscript/util/CASRefPtr.h>

#include "ASMod/CASSimpleEnvironment.h"

#include "keyvalues/KVForward.h"

#define ASMOD_CONFIG_FILENAME "Config.txt"
#define ASMOD_MODULES_DIR "modules"
#define ASMOD_MODULES_FILENAME "Modules.txt"

class CASModModuleInfo;
class IASLogger;

class CASMod final
{
private:
	using Modules_t = std::vector<CASModModuleInfo>;

public:
	CASMod() = default;
	~CASMod() = default;

	/**
	*	The loader directory. This is the base directory for this Metamod plugin, and where we'll start to look for configs & modules.
	*/
	const char* GetLoaderDirectory() const { return m_szLoaderDir; }

	bool Initialize();

	void Shutdown();

	/**
	*	@return Whether the game factory is available.
	*/
	bool HasGameFactory() const { return m_pGameFactory != nullptr; }

	/**
	*	@return Whether we're using a local environment.
	*/
	bool UsingLocalEnvironment() const { return m_bUsingLocalEnvironment; }

	/**
	*	@return The Angelscript environment.
	*/
	CASSimpleEnvironment& GetEnvironment() { return m_Environment; }

	/**
	*	Queries the game factory. If no factory was loaded, returns null and sets pReturnCode to IFACE_FAILED if it was provided.
	*/
	IBaseInterface* QueryGameFactory( const char* pszName, int* pReturnCode = nullptr );

private:
	/**
	*	Loads the loader configuration.
	*	@param pszConfigFilename Configuration filename. Starts in addons/ASMod/.
	*	@param bOptional Whether this is an optional config file. If not, logs an error if the file doesn't exist.
	*	@return Whether the config file was successfully read.
	*/
	bool LoadConfig( const char* pszConfigFilename, const bool bOptional );

	/**
	*	Applies the configuration found in block.
	*/
	void ApplyConfig( kv::Block& block );

	/**
	*	Loads the game module.
	*	@return Whether the module was successfully loaded.
	*/
	bool LoadGameModule();

	/**
	*	Sets up the environment.
	*	@return Whether the environment was successfully set up.
	*/
	bool SetupEnvironment();

	/**
	*	Loads all modules.
	*	@return Whether module loading succeeded without critical errors.
	*/
	bool LoadModules();

	/**
	*	Loads modules data from the given block.
	*	@return Whether modules loading succeeded without errors.
	*/
	bool LoadModulesFromBlock( kv::Block& block );

	/**
	*	Loads a module from a block.
	*	@return Whether module loading succeeded without errors.
	*/
	bool LoadModuleFromBlock( kv::Block& block );

	/**
	*	Unloads all modules.
	*/
	void UnloadModules();

private:
	char m_szLoaderDir[ PATH_MAX ] = {};

	CSysModule* m_hGame = nullptr;
	CreateInterfaceFn m_pGameFactory = nullptr;

	bool m_bUsingLocalEnvironment = false;

	CASSimpleEnvironment m_Environment;

	IASLogger* m_pLogger = nullptr;

	bool m_bUsingLocalLogger = false;

	Modules_t m_Modules;

	//Configuration
	bool m_bForceLocalEnvironment = false;

private:
	CASMod( const CASMod& ) = delete;
	CASMod& operator=( const CASMod& ) = delete;
};

extern CASMod g_ASMod;

#endif //ASMOD_CASMOD_H
