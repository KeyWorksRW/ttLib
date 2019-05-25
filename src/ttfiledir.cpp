/////////////////////////////////////////////////////////////////////////////
// Name:      filedir.cpp
// Purpose:   Functions for dealing with files and directories
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"        // precompiled header

#include "../include/ttdebug.h"     // for ttASSERTS
#include "../include/ttstr.h"   // ttCStr

#ifndef _WINDOWS_
    #error This code will only work on Windows
#endif

bool ttFileExists(const char* pszFile)
{
    if (!pszFile)
        return false;
    DWORD result = GetFileAttributesA(pszFile);
    return (result != INVALID_FILE_ATTRIBUTES && !(result & FILE_ATTRIBUTE_DIRECTORY));
}

bool ttFileExists(const wchar_t* pszFile)
{
    if (!pszFile)
        return false;
    DWORD result = GetFileAttributesW(pszFile);
    return (result != INVALID_FILE_ATTRIBUTES && !(result & FILE_ATTRIBUTE_DIRECTORY));
}

bool ttDirExists(const char* pszFolder)
{
    if (!pszFolder)
        return false;
    DWORD result = GetFileAttributesA(pszFolder);
    return (result != INVALID_FILE_ATTRIBUTES && result & FILE_ATTRIBUTE_DIRECTORY);
}

bool ttDirExists(const wchar_t* pszFolder)
{
    if (!pszFolder)
        return false;
    DWORD result = GetFileAttributesW(pszFolder);
    return (result != INVALID_FILE_ATTRIBUTES && result & FILE_ATTRIBUTE_DIRECTORY);
}

// REVIEW: [randalphwa - 09-01-2018] I suspect this will fail if the sub folder name contains a trailing slash. I.e.,
// "c:\foo\bar\" will not create "c:\foo", but "c:\foo\bar" would. Add this to our test suite.

bool ttCreateDir(const char* pszDir)
{
    ttASSERT(pszDir);
    if (!pszDir)
        return false;

    if (CreateDirectoryA(pszDir, nullptr))
        return true;

    ttCStr cszDir(pszDir);
    ttBackslashToForwardslash(cszDir);

    char* psz = ttStrChrR(cszDir, '/');
    if (!psz)
        return false;
    *psz = '\0';
    if (!ttCreateDir(cszDir))
        return false;
    *psz = '/';

    return CreateDirectoryA(pszDir, nullptr) ? true : false;
}

bool ttCreateDir(const wchar_t* pszDir)
{
    ttASSERT(pszDir);
    if (!pszDir)
        return false;

    if (CreateDirectoryW(pszDir, nullptr))
        return true;

    ttCStr cszDir(pszDir);
    ttBackslashToForwardslash(cszDir);

    char* psz = ttStrChrR(cszDir, '/');
    if (!psz)
        return false;
    *psz = '\0';
    if (!ttCreateDir(cszDir))
        return false;
    *psz = '/';

    return CreateDirectoryW(pszDir, nullptr) ? true : false;
}

/*
    This function serves two purposes. It converts a full path into a relative path, and it fixes a path to a file that is
    supposed to be relative to a root location.

    ConvertToRelative("c:/myProject/foo.cpp", "c:/myProject/foo.h")

        The above will set cszResult to "foo.h"

    ConvertToRelative("c:/myProject/src/foo.cpp", "c:/myProject/include/foo.h")

        The above will set cszResult to "../include/foo.h"

    ConvertToRelative("../res/foo.rc", "lang/en.rc")

        The above will set cszResult to "../res/lang/en.rc"

*/

