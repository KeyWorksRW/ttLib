/////////////////////////////////////////////////////////////////////////////
// Name:		ttFindFile
// Purpose:		Header-only class for locating a file
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
	ttFindFile(const char* pszFilePattern) { m_hfind = FindFirstFile(pszFilePattern, this); }
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

	bool isDir() { return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false; }
	bool isReadOnly() { return (dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? true : false; }
	bool isValid() { return (m_hfind != INVALID_HANDLE_VALUE) ? true : false; }

	DWORD GetAttributes() { return dwFileAttributes; }
	int64_t GetFullFileSize() { return (((int64_t) nFileSizeHigh) << 32) + (int64_t) nFileSizeLow; }

	const char* GetFileName() { return cFileName; }

	operator const char*() const { return cFileName; }
	operator DWORD() const { return dwFileAttributes; }

protected:
	HANDLE m_hfind;
};

#endif	// __TTLIB_FINDFILE_H__
