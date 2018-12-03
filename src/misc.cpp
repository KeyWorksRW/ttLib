/////////////////////////////////////////////////////////////////////////////
// Name:		misc.cpp
// Purpose:		Miscellaneous functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"		// precompiled header

#include "../include/cstr.h"	// CStr

#if !defined(_WINDOWS_) && !defined(_WX_WX_H_)
	#error wxWidgets is required for non-Windows builds
#endif

#ifdef _WX_WX_H_
	#include <wx/dir.h>	// wxDir class
#endif

// typedef size_t HASH;		// defined in ttlib.h

size_t HashFromSz(const char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	ASSERT_MSG(*psz, "empty string!");

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

size_t HashFromSz(const wchar_t* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	ASSERT_MSG(*psz, "empty string!");

	if (!psz || !*psz)
		return 5381;

	size_t hash = 5381;

	while (*psz)
		hash = ((hash << 5) + hash) + (size_t) *psz++;

	return hash;
}

// Unlike a "regular" hash, this version treats forward and backslashes identically, as well as upper and lower-case letters

size_t HashFromURL(const char* pszURL)
{
	CStr csz(pszURL);
	BackslashToForwardslash(csz);
	csz.MakeLower();
	return HashFromSz(csz);
}

size_t HashFromURL(const wchar_t* pszURL)
{
	CStr csz(pszURL);
	BackslashToForwardslash(csz);
	csz.MakeLower();
	return HashFromSz(csz);
}

void trim(char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz || !*psz)
		return;

	char* pszEnd = psz + strlen(psz) - 1;
	while ((*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\r' || *pszEnd == '\n' || *pszEnd == '\f')) {
		pszEnd--;
		if (pszEnd == psz) {
			*pszEnd = 0;
			return;
		}
	}
	pszEnd[1] = '\0';
}

int GetCPUCount()
{
#ifdef	_WX_WX_H_
	int cpus = wxThread::GetCPUCount();
	if (cpus == -1)
		cpus = 1;
#else	// not _WX_WX_H_
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpus = (int) si.dwNumberOfProcessors;
#endif	// _WX_WX_H_
	return cpus;
}
