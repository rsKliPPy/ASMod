#ifndef ASMOD_CASMOD_H
#define ASMOD_CASMOD_H

#include <vector>

#include "interface.h"

#include "osdep.h"

//TODO: clean up - Solokiller
#undef VOID

#include <Angelscript/util/CASRefPtr.h>

#include "ASMod/CASSimpleEnvironment.h"
#include "ASMod/IASMod.h"

#include "keyvalues/KVForward.h"

#define ASMOD_CONFIG_FILENAME "Config.txt"
#define ASMOD_MODULES_DIR "modules"
#define ASMOD_MODULES_FILENAME "Modules.txt"

#define ASMOD_SCSUPPORT_FILENAME "SvenCoopSupport.txt"

class CASModModuleInfo;
class IASLogger;

class CASMod final : public IASMod
{
private:
	using Modules_t = std::vector<CASModModuleInfo>;

	/**
	*	Environment types.
	*/
	enum class EnvType
	{
		/**
		*	Query game for environment, fall back to local if none found.
		*/
		DEFAULT = 0,

		/**
		*	Always use local environment.
		*/
		LOCAL,

		/**
		*	Special support for Sven Co-op: hack into the server and acquire required environment manually.
		*/
		SVENCOOP_HACK,
	};

public:
	CASMod() = default;
	~CASMod() = default;

	const char* GetLoaderDirectory() const override final { return m_szLoaderDir; }

	CSysModule* GetGameModuleHandle() override final { return m_hGame; }

	bool Initialize();

	void Shutdown();

	void Think();

	bool HasGameFactory() const override final { return m_pGameFactory != nullptr; }

	bool UsingLocalEnvironment() const override final { return m_bUsingLocalEnvironment; }

	CASSimpleEnvironment& GetEnvironment() override final { return m_Environment; }

	IBaseInterface* QueryGameFactory( const char* pszName, int* pReturnCode = nullptr ) override final;

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
	EnvType m_EnvType = EnvType::DEFAULT;

private:
	CASMod( const CASMod& ) = delete;
	CASMod& operator=( const CASMod& ) = delete;
};

extern CASMod g_ASMod;

#endif //ASMOD_CASMOD_H
