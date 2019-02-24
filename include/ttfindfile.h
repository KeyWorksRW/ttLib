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
		return isValid();
	}

	bool isArchive() const { return (dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? true : false; }
	bool isCompressed() const { return (dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) ? true : false; }
	bool isDir() const { return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false; }
	bool isHidden() const { return (dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? true : false; }
	bool isOffline() const { return (dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) ? true : false; }
	bool isReadOnly() const { return (dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? true : false; }
	bool isSystem() const { return (dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? true : false; }
	bool isTemporary() const { return (dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) ? true : false; }
	bool isValid() const { return (m_hfind != INVALID_HANDLE_VALUE) ? true : false; }

	DWORD GetAttributes() { return dwFileAttributes; }
	int64_t GetFullFileSize() { return (((int64_t) nFileSizeHigh) << 32) + (int64_t) nFileSizeLow; }

	const char* GetFileName() { return cFileName; }

	char*	findExtPortion() { return (char*) tt::findExtPortion(cFileName); }	// find filename extension
	char*	findChar(char ch) { return tt::findChar(cFileName, ch); }
	char*	findLastChar(char ch) { return tt::findLastChar(cFileName, ch); }

	size_t	strByteLen() { return tt::strByteLen(cFileName); }	// length of string in bytes including 0 terminator
	size_t	strLen() { return tt::strLen(cFileName); }		// number of characters (use strByteLen() for buffer size calculations)

	bool	isSameStr(const char* psz) { return tt::isSameStr(cFileName, psz); }
	bool	isSameStri(const char* psz) { return tt::isSameStri(cFileName, psz); }
	bool	isSameSubStr(const char* psz) { return tt::isSameSubStr(cFileName, psz); }
	bool	isSameSubStri(const char* psz) { return tt::isSameSubStri(cFileName, psz); }

	bool	isEmpty() const { return (!isValid() || !*cFileName)  ? true : false; }
	bool	isNonEmpty() const { return (isValid() && *cFileName) ? true : false; }

	operator char*() const { return (char*) cFileName; }
	operator DWORD() const { return dwFileAttributes; }

	// Note that the two == operators are case insensitive since filenames on Windows are case insensitive

	bool operator == (const char* psz) { return (isEmpty()) ? false : tt::isSameStri(cFileName, psz); } // isSameStr will check for psz == null
	bool operator == (char* psz) { return (isEmpty()) ? false : tt::isSameStri(cFileName, psz); }		 // isSameStr will check for psz == null

protected:
#ifdef _DEBUG
	char* m_pszFilename;
#endif

	HANDLE m_hfind;
};

#endif	// __TTLIB_FINDFILE_H__
