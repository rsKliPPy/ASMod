#ifndef ASMOD_CASMODBASEMODULE_H
#define ASMOD_CASMODBASEMODULE_H

#include "IASModModule.h"

class IASMod;

/**
*	Base class for ASMod modules.
*/
class CASModBaseModule : public IASModModule
{
public:
	CASModBaseModule() = default;
	virtual ~CASModBaseModule() = default;

	bool Initialize( const CreateInterfaceFn* pFactories, const size_t uiNumFactories ) override;

	bool Shutdown() override;

	/**
	*	@return The Angelscript environment.
	*/
	IASEnvironment& GetEnvironment() { return *m_pEnvironment; }

protected:
	IASMod* m_pASMod = nullptr;
	IASEnvironment* m_pEnvironment = nullptr;

private:
	CASModBaseModule( const CASModBaseModule& ) = delete;
	CASModBaseModule& operator=( const CASModBaseModule& ) = delete;
};

#endif //ASMOD_CASMODBASEMODULE_H
