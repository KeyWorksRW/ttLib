/////////////////////////////////////////////////////////////////////////////
// Name:		misc.cpp
// Purpose:		miscellaneous functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// Licence:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttdebug.h" 	// for ttASSERTS
#include "../include/ttstr.h"	// ttCStr

size_t ttHashFromSz(const char* psz)
{
	ttASSERT_NONEMPTY(psz);

	if (!psz || !*psz)
		return 0;

	// djb2 hash algorithm

#if 1	// this is a djb2 hash algorithm
	size_t hash = 5381;

	while (*psz)
		hash = ((hash << 5) + hash) ^ (size_t) *psz++;

#else	// sdbm hash algorithm
	size_t hash = 0;

	while (*psz)
		hash = (size_t) *psz++ + (hash << 6) + (hash << 16) - hash;
#endif
	return hash;
}

size_t ttHashFromSz(const wchar_t* psz)
{
	ttASSERT_NONEMPTY(psz);

	if (!psz || !*psz)
		return 5381;

	size_t hash = 5381;

	while (*psz)
		hash = ((hash << 5) + hash) + (size_t) *psz++;

	return hash;
}

// Unlike a "regular" hash, this version treats forward and backslashes identically, as well as upper and lower-case letters

size_t ttHashFromURL(const char* pszURL)
{
	ttCStr csz(pszURL);
	ttBackslashToForwardslash(csz);
	csz.MakeLower();
	return ttHashFromSz(csz);
}

size_t ttHashFromURL(const wchar_t* pszURL)
{
	ttCStr csz(pszURL);
	ttBackslashToForwardslash(csz);
	csz.MakeLower();
	return ttHashFromSz(csz);
}