void ttConvertToRelative(const char* pszRoot, const char* pszFile, ttCStr& cszResult)
{
    ttASSERT_NONEMPTY(pszRoot);
    ttASSERT_NONEMPTY(pszFile);
    if (!pszFile || !*pszFile)
    {
        // REVIEW: [randalphwa - 4/15/2019] it might make more sense to just throw -- the calling program will not correctly no matter what
        cszResult = "internal error";
        return;
    }

    if (!pszRoot)
    {
        cszResult = pszFile ? pszFile : "";
        return;
    }

    ttCStr cszRoot(pszRoot);
    if (ttIsValidFileChar(pszFile, 0) && ttIsValidFileChar(pszFile, 1))   // this would mean we were only passed a filename
    {
        if (ttFileExists(cszRoot))                                      // if the root included a filename, then remove it now
        {
            char* pszFilePortion = (char*) ttFindFilePortion(cszRoot);
            if (pszFilePortion)
                *pszFilePortion = 0;
        }
        if (cszRoot[0])
        {
            cszRoot.AppendFileName(pszFile);
            cszResult = cszRoot;
        }
        else
            cszResult = pszFile;
        return;
    }

    cszRoot.GetFullPathName();
    ttCStr cszFile(pszFile);
    cszFile.GetFullPathName();

    if (toupper(cszRoot[0]) != toupper(cszFile[0]))   // probably on a different drive, but clearly there's nothing relative about it
    {
        cszResult = cszFile;
        return;
    }

    ttBackslashToForwardslash(cszRoot); // slashes need to be the same for valid comparisons
    ttBackslashToForwardslash(cszFile);

    // We might have been passed a filename as the root, remove the filename portion if that's the case

    if (ttFileExists(cszRoot))
    {
        char* pszFilePortion = (char*) ttFindFilePortion(cszRoot);
        if (pszFilePortion)
            *pszFilePortion = 0;
    }
    cszRoot.AddTrailingSlash(); // it is imperative that we end with a slash

    size_t pos;
    char* pszLastSlash = nullptr;

    for (pos = 0; cszRoot[pos] && tolower(cszRoot[pos]) == tolower(cszFile[pos]); ++pos) {
        if (cszRoot[pos] == '/')
            pszLastSlash = cszRoot.GetPtr() + pos;
    }

    if (!pszLastSlash[1]) // did the entire path match?
    {
        cszResult = cszFile.GetPtr() + pos;
        return;
    }
#if 0
    if (!cszFile[pos])  // this should be impossible
    {
        cszResult = pszFile;
        return;
    }

    char* pszFilePortion = ttFindFilePortion(cszFile);
    if (pszFilePortion == cszFile.GetPtr() + pos)     // if file is in the same dir as the root, then we're done
    {
        cszResult = pszFilePortion;
        return;
    }
#endif

    // The following scenarios need to be dealt with

    //  c:/foo/bar/src/ c:/foo/bar/inc/file.h >> ../inc/file.h
    //             ^ pos


    //  c:/foo/bar/src/ c:/foo/inc/file.h     >> ../../inc/file.h
    //         ^ pos

    cszResult.Delete();
    ++pszLastSlash;
    size_t posDiff = pszLastSlash - cszRoot.GetPtr();
    ttASSERT(ttStrChr(pszLastSlash, '/'));  // we should never be pointing to the last slash

    do {
        while (*pszLastSlash != '/')
            ++pszLastSlash;
        cszResult += "../";
        ++pszLastSlash;
    } while(*pszLastSlash);

    cszResult += (cszFile.GetPtr() + posDiff);
}

void ttBackslashToForwardslash(char* psz)
{
    ttASSERT_MSG(psz, "NULL pointer!");
    if (!psz)
        return;

    char* pszSlash = ttStrChr(psz, '\\');
    while (pszSlash)
    {
        *pszSlash = '/';
        pszSlash = ttStrChr(ttNextChar(pszSlash), '\\');
    }
}

void ttForwardslashToBackslash(char* psz)
{
    ttASSERT_MSG(psz, "NULL pointer!");
    if (!psz)
        return;

    char* pszSlash = ttStrChr(psz, '/');
    while (pszSlash)
    {
        *pszSlash = '\\';
        pszSlash = ttStrChr(ttNextChar(pszSlash), '/');
    }
}

char* ttFindFilePortion(const char* pszPath)
{
    ttASSERT_MSG(pszPath, "NULL pointer!");
    if (!pszPath)
        return nullptr;

    char* psz;
#ifdef _WINDOWS_
    psz = ttStrChrR(pszPath, '\\'); // Paths usually have back slashes under Windows
    if (psz)
        pszPath = psz + 1;
#endif  // _WINDOWS_
    psz = ttStrChrR(pszPath, '/');  // forward slashes are valid on all OS, so check that too
    if (psz)
        return psz + 1;

    // path contains no forward or back slash, so look for a colon
    psz = ttStrChrR(pszPath, ':');
    return (psz ? psz + 1 : (char*) pszPath);
}

char* ttFindExtPortion(const char* pszPath)
{
    char* psz = ttStrChrR(pszPath, '.');
    if (psz && !(psz == pszPath || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/'))   // ignore .file, ./file, and ../file
        return psz;
    else
        return nullptr;
}

bool ttIsValidFileChar(const char* psz, size_t pos)
{
    if (psz) {
        switch (psz[pos])
        {
            case '.':
                if (pos == 0 && (psz[1] == 0 || psz[1] == '.'))
                    return false;   // "." and ".." are folders, not a filename
                else if (pos == 1 && psz[0] == '.')
                    return false;   // ".." is a folder, not a filename
                return true;        // valid if not above two exceptions

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
