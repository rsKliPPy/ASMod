#include <extdll.h>
#include <meta_api.h>

#include "Module.h"

CASModBaseModule* g_pModule = nullptr;

meta_globals_t *gpMetaGlobals;		// metamod globals
gamedll_funcs_t *gpGamedllFuncs;	// gameDLL function tables
mutil_funcs_t *gpMetaUtilFuncs;		// metamod utility functions

//! Holds engine functionality callbacks
enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;

//TODO: temporary until logging issues are resolved.
plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,	// ifvers
	"ASModModule",	// name
	"1.0 Alpha",	// version
	__DATE__,			// date
	"Sam \"Solokiller\" Vanheer",		// author
	"https://github.com/SamVanheer",			// url
	"ASMOD-Module",		// logtag
	PT_STARTUP,	// (when) loadable
	PT_STARTUP,	// (when) unloadable
};

//Config file that contains the MySQL settings to use for default connections.
cvar_t	as_mysql_config = { "as_mysql_config", "server/default_mysql_config.txt", FCVAR_SERVER | FCVAR_UNLOGGED };

#ifdef WIN32
//See post VS 2015 update 3 delayimp.h for the reason why this has to be defined. - Solokiller
#define DELAYIMP_INSECURE_WRITABLE_HOOKS
#include <delayimp.h>

FARPROC WINAPI DelayHook(
	unsigned        dliNotify,
	PDelayLoadInfo  pdli
)
{
	if( dliNotify == dliNotePreLoadLibrary )
	{
		if( strcmp( pdli->szDll, "sqlite3.dll" ) == 0 ||
			strcmp( pdli->szDll, "libmariadb.dll" ) == 0 )
		{
			char szPath[ MAX_PATH ];

			const char* pszGameDir = gpMetaUtilFuncs->pfnGetGameInfo( PLID, GINFO_GAMEDIR );

			if( !pszGameDir )
				return nullptr;

			const int iResult = snprintf( szPath, sizeof( szPath ), "%s/%s", pszGameDir, pdli->szDll );

			if( iResult < 0 || static_cast<size_t>( iResult ) >= sizeof( szPath ) )
				return nullptr;

			HMODULE hLib = LoadLibraryA( szPath );

			return ( FARPROC ) hLib;
		}
	}

	return nullptr;
}

ExternC PfnDliHook __pfnDliNotifyHook2 = DelayHook;

ExternC PfnDliHook   __pfnDliFailureHook2 = nullptr;
#endif
