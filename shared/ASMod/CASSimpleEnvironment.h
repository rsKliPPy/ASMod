#ifndef ASMOD_CASSIMPLEENVIRONMENT_H
#define ASMOD_CASSIMPLEENVIRONMENT_H

#include <Angelscript/util/CASRefPtr.h>

#include "ASMod/IASEnvironment.h"

/**
*	A simple environment where all environment members are class members.
*/
class CASSimpleEnvironment : public IASEnvironment
{
public:
	CASSimpleEnvironment() = default;

	/**
	*	Constructs a new environment that is initialized to the given members.
	*/
	CASSimpleEnvironment( asIScriptEngine* pScriptEngine, asALLOCFUNC_t allocFunc, asFREEFUNC_t freeFunc, IASLogger* pLogger = nullptr );

	/**
	*	Copies the given environment into this one.
	*/
	CASSimpleEnvironment( IASEnvironment& environment );

	CASSimpleEnvironment( const CASSimpleEnvironment& other ) = default;
	CASSimpleEnvironment& operator=( const CASSimpleEnvironment& other ) = default;
	CASSimpleEnvironment( CASSimpleEnvironment&& other ) = default;
	CASSimpleEnvironment& operator=( CASSimpleEnvironment&& other ) = default;

	asIScriptEngine* GetScriptEngine() override final { return m_ScriptEngine.Get(); }

	asALLOCFUNC_t GetAllocFunc() override final { return m_AllocFunc; }

	asFREEFUNC_t GetFreeFunc() override final { return m_FreeFunc; }

	IASLogger* GetLogger() override final { return m_pLogger; }

	void SetScriptEngine( asIScriptEngine* pScriptEngine )
	{
		m_ScriptEngine = pScriptEngine;
	}

	void SetAllocFunc( asALLOCFUNC_t allocFunc )
	{
		m_AllocFunc = allocFunc;
	}

	void SetFreeFunc( asFREEFUNC_t freeFunc )
	{
		m_FreeFunc = freeFunc;
	}

	void SetLogger( IASLogger* pLogger )
	{
		m_pLogger = pLogger;
	}

	/**
	*	@return Whether this environment is valid. An environment is valid if all required members are set.
	*/
	bool IsValid() const
	{
		return m_ScriptEngine.Get() && m_AllocFunc && m_FreeFunc;
	}

private:
	CASRefPtr<asIScriptEngine> m_ScriptEngine;
	asALLOCFUNC_t m_AllocFunc = nullptr;
	asFREEFUNC_t m_FreeFunc = nullptr;
	IASLogger* m_pLogger = nullptr;
};

inline CASSimpleEnvironment::CASSimpleEnvironment( asIScriptEngine* pScriptEngine, asALLOCFUNC_t allocFunc, asFREEFUNC_t freeFunc, IASLogger* pLogger )
	: m_ScriptEngine( pScriptEngine )
	, m_AllocFunc( allocFunc )
	, m_FreeFunc( freeFunc )
	, m_pLogger( pLogger )
{
}

inline CASSimpleEnvironment::CASSimpleEnvironment( IASEnvironment& environment )
	: m_ScriptEngine( environment.GetScriptEngine() )
	, m_AllocFunc( environment.GetAllocFunc() )
	, m_FreeFunc( environment.GetFreeFunc() )
	, m_pLogger( environment.GetLogger() )
{
}

#endif //ASMOD_CASSIMPLEENVIRONMENT_H
