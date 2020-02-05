/////////////////////////////////////////////////////////////////////////////
// Name:      ttCFindFile
// Purpose:   Header-only class for locating one or more files
// Author:    Ralph Walden
// Copyright: Copyright (c) 1999-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <fileapi.h>
#include <stdint.h>

#include "ttlibwin.h"

#if !defined(INVALID_HANDLE_VALUE)
    #define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR) -1)
#endif

/*
        Example usage:

        ttCFindFile ff("*.*");
        if (ff.IsValid()) {
            do {
                if (ff.IsDir())
                    printf("Found the directory  %s\n", (const char*) ff);
                else
                    printf("Found the file %s\n", (const char*) ff);
            } while(ff.NextFile());
        }
*/

// Header-only class for locating one or more files
class ttCFindFile : public WIN32_FIND_DATAA
{
public:
    ttCFindFile(void)
    {
        m_hfind = INVALID_HANDLE_VALUE;
    }  // With this constructor, call NewPattern(...) to initialize
    ttCFindFile(const char* pszFilePattern)
    {
        m_hfind = FindFirstFileExA(pszFilePattern, FindExInfoBasic, this, FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
#if !defined(NDEBUG)  // Starts debug section.
        m_pszFilename = cFileName;
#endif
    }
    ~ttCFindFile()
    {
        if (m_hfind != INVALID_HANDLE_VALUE)
            FindClose(m_hfind);
    }

    bool NextFile() { return FindNextFileA(m_hfind, this) ? true : false; }
    bool NewPattern(const char* pszFilePattern)
    {
        if (m_hfind != INVALID_HANDLE_VALUE)
            FindClose(m_hfind);
        m_hfind = FindFirstFileExA(pszFilePattern, FindExInfoBasic, this, FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
        return IsValid();
    }

    bool IsArchive() const { return (dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? true : false; }
    bool IsCompressed() const { return (dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) ? true : false; }
    bool IsDir() const { return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false; }
    bool IsHidden() const { return (dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? true : false; }
    bool IsOffline() const { return (dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) ? true : false; }
    bool IsReadOnly() const { return (dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? true : false; }
    bool IsSystem() const { return (dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? true : false; }
    bool IsTemporary() const { return (dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) ? true : false; }
    bool IsValid() const { return (m_hfind != INVALID_HANDLE_VALUE) ? true : false; }

    DWORD   GetAttributes() { return dwFileAttributes; }
    int64_t GetFullFileSize() { return (((int64_t) nFileSizeHigh) << 32) + (int64_t) nFileSizeLow; }

    const char* GetFileName() { return cFileName; }

    char* FindExtPortion() { return (char*) ttFindExtPortion(cFileName); }  // find filename extension
    char* FindChar(char ch) { return ttStrChr(cFileName, ch); }
    char* FindLastChar(char ch) { return ttStrChrR(cFileName, ch); }

    // length of string in bytes including 0 terminator
    size_t StrByteLen() { return ttStrByteLen(cFileName); }

    // number of characters (use strByteLen() for buffer size calculations)
    size_t StrLen() { return ttStrLen(cFileName); }

    bool IsSameStr(const char* psz) { return ttIsSameStr(cFileName, psz); }
    bool IsSameStrI(const char* psz) { return ttIsSameStrI(cFileName, psz); }
    bool IsSameSubStr(const char* psz) { return ttIsSameSubStr(cFileName, psz); }
    bool IsSameSubStrI(const char* psz) { return ttIsSameSubStrI(cFileName, psz); }

    bool IsEmpty() const { return (!IsValid() || !*cFileName) ? true : false; }
    bool IsNonEmpty() const { return (IsValid() && *cFileName) ? true : false; }

    operator char*() const { return (char*) cFileName; }
    operator DWORD() const { return dwFileAttributes; }

    // Note that the two == operators are case insensitive since filenames on Windows are case insensitive

    bool operator==(const char* psz) { return (IsEmpty()) ? false : ttIsSameStrI(cFileName, psz); }
    bool operator==(char* psz) { return (IsEmpty()) ? false : ttIsSameStrI(cFileName, psz); }

private:
#if !defined(NDEBUG)  // Starts debug section.
    char* m_pszFilename;
#endif

    HANDLE m_hfind;
};
