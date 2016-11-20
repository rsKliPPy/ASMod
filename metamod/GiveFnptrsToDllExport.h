// vi: set ts=4 sw=4 :
// vim: set tw=75 :

/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef METAMOD_GIVEFNPTRSTODLLEXPORT_H
#define METAMOD_GIVEFNPTRSTODLLEXPORT_H

/**
*	@file
*	Defines MSVC specific linker settings to properly export GiveFnptrsToDll.
*	This header must be included by all projects that export this function.
*/

// Fixed MSVC compiling, by Nikolay "The Storm" Baklicharov.
#if defined(_WIN32) && !defined(__GNUC__) && defined (_MSC_VER)
	#pragma comment(linker, "/EXPORT:GiveFnptrsToDll=_GiveFnptrsToDll@8,@1")
	#pragma comment(linker, "/SECTION:.data,RW")
#endif

#endif //METAMOD_GIVEFNPTRSTODLLEXPORT_H
