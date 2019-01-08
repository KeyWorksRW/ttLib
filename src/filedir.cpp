/////////////////////////////////////////////////////////////////////////////
// Name:		filedir.cpp
// Purpose:		Functions for dealing with files and directories
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"		// precompiled header

#include "../include/cstr.h"	// CStr
#include "../include/kstr.h"

#if !defined(_WINDOWS_) && !defined(_WX_WX_H_)
	#error wxWidgets is required for non-Windows builds
#endif

#ifdef _WX_WX_H_
	#include <wx/dir.h>	// wxDir class
#endif

bool FileExists(const char* pszFile)
{
	if (!pszFile)
		return false;
#ifdef _WINDOWS_
	DWORD result = GetFileAttributesA(pszFile);
	return (result != INVALID_FILE_ATTRIBUTES && !(result & FILE_ATTRIBUTE_DIRECTORY));
#else
	wxFileName fn;
	return fn.FileExists(pszFile);
#endif
}

bool FileExists(const wchar_t* pszFile)
{
	if (!pszFile)
		return false;
#ifdef _WINDOWS_
	DWORD result = GetFileAttributesW(pszFile);
	return (result != INVALID_FILE_ATTRIBUTES && !(result & FILE_ATTRIBUTE_DIRECTORY));
#else
	wxFileName fn;
	return fn.FileExists(pszFile);
#endif
}

bool DirExists(const char* pszFolder)
{
	if (!pszFolder)
		return false;
#ifdef _WINDOWS_
	DWORD result = GetFileAttributesA(pszFolder);
	return (result != INVALID_FILE_ATTRIBUTES && result & FILE_ATTRIBUTE_DIRECTORY);
#else
	wxFileName fn;
	return fn.DirExists(pszFile);
#endif
}

bool DirExists(const wchar_t* pszFolder)
{
	if (!pszFolder)
		return false;
#ifdef _WINDOWS_
	DWORD result = GetFileAttributesW(pszFolder);
	return (result != INVALID_FILE_ATTRIBUTES && result & FILE_ATTRIBUTE_DIRECTORY);
#else
	wxFileName fn;
	return fn.DirExists(pszFile);
#endif
}

// REVIEW: [randalphwa - 09-01-2018] I suspect this will fail if the sub folder name contains a trailing
// slash. I.e., "c:\foo\bar\" will not create "c:\foo", but "c:\foo\bar" would. Add this to our test suite.

bool CreateDir(const char* pszDir)
{
	ASSERT(pszDir);
	if (!pszDir)
		return false;

#ifdef	_WX_WX_H_
	wxDir dir;
	if (dir.Make(pszDir))
		return true;
#else
	if (CreateDirectoryA(pszDir, nullptr))
		return true;
#endif

	CStr cszDir(pszDir);
	BackslashToForwardslash(cszDir);

	char* psz = kstrchrR(cszDir, '/');
	if (!psz)
		return false;
	*psz = '\0';
	if (!CreateDir(cszDir))
		return false;
	*psz = '/';

#ifdef	_WX_WX_H_
	return dir.Make(pszDir);
#else
	return CreateDirectoryA(pszDir, nullptr) ? true : false;
#endif
}

bool CreateDir(const wchar_t* pszDir)
{
	ASSERT(pszDir);
	if (!pszDir)
		return false;

#ifdef	_WX_WX_H_
	wxDir dir;
	if (dir.Make(pszDir))
		return true;
#else
	if (CreateDirectoryW(pszDir, nullptr))
		return true;
#endif

	CStr cszDir(pszDir);
	BackslashToForwardslash(cszDir);

	char* psz = kstrchrR(cszDir, '/');
	if (!psz)
		return false;
	*psz = '\0';
	if (!CreateDir(cszDir))
		return false;
	*psz = '/';

#ifdef	_WX_WX_H_
	return dir.Make(pszDir);
#else
	return CreateDirectoryW(pszDir, nullptr) ? true : false;
#endif
}

// A copy of pszFile is made, so okay if pszFile points to cszPath

