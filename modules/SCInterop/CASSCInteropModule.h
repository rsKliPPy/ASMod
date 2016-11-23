#ifndef SCINTEROP_CASSCINTEROPMODULE_H
#define SCINTEROP_CASSCINTEROPMODULE_H

#include "ASMod/CASModBaseModule.h"

class CASSCInteropModule : public CASModBaseModule
{
public:
	typedef CASSCInteropModule ThisClass;
	typedef CASModBaseModule BaseClass;

public:
	CASSCInteropModule() = default;
	~CASSCInteropModule() = default;

	const char* GetName() const override final;

	bool Initialize( const CreateInterfaceFn* pFactories, const size_t uiNumFactories ) override;

	bool Shutdown() override;

private:
	CASSCInteropModule( const CASSCInteropModule& ) = delete;
	CASSCInteropModule& operator=( const CASSCInteropModule& ) = delete;
};

#endif //SCINTEROP_CASSCINTEROPMODULE_H
