#ifndef INTERFACEHELPERS_H
#define INTERFACEHELPERS_H

#include "interface.h"

/**
*	@file
*	Defines helper functions for interface creation.
*/

/**
*	Creates an interface from a list of interfaces. For use with interfaces deriving from IBaseInterface.
*	@param pFactories List of factories.
*	@param uiNumFactories Number of factories in the list.
*	@param pszName Name of the interface.
*	@return If the interface was created, the interface. Otherwise, returns null.
*	@tparam IFACE Type of the interface to create.
*/
template<typename IFACE>
IFACE IFACE_CreateFromList( const CreateInterfaceFn* pFactories, size_t uiNumFactories, const char* const pszName )
{
	IFACE pInstance = nullptr;

	while( uiNumFactories-- > 0 )
	{
		if( ( pInstance = static_cast<IFACE>( ( *pFactories )( pszName, nullptr ) ) ) != nullptr )
			return pInstance;

		++pFactories;
	}

	return nullptr;
}

/**
*	Creates an interface from a list of interfaces. For use with interfaces that use the old C-style interface struct approach.
*	@param pFactories List of factories.
*	@param uiNumFactories Number of factories in the list.
*	@param pszName Name of the interface.
*	@return If the interface was created, the interface. Otherwise, returns null.
*	@tparam IFACE Type of the interface to create.
*/
template<typename IFACE>
IFACE IFACE_CreateCStyleFromList( const CreateInterfaceFn* pFactories, size_t uiNumFactories, const char* const pszName )
{
	IFACE pInstance = nullptr;

	while( uiNumFactories-- > 0 )
	{
		if( ( pInstance = reinterpret_cast<IFACE>( ( *pFactories )( pszName, nullptr ) ) ) != nullptr )
			return pInstance;

		++pFactories;
	}

	return nullptr;
}

/**
*	Exposes a global variable that uses a C style interface struct.
*	Performs a static cast to check if the interface is what it should be.
*/
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR_CSTYLE( interfaceName, versionName, globalVarName )			\
static IBaseInterface* __Create##interfaceName##_interface()											\
{																										\
	return reinterpret_cast<IBaseInterface*>( static_cast<interfaceName*>( &( globalVarName ) ) );		\
}																										\
static InterfaceReg __g_Create##interfaceName##_reg(__Create##interfaceName##_interface, versionName)

#endif //INTERFACEHELPERS_H
