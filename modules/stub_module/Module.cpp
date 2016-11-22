#include <extdll.h>
#include <meta_api.h>

#include "Module.h"

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