void ConvertToRelative(const char* pszRoot, const char* pszFile, CStr& cszResult)
{
	ASSERT_MSG(pszRoot && pszFile, "NULL pointer!");
	ASSERT_MSG(*pszRoot && *pszFile, "empty string!");

	if (!pszRoot) {
		cszResult = pszFile ? pszFile : "";
		return;
	}

	CStr cszRoot(pszRoot);
	if (pszFile == FindFilePortion(pszFile)) {	// this would mean we were only passed a filename
		if (FileExists(cszRoot)) {	// if the root included a filename, then remove it now
			char* pszFilePortion = FindFilePortion(cszRoot);
			if (pszFilePortion)
				*pszFilePortion = 0;
		}
		if (cszRoot[0])	{
			cszRoot.AppendFileName(pszFile);
			cszResult = cszRoot;
		}
		else
			cszResult = pszFile;
		return;
	}

	cszRoot.GetFullPathName();
	CStr cszFile(pszFile);
	cszFile.GetFullPathName();

	if (cszRoot[0] != cszFile[0]) {	// probably on a different drive, but clearly there's nothing relative about it
		cszResult = cszFile;
		return;
	}

	BackslashToForwardslash(cszRoot);	// slashes need to be the same for valid comparisons
	BackslashToForwardslash(cszFile);

	// We might have been passed a filename as the root, remove the filename portion if that's the case

	if (FileExists(cszRoot)) {
		char* pszFilePortion = FindFilePortion(cszRoot);
		if (pszFilePortion)
			*pszFilePortion = 0;
	}
	cszRoot.AddTrailingSlash();	// it is imperative that we end with a slash

	size_t pos;
	char* pszLastSlash = nullptr;

	for (pos = 0; cszRoot[pos] && cszRoot[pos] == cszFile[pos]; ++pos) {
		if (cszRoot[pos] == '/')
			pszLastSlash = cszRoot.getptr() + pos;
	}

	if (!pszLastSlash[1]) { // did the entire path match?
		cszResult = cszFile.getptr() + pos;
		return;
	}
#if 0
	if (!cszFile[pos]) {	// this should be impossible
		cszResult = pszFile;
		return;
	}

	char* pszFilePortion = FindFilePortion(cszFile);
	if (pszFilePortion == cszFile.getptr() + pos) {		// if file is in the same dir as the root, then we're done
		cszResult = pszFilePortion;
		return;
	}
#endif

	// The following scenarios need to be dealt with

	//	c:/foo/bar/src/ c:/foo/bar/inc/file.h >> ../inc/file.h
	//			   ^ pos


	//	c:/foo/bar/src/ c:/foo/inc/file.h	  >> ../../inc/file.h
	//		   ^ pos

	cszResult.Delete();
	++pszLastSlash;
	size_t posDiff = pszLastSlash - cszRoot.getptr();
	ASSERT(tt::strchr(pszLastSlash, '/'));	// we should never be pointing to the last slash

	do {
		while (*pszLastSlash != '/')
			++pszLastSlash;
		cszResult += "../";
		++pszLastSlash;
	} while(*pszLastSlash);

	cszResult += (cszFile.getptr() + posDiff);
}

void BackslashToForwardslash(char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;

	char* pszSlash = kstrchr(psz, '\\');
	while (pszSlash) {
		*pszSlash = '/';
		pszSlash = tt::strchr(tt::nextchr(pszSlash), '\\');
	}
}

void ForwardslashToBackslash(char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;

	char* pszSlash = kstrchr(psz, '/');
	while (pszSlash) {
		*pszSlash = '\\';
		pszSlash = kstrchr(knextchr(pszSlash), '/');
	}
}

void AddTrailingSlash(char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;

#ifdef _WINDOWS_
	size_t cb = kstrlen(psz);
	char ch = *(CharPrevExA(CP_UTF8, psz, psz + cb, 0));
	if (ch != '/' && ch != '\\' && ch != ':') {
		kstrcat(psz, "/");
	}

#else	// not _WINDOWS_

	char* pszLast = kstrchrR(psz, '/');
	if (pszLast && !pszLast[1])
		return;	// already has a trailing slash
	char* pszColon = kstrchrR(psz, ':');
	if (pszColon && !pszColon[1])
		return;	// e.g., if the string is something like "c:" then do NOT add a trailing slash
	kstrcat(psz, "/");

#endif	// _WINDOWS_
}

char* FindFilePortion(const char* pszFile)
{
	ASSERT_MSG(pszFile, "NULL pointer!");
	if (!pszFile)
		return nullptr;

	char* psz;
#ifdef _WINDOWS_
	psz = tt::strchrR(pszFile, '\\');	// Paths usually have back slashes under Windows
	if (psz)
		pszFile = psz + 1;
#endif	// _WINDOWS_
	psz = tt::strchrR(pszFile, '/');	// forward slashes are valid on all OS, so check that too
	if (psz)
		return psz + 1;
	psz = tt::strchrR(pszFile, ':');
	return (psz ? psz + 1 : (char*) pszFile);
}

char* FindExtPortion(const char* pszFile)
{
	char* psz = tt::strchrR(pszFile, '.');
	if (psz && !(psz == pszFile || *(psz - 1) == '.' || psz[1] == CH_BACKSLASH || psz[1] == CH_FORWARDSLASH))	// ignore .file, ./file, and ../file
		return psz;
	else
		return nullptr;
}

bool IsValidFileChar(const char* psz, size_t pos)
{
	if (psz) {
		switch (psz[pos]) {
			case '.':
				if (pos == 0 && (psz[1] == 0 || psz[1] == '.'))
					return false;	// "." and ".." are folders, not a filename
				else if (pos == 1 && psz[0] == '.')
					return false;	// ".." is a folder, not a filename
				return true;		// valid if not above two exceptions

			case '<':
			case '>':
			case ':':
			case '/':
			case '\\':
			case '|':
			case '?':
			case '*':
			case 0:
				return false;
		}
		return true;
	}
	return false;
}
