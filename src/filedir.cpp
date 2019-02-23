/////////////////////////////////////////////////////////////////////////////
// Name:		filedir.cpp
// Purpose:		Functions for dealing with files and directories
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"		// precompiled header

#include "../include/ttdebug.h" 	// for ttASSERTS
#include "../include/ttstr.h"	// ttCStr

#if !defined(_WINDOWS_) && !defined(_WX_WX_H_)
	#error wxWidgets is required for non-Windows builds
#endif

#ifdef _WX_WX_H_
	#include <wx/dir.h>	// wxDir class
#endif

bool tt::FileExists(const char* pszFile)
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

bool tt::FileExists(const wchar_t* pszFile)
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

bool tt::DirExists(const char* pszFolder)
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

bool tt::DirExists(const wchar_t* pszFolder)
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

// REVIEW: [randalphwa - 09-01-2018] I suspect this will fail if the sub folder name contains a trailing slash. I.e.,
// "c:\foo\bar\" will not create "c:\foo", but "c:\foo\bar" would. Add this to our test suite.

bool tt::CreateDir(const char* pszDir)
{
	ttASSERT(pszDir);
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

	ttCStr cszDir(pszDir);
	tt::BackslashToForwardslash(cszDir);

	char* psz = tt::findLastChar(cszDir, '/');
	if (!psz)
		return false;
	*psz = '\0';
	if (!tt::CreateDir(cszDir))
		return false;
	*psz = '/';

#ifdef	_WX_WX_H_
	return dir.Make(pszDir);
#else
	return CreateDirectoryA(pszDir, nullptr) ? true : false;
#endif
}

bool tt::CreateDir(const wchar_t* pszDir)
{
	ttASSERT(pszDir);
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

	ttCStr cszDir(pszDir);
	tt::BackslashToForwardslash(cszDir);

	char* psz = tt::findLastChar(cszDir, '/');
	if (!psz)
		return false;
	*psz = '\0';
	if (!tt::CreateDir(cszDir))
		return false;
	*psz = '/';

#ifdef	_WX_WX_H_
	return dir.Make(pszDir);
#else
	return CreateDirectoryW(pszDir, nullptr) ? true : false;
#endif
}

// A copy of pszFile is made, so okay if pszFile points to cszPath

void tt::ConvertToRelative(const char* pszRoot, const char* pszFile, ttCStr& cszResult)
{
	ttASSERT_NONEMPTY(pszRoot);
	ttASSERT_NONEMPTY(pszFile);

	if (!pszRoot) {
		cszResult = pszFile ? pszFile : "";
		return;
	}

	ttCStr cszRoot(pszRoot);
	if (pszFile == tt::findFilePortion(pszFile)) {	// this would mean we were only passed a filename
		if (tt::FileExists(cszRoot)) {	// if the root included a filename, then remove it now
			char* pszFilePortion = (char*) tt::findFilePortion(cszRoot);
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

	cszRoot.getFullPathName();
	ttCStr cszFile(pszFile);
	cszFile.getFullPathName();

	if (cszRoot[0] != cszFile[0]) {	// probably on a different drive, but clearly there's nothing relative about it
		cszResult = cszFile;
		return;
	}

	tt::BackslashToForwardslash(cszRoot);	// slashes need to be the same for valid comparisons
	tt::BackslashToForwardslash(cszFile);

	// We might have been passed a filename as the root, remove the filename portion if that's the case

	if (tt::FileExists(cszRoot)) {
		char* pszFilePortion = (char*) tt::findFilePortion(cszRoot);
		if (pszFilePortion)
			*pszFilePortion = 0;
	}
	cszRoot.AddTrailingSlash();	// it is imperative that we end with a slash

	size_t pos;
	char* pszLastSlash = nullptr;

	for (pos = 0; cszRoot[pos] && cszRoot[pos] == cszFile[pos]; ++pos) {
		if (cszRoot[pos] == '/')
			pszLastSlash = cszRoot.getPtr() + pos;
	}

	if (!pszLastSlash[1]) { // did the entire path match?
		cszResult = cszFile.getPtr() + pos;
		return;
	}
#if 0
	if (!cszFile[pos]) {	// this should be impossible
		cszResult = pszFile;
		return;
	}

	char* pszFilePortion = tt::FindFilePortion(cszFile);
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
	size_t posDiff = pszLastSlash - cszRoot.getPtr();
	ttASSERT(tt::findChar(pszLastSlash, '/'));	// we should never be pointing to the last slash

	do {
		while (*pszLastSlash != '/')
			++pszLastSlash;
		cszResult += "../";
		++pszLastSlash;
	} while(*pszLastSlash);

	cszResult += (cszFile.getPtr() + posDiff);
}

void tt::BackslashToForwardslash(char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;

	char* pszSlash = tt::findChar(psz, '\\');
	while (pszSlash) {
		*pszSlash = '/';
		pszSlash = tt::findChar(tt::nextChar(pszSlash), '\\');
	}
}

void tt::ForwardslashToBackslash(char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;

	char* pszSlash = tt::findChar(psz, '/');
	while (pszSlash) {
		*pszSlash = '\\';
		pszSlash = tt::findChar(tt::nextChar(pszSlash), '/');
	}
}

char* tt::findFilePortion(const char* pszPath)
{
	ttASSERT_MSG(pszPath, "NULL pointer!");
	if (!pszPath)
		return nullptr;

	char* psz;
#ifdef _WINDOWS_
	psz = tt::findLastChar(pszPath, '\\');	// Paths usually have back slashes under Windows
	if (psz)
		pszPath = psz + 1;
#endif	// _WINDOWS_
	psz = tt::findLastChar(pszPath, '/');	// forward slashes are valid on all OS, so check that too
	if (psz)
		return psz + 1;

	// path contains no forward or back slash, so look for a colon
	psz = tt::findLastChar(pszPath, ':');
	return (psz ? psz + 1 : (char*) pszPath);
}

char* tt::findExtPortion(const char* pszPath)
{
	char* psz = tt::findLastChar(pszPath, '.');
	if (psz && !(psz == pszPath || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/'))	// ignore .file, ./file, and ../file
		return psz;
	else
		return nullptr;
}

bool tt::isValidFileChar(const char* psz, size_t pos)
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
