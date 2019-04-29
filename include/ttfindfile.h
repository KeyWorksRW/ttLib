/////////////////////////////////////////////////////////////////////////////
// Name:		ttCFindFile
// Purpose:		Header-only class for locating one or more files
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1999-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_FINDFILE_H__
#define __TTLIB_FINDFILE_H__

#ifndef _WINDOWS_
	#error ttCFindFile can only be used when compiling for Windows.
#endif

/*
		Example usage:

		ttCFindFile ff("*.*");
		if (ff.isValid()) {
			do {
				if (ff.isDir())
					printf("Found the directory  %s\n", (const char*) ff);
				else
					printf("Found the file %s\n", (const char*) ff);
			} while(ff.NextFile());
		}
*/

class ttCFindFile : public WIN32_FIND_DATA
{
public:
	ttCFindFile(const char* pszFilePattern) { m_hfind = FindFirstFile(pszFilePattern, this);
#ifdef _DEBUG
		m_pszFilename = cFileName;
#endif
	}
	~ttCFindFile() {
		if (m_hfind != INVALID_HANDLE_VALUE)
			FindClose(m_hfind);
	}

	bool NextFile() { return FindNextFile(m_hfind, this) ? true : false; }
	bool NewPattern(const char* pszFilePattern) {
		if (m_hfind != INVALID_HANDLE_VALUE)
			FindClose(m_hfind);
		m_hfind = FindFirstFile(pszFilePattern, this);
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

	DWORD GetAttributes() { return dwFileAttributes; }
	int64_t GetFullFileSize() { return (((int64_t) nFileSizeHigh) << 32) + (int64_t) nFileSizeLow; }

	const char* GetFileName() { return cFileName; }

	char*	FindExtPortion() { return (char*) tt::FindExtPortion(cFileName); }	// find filename extension
	char*	FindChar(char ch) { return tt::FindChar(cFileName, ch); }
	char*	FindLastChar(char ch) { return tt::FindLastChar(cFileName, ch); }

	size_t	StrByteLen() { return tt::StrByteLen(cFileName); }	// length of string in bytes including 0 terminator
	size_t	StrLen() { return ttstrlen(cFileName); }		// number of characters (use strByteLen() for buffer size calculations)

	bool	IsSameStr(const char* psz) { return tt::IsSameStr(cFileName, psz); }
	bool	IsSameStrI(const char* psz) { return tt::IsSameStrI(cFileName, psz); }
	bool	IsSameSubStr(const char* psz) { return tt::IsSameSubStr(cFileName, psz); }
	bool	IsSameSubStrI(const char* psz) { return tt::IsSameSubStrI(cFileName, psz); }

	bool	IsEmpty() const { return (!IsValid() || !*cFileName)  ? true : false; }
	bool	IsNonEmpty() const { return (IsValid() && *cFileName) ? true : false; }

	operator char*() const { return (char*) cFileName; }
	operator DWORD() const { return dwFileAttributes; }

	// Note that the two == operators are case insensitive since filenames on Windows are case insensitive

	bool operator == (const char* psz) { return (IsEmpty()) ? false : tt::IsSameStrI(cFileName, psz); } // isSameStr will check for psz == null
	bool operator == (char* psz) { return (IsEmpty()) ? false : tt::IsSameStrI(cFileName, psz); }		 // isSameStr will check for psz == null

protected:
#ifdef _DEBUG
	char* m_pszFilename;
#endif

	HANDLE m_hfind;
};

#endif	// __TTLIB_FINDFILE_H__
