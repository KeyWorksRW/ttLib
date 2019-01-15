/////////////////////////////////////////////////////////////////////////////
// Name:		ttstr.h
// Purpose:		various functions dealing with strings
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// These functions offer a bit more functionality then either the C++ standard or C++ secure variants. In particular,
// they handle UTF8 strings and they will ASSERT when passed a null pointer when a valid pointer is required. If an
// argument is invalid, the function will attempt to continue without throwing an exception (on the assumption that you
// caught the bug in a _DEBUG build where the ASSERT macro would have fired). For example, calling kstrcat(pszMain,
// cchDest, nullptr) is perfectly valid (but will ASSERT), but strcat_s(pszMain, cchDest, nullptr) will throw an
// exception.

// Both secure and non-secure strcat and strcpy functions will limit total destination string length to 16 megabytes,
// truncating the destination (still zero-terminated) if the string exceeds that size.

// Note that when compiled with _DEBUG under Windows, ASSERTS will display a MessageBox that will display what the
// problem is (null pointer, empty string, oversized string, etc.) and allow you to break directly into a debugger to
// look at the problem, ignore it if you want to continue running and fix the problem later, or simply exit the program.

// When compiled using wxWidgets, kstristr() and IsSameSubString() will do case-insensitive comparisons on UTF8 strings.
// Without wxWidgets, the ASCII portion of a UTF8 string will be case-insensitive, but any non-ASCII characters must have
// an exact case match to be considered equal.

#pragma once

#ifndef __TTLIB_STR_H__
#define __TTLIB_STR_H__

#include "ttdebug.h"	// ASSERTs
#include "ttheap.h" 	// ttHeap

/*
  ttStr can be used as a lightweight, header-only container for a zero-terminated string. It uses the above string functions
  for handling null pointers, overflows, etc. Use the ttString class for a full blown string-handling class (see ttstring.h)

  if (some condition) {
	ttStr szBuf(256);
	szBuf.strcpy("some text");	// equivalent to strcpy_s(szBuf, sizeof(szBuf), "some text");
	szBuf.strcat(" and some more text");	// this will truncate if string to add is too long
	cout << (char*) szBuf;
  } // szBuf is freed because it went out of scope
*/

// CAUTION! strcat() and += are provided but they do NOT allocate more memory -- if the current buffer for ttStr is too
// small, the additional string will be truncated. Use the ttString class if you need dynamic memory allocation when adding
// to an existing string.

class ttStr
{
public:
	ttStr(void) { m_psz = nullptr; }
	ttStr(size_t size) { m_psz = (char*) tt::malloc(size); }
	ttStr(const char* psz) { m_psz = (char*) tt::strdup(psz); }
	~ttStr(void) { if (m_psz) tt::free(m_psz); }

	void	resize(size_t cb) { m_psz = m_psz ? (char*) tt::realloc(m_psz, cb) : (char*) tt::malloc(cb); }
	size_t	sizeBuffer() { return tt::size(m_psz); }	// returns 0 if m_psz is null
	void	Delete() { if (m_psz) { tt::free(m_psz); m_psz = nullptr; } }

	char*	findext(const char* pszExt) { return (char*) tt::findext(m_psz, pszExt); }	// find filename extension
	char*	findstr(const char* psz) { return tt::findstr(m_psz, psz); }
	char*	findstri(const char* psz) { return tt::findstri(m_psz, psz); }
	char*	findchr(char ch) { return tt::findchr(m_psz, ch); }
	char*	findlastchr(char ch) { return tt::findlastchr(m_psz, ch); }

	size_t	strbyte() { return tt::strbyte(m_psz); }	// length of string in bytes including 0 terminator
	int		strcat(const char* psz) { return tt::strcat_s(m_psz, tt::size(m_psz), psz); }	// Does NOT reallocate string!
	int		strcpy(const char* psz) { return tt::strcpy_s(m_psz, tt::size(m_psz), psz); }
	size_t	strlen() { return tt::strlen(m_psz); }		// number of characters (use strbyte() for buffer size calculations)

