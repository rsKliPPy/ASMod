#ifndef SQL_CASSQLMODULE_H
#define SQL_CASSQLMODULE_H

#include "ASMod/CASModBaseModule.h"

class CASSQLModule : public CASModBaseModule
{
public:
	typedef CASSQLModule ThisClass;
	typedef CASModBaseModule BaseClass;

public:
	CASSQLModule() = default;
	~CASSQLModule() = default;

	const char* GetName() const override final;

	bool Initialize( const CreateInterfaceFn* pFactories, const size_t uiNumFactories ) override;

	bool Shutdown() override;

	void Think() override;

private:
	CASSQLModule( const CASSQLModule& ) = delete;
	CASSQLModule& operator=( const CASSQLModule& ) = delete;
};

#endif //SQL_CASSQLMODULE_H
