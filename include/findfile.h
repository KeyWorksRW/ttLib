/////////////////////////////////////////////////////////////////////////////
// Name:		ttFindFile
// Purpose:		Header-only class for locating one or more files
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1999-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_FINDFILE_H__
#define __TTLIB_FINDFILE_H__

#ifndef _WINDOWS_
	#error ttFindFile can only be used when compiling for Windows.
#endif

/*
		Example usage:

		ttFindFile ff("*.*");
		if (ff.isValid()) {
			do {
				if (ff.isDir())
					printf("Found the directory  %s\n", (const char*) ff);
				else
					printf("Found the file %s\n", (const char*) ff);
			} while(ff.NextFile());
		}
*/

class ttFindFile : public WIN32_FIND_DATA
{
public:
	ttFindFile(const char* pszFilePattern) { m_hfind = FindFirstFile(pszFilePattern, this);
#ifdef _DEBUG
		m_pszFilename = cFileName;
#endif
		}
	~ttFindFile() {
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

	char*	fndExtension() { return (char*) tt::fndExtension(cFileName); }	// find filename extension
	char*	findchr(char ch) { return tt::findchr(cFileName, ch); }
	char*	findlastchr(char ch) { return tt::findlastchr(cFileName, ch); }

	size_t	strbyte() { return tt::strbyte(cFileName); }	// length of string in bytes including 0 terminator
	size_t	strlen() { return tt::strlen(cFileName); }		// number of characters (use strbyte() for buffer size calculations)

	bool	samestr(const char* psz) { return tt::samestr(cFileName, psz); }
	bool	samestri(const char* psz) { return tt::samestri(cFileName, psz); }
	bool	samesubstr(const char* psz) { return tt::samesubstr(cFileName, psz); }
	bool	samesubstri(const char* psz) { return tt::samesubstri(cFileName, psz); }

	bool	isempty() const { return (!isValid() || !*cFileName)  ? true : false; }
	bool	isnonempty() const { return (isValid() && *cFileName) ? true : false; }

	operator char*() const { return (char*) cFileName; }
	operator DWORD() const { return dwFileAttributes; }

	// Note that the two == operators are case insensitive since filenames on Windows are case insensitive

	bool operator == (const char* psz) { return (isempty()) ? false : tt::samestri(cFileName, psz); } // samestr will check for psz == null
	bool operator == (char* psz) { return (isempty()) ? false : tt::samestri(cFileName, psz); }		 // samestr will check for psz == null

protected:
#ifdef _DEBUG
	char* m_pszFilename;
#endif

	HANDLE m_hfind;
};

#endif	// __TTLIB_FINDFILE_H__