	bool	samestr(const char* psz) { return tt::samestr(m_psz, psz); }
	bool	samestri(const char* psz) { return tt::samestri(m_psz, psz); }
	bool	samesubstr(const char* psz) { return tt::samesubstr(m_psz, psz); }
	bool	samesubstri(const char* psz) { return tt::samesubstri(m_psz, psz); }

	char*	nextnonspace() { return (char*) tt::nextnonspace(m_psz); }
	char*	nextspace() { return (char*) tt::nextspace(m_psz); }

	ptrdiff_t atoi() { return tt::atoi(m_psz); }

	char*	itoa(int32_t val)  { char szNum[32]; tt::itoa(val, szNum, sizeof(szNum)); return tt::strdup(szNum, &m_psz); }
	char*	itoa(int64_t val)  { char szNum[32]; tt::itoa(val, szNum, sizeof(szNum)); return tt::strdup(szNum, &m_psz); }
	char*	utoa(uint32_t val) { char szNum[32]; tt::utoa(val, szNum, sizeof(szNum)); return tt::strdup(szNum, &m_psz); }
	char*	utoa(uint64_t val) { char szNum[32]; tt::utoa(val, szNum, sizeof(szNum)); return tt::strdup(szNum, &m_psz); }

	void	trim_right() { tt::trim_right(m_psz); }

	bool	isempty() { return (!m_psz || !*m_psz)  ? true : false; }
	bool	isnonempty() const { return (m_psz && *m_psz) ? true : false; }
	bool	isnull() const { return (m_psz == nullptr); }

#ifdef _WINDOWS_
	char* GetResString(size_t idString) {
		resize(1024);
		int cb = LoadStringA(tt::hinstResources, (UINT) idString, m_psz, 1024);
		m_psz[cb] = 0;	// in case LoadStringA() failed
		resize(cb + sizeof(char));
		return m_psz;
	}

	char* getCWD() {
		resize(MAX_PATH);
		DWORD cb = GetCurrentDirectoryA(MAX_PATH, m_psz);	// we don't use _getcwd() because it would require loading <direct.h> for everyone using ttLib
		m_psz[cb] = 0;	// in case GetCurrentDirectory() failed
		return m_psz;	// we leave the full buffer allocated in case you want to add a filename to the end
	}

	void AppendFileName(const char* pszName) {
#ifdef _DEBUG
		ttASSERT_MSG(m_psz, "NULL pointer!");
		ttASSERT(sizeBuffer() >= MAX_PATH);
#endif
		if (m_psz) {
			tt::AddTrailingSlash(m_psz);
			strcat(pszName);
		}
	}

#else	// not _WINDOWS_
	char* getCWD() {
		resize(4096);
		char* psz = getcwd(m_psz, 4096);
		if (!psz)
			m_psz[0] = 0;	// in case getcwd() failed
		return m_psz;		// we leave the full buffer allocated in case you want to add a filename to the end
	}
#endif	// _WINDOWS_

	char* cdecl printf(const char* pszFormat, ...) {	// This will free m_psz if needed, and automatically malloc the needed size
						va_list argList;
						va_start(argList, pszFormat);
 						tt::vprintf(&m_psz, pszFormat, argList);
						va_end(argList);
						return m_psz;
					}

	operator char*()  { return (char*) m_psz; };
	operator void*()  { return (void*) m_psz; }

	char operator [] (size_t pos) { return (m_psz ? m_psz[pos] : 0); }	// Beware! no check for pos beyond end of string!
	char operator [] (int pos)    { return (m_psz ? m_psz[pos] : 0); }	// Beware! no check for pos beyond end of string!

	void operator = (const char* psz) { if (m_psz) tt::free(m_psz); m_psz = tt::strdup(psz); }

	bool operator == (const char* psz) { return (isempty() || !psz) ? false : tt::samestr(m_psz, psz); } // samestr will check for m_psz == null
	bool operator == (char* psz) { return (isempty() || !psz) ? false : tt::samestr(m_psz, psz); }		 // samestr will check for m_psz == null

	char* m_psz;
};

#endif	//__TTLIB_STR_H__
