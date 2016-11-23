#include <extdll.h>
#include <meta_api.h>

#include "interface.h"

#include "InterfaceHelpers.h"

#include "ASMod/CreateInterface_api.h"

EXPOSE_SINGLE_INTERFACE_GLOBALVAR_CSTYLE( enginefuncs_t, ENGINEFUNCS_T_NAME, g_engfuncs );

EXPOSE_SINGLE_INTERFACE_GLOBALVAR_CSTYLE( globalvars_t, GLOBALVARS_T_NAME, *gpGlobals );

EXPOSE_SINGLE_INTERFACE_GLOBALVAR_CSTYLE( meta_globals_t, META_GLOBALS_T_NAME, *gpMetaGlobals );

EXPOSE_SINGLE_INTERFACE_GLOBALVAR_CSTYLE( gamedll_funcs_t, GAMEDLL_FUNCS_T_NAME, *gpGamedllFuncs );

EXPOSE_SINGLE_INTERFACE_GLOBALVAR_CSTYLE( mutil_funcs_t, MUTIL_FUNCS_T_NAME, *gpMetaUtilFuncs );
