#include <cstring>
#include <string>

#include <angelscript.h>

#include <Angelscript/add_on/scriptstdstring.h>

#include "utility/CString.h"

#include "ASMod/CASModBaseModule.h"
#include "ASMod/IASEnvironment.h"
#include "Module.h"

#include "StringInterop.h"

//Dummy version of CString so we can initialize it more easily, since modifying the entire CString class is a lot of work.
struct CSCCString
{
	typedef size_t size_type;

	static const size_type BUFFER_SIZE = 20;	//At least this much memory is needed before dynamic allocation is required

	//m_iCapacity stores a flag that tells us whether the string is static or not
	//Static strings need to allocate memory if modified
	static const size_type STATIC_BIT = 31;
	static const size_type STATIC_MASK = static_cast<size_type>( 1 << 31 );
	static const size_type ALLOC_MASK = STATIC_MASK - 1;

	CSCCString( const char* pszString )
	{
		//Always allocate dynamically, makes things easier.
		const auto uiLength = strlen( pszString );

		m_uiCapacity = uiLength + 1;
		//Just in case.
		m_uiCapacity &= ALLOC_MASK;

		//Quick and dirty strcpy into the destination buffer. Sven Co-op will do its thing.
		m_pszString = reinterpret_cast<char*>( g_pModule->GetEnvironment().GetArrayAllocFunc()( m_uiCapacity ) );

		UTIL_SafeStrncpy( m_pszString, pszString, m_uiCapacity );

		m_uiLength = m_uiCapacity - 1;
	}

	//Not really needed, but just in case.
	~CSCCString()
	{
		g_pModule->GetEnvironment().GetArrayFreeFunc()( m_pszString );
	}

	char m_szBuffer[ BUFFER_SIZE ];
	char* m_pszString;		//Can point to m_szBuffer, or a heap allocated buffer
	size_type m_uiLength;	//Length, excluding null terminator
	size_type m_uiCapacity;	//Total capacity and static flag. Never use this directly
};

static std::string CStringToStdString( const CSCCString* pszString )
{
	return pszString->m_pszString;
}

static CSCCString StdStringToCString( const std::string* pszString )
{
	return pszString->c_str();
}

void RegisterScriptStringInterop( asIScriptEngine& scriptEngine )
{
	//Register our string type. It uses AS_STRING_OBJNAME, and doesn't override the SC version's string factory.
	RegisterStdString( &scriptEngine, false );

	//Implicit conversion to our string type.
	scriptEngine.RegisterObjectMethod(
		"string", AS_STRING_OBJNAME " opImplConv() const",
		asFUNCTION( CStringToStdString ), asCALL_CDECL_OBJFIRST );

	//Implicit conversion to their string type.
	scriptEngine.RegisterObjectMethod(
		AS_STRING_OBJNAME, "string opImplConv() const",
		asFUNCTION( StdStringToCString ), asCALL_CDECL_OBJFIRST );
}
