/////////////////////////////////////////////////////////////////////////////
// Name:		misc.cpp
// Purpose:
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// Licence:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttstring.h"	// ttString

size_t tt::HashFromSz(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	ttASSERT_MSG(*psz, "empty string!");

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

size_t tt::HashFromSz(const wchar_t* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	ttASSERT_MSG(*psz, "empty string!");

	if (!psz || !*psz)
		return 5381;

	size_t hash = 5381;

	while (*psz)
		hash = ((hash << 5) + hash) + (size_t) *psz++;

	return hash;
}

// Unlike a "regular" hash, this version treats forward and backslashes identically, as well as upper and lower-case letters

size_t tt::HashFromURL(const char* pszURL)
{
	ttString csz(pszURL);
	tt::BackslashToForwardslash(csz);
	csz.MakeLower();
	return tt::HashFromSz(csz);
}

size_t tt::HashFromURL(const wchar_t* pszURL)
{
	ttString csz(pszURL);
	tt::BackslashToForwardslash(csz);
	csz.MakeLower();
	return HashFromSz(csz);
}
