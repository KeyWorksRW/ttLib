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
#include "ttheap.h"	// ttHeap

namespace tt
{
	const size_t MAX_STRING_LEN = 0x00FFFFFF;	// strings limited to 16,777,215 bytes

	char* findchr(const char* psz, char ch);
	char* findlastchr(const char* psz, char ch);	// returns nullptr if not found, works on UTF8 strings
	char* findstr(const char* pszMain, const char* pszSub);
	char* findstri(const char* pszMain, const char* pszSub);
	char* findext(const char* pszPath, const char* pszExt);			// find a case-insensitive extension in a path string

	wchar_t* findchr(const wchar_t* psz, wchar_t ch);
	wchar_t* findlastchr(const wchar_t* psz, wchar_t ch);	// returns nullptr if not found, works on UTF8 strings
	wchar_t* findstr(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* findstri(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* findext(const wchar_t* pszPath, const wchar_t* pszExt);			// find a case-insensitive extension in a path string

	bool samestr(const char* psz1, const char* psz2);	// same as strcmp, but returns true/false
	bool samestri(const char* psz1, const char* psz2);	// case-insensitive comparison
	bool samesubstr(const char* pszMain, const char* pszSub);	// true if sub string matches first part of main string
	bool samesubstri(const char* pszMain, const char* pszSub);	// case-insensitive comparison

	bool samestr(const wchar_t* psz1, const wchar_t* psz2); 			// same as strcmp, but returns true/false
	bool samestri(const wchar_t* psz1, const wchar_t* psz2);			// case-insensitive comparison
	bool samesubstr(const wchar_t* pszMain, const wchar_t* pszSub);		// true if sub string matches first part of main string
	bool samesubstri(const wchar_t* pszMain, const wchar_t* pszSub);	// case-insensitive comparison

	char*	nextchr(const char * psz);		// handles UTF8 strings
	char* nextnonspace(const char* psz);	// returns pointer to the next non-space character
	char* nextspace(const char* psz);		// returns pointer to the next space character

	wchar_t* nextnonspace(const wchar_t* psz);	// returns pointer to the next non-space character
	wchar_t* nextspace(const wchar_t* psz);		// returns pointer to the next space character

	inline	bool isalpha(char ch) { return ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }
	inline	bool isdigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }
	inline	bool isempty(const char* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool isnonempty(const char* psz) { return (psz != nullptr && psz[0]); }
	inline	bool ispunct(char ch) { return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!'); }
	inline	bool isutf8(char ch) { return ((ch & 0xC0) != 0x80); }	// is ch the start of a utf8 sequence?
	inline	bool iswhitespace(char ch) { return tt::findchr(" \t\r\n\f", ch) ? true : false; };

	inline	bool isalpha(wchar_t ch) { return ( (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')); }
	inline	bool isdigit(wchar_t ch) { return ((ch >= L'0' && ch <= L'9') || ch == L'-'); }
	inline	bool isempty(const wchar_t* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool isnonempty(const wchar_t* psz) { return (psz != nullptr && psz[0]); }
	inline	bool ispunct(wchar_t ch) { return (ch == L'.' || ch == L',' || ch == L';' || ch == L':' || ch == L'?' || ch == L'!'); }
	inline	bool iswhitespace(wchar_t ch) { return (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'\f') ? true : false; }

	inline char* strchr(const char* psz, char ch) { return findchr(psz, ch); }
	inline char* strchrR(const char* psz, char ch) { return findlastchr(psz, ch); }
	inline char* strstr(const char* pszMain, const char* pszSub) { return findstr(pszMain, pszSub); }
	inline char* stristr(const char* pszMain, const char* pszSub){ return findstri(pszMain, pszSub); }

	inline wchar_t* strchr(const wchar_t* psz, char ch) { return findchr(psz, ch); }
	inline wchar_t* strchrR(const wchar_t* psz, char ch) { return findlastchr(psz, ch); }
	inline wchar_t* strstr(const wchar_t* pszMain, const wchar_t* pszSub) { return findstr(pszMain, pszSub); }
	inline wchar_t* stristr(const wchar_t* pszMain, const wchar_t* pszSub){ return findstri(pszMain, pszSub); }

	int			strcat_s(char* pszDst, size_t cchDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	int			strcpy_s(char* pszDst, size_t cchDest, const char* pszSrc);	// will ALWAYS null-terminate destination string (unlike std::strcpy_s())
	size_t		strlen(const char* psz);

	int			strcat_s(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc);
	int			strcpy_s(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc);
	size_t		strlen(const wchar_t* pwsz);

	// force "normal" calls to secure version -- possible buffer overflow if destination isn't large enough

	inline char* strcat(char* pszDst, const char* pszSrc) { strcat_s(pszDst, tt::MAX_STRING_LEN, pszSrc); return pszDst; }
	inline char* strcpy(char* pszDst, const char* pszSrc) { strcpy_s(pszDst, tt::MAX_STRING_LEN, pszSrc); return pszDst; }

	inline void  strcat(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcat_s(pwszDst, tt::MAX_STRING_LEN, pszSrc); }
	inline void  strcpy(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcpy_s(pwszDst, tt::MAX_STRING_LEN, pszSrc); }

	// Use strlen() to get the number of characters without trailing zero, use strbyte() to get the number of
	// bytes including the terminating zero

	inline size_t	strbyte(const char* psz) { return tt::strlen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
	inline size_t	strbyte(const wchar_t* pwsz) { return tt::strlen(pwsz) * sizeof(wchar_t) + sizeof(wchar_t); }


	void trim_right(char* psz);

	ptrdiff_t atoi(const char* psz);
	char*	  hextoa(size_t val, char* pszDst, bool bUpperCase);
	char*	  itoa(int32_t val, char* pszDst, size_t cbDst);
	char*	  itoa(int64_t val, char* pszDst, size_t cbDst);
	char*	  utoa(uint32_t val, char* pszDst, size_t cbDst);
	char*	  utoa(uint64_t val, char* pszDst, size_t cbDst);

	ptrdiff_t	atoi(const wchar_t* psz);
	wchar_t*	hextoa(size_t val, wchar_t* pszDst, bool bUpperCase);
	wchar_t*	itoa(int32_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*	itoa(int64_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*	utoa(uint32_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*	utoa(uint64_t val, wchar_t* pszDst, size_t cbDst);

} // end of tt namespace

/*
  ttStr can be used as a lightweight, header-only container for a zero-terminated string. It uses the above string functions
  for handling null pointers, overflows, etc. Use the ttString class for a full blown string-handling class (see ttstring.h)

  if (some condition) {
	ttStr szBuf(256);
	tt::strcpy_s(szBuf, szBuf.sizeBuffer(), "some text");
	szBuf.strcat(" and some more text");	// this will truncate if string to add is too long
	cout << (char*) szBuf;
  } // szBuf is freed because it went out of scope
*/

// ttStr can be used as a lightweight, header-only container for a zero-terminated UTF8 string. It uses the ttLib string
// functions for handling null pointers, overflows, etc. Use the ttString (ttstring.h) class if you need a full blown
// string-handling class

// CAUTION! strcat() and += are provided but they do NOT allocate more memory -- if the current buffer for ttStr is too
// small, the additional string will be truncated. Use the ttString class if you need dynamic memory allocation

class ttStr
{
public:
	ttStr(void) { m_psz = nullptr; }
	ttStr(size_t size) { m_psz = (char*) tt::malloc(size); }
	ttStr(const char* psz) { m_psz = (char*) tt::strdup(psz); }
	~ttStr(void) {
		if (m_psz)
			tt::free(m_psz);
	}
	void resize(size_t cb) { m_psz = m_psz ? (char*) tt::realloc(m_psz, cb) : (char*) tt::malloc(cb); }

	char*	findext(const char* pszExt) { return (char*) tt::findext(m_psz, pszExt); }	// find filename extension
	char*	findstr(const char* psz) { return tt::findstr(m_psz, psz); }
	char*	findstri(const char* psz) { return tt::findstri(m_psz, psz); }
	char*	findchr(char ch) { return tt::findchr(m_psz, ch); }
	char*	findlastchr(char ch) { return tt::findlastchr(m_psz, ch); }

	size_t	strbyte() { return tt::strbyte(m_psz); }	// length of string including 0 terminator
	int		strcat(const char* psz) { return tt::strcat_s(m_psz, tt::size(m_psz), psz); }	// Does NOT reallocate string!
	void	strcpy(const char* psz) { tt::strcpy_s(m_psz, tt::size(m_psz), psz); }
	size_t	strlen() { return tt::strlen(m_psz); }		// number of characters (use strbyte() for buffer size calculations)

	bool	samestr(const char* psz) { return tt::samestr(m_psz, psz); }
	bool	samestri(const char* psz) { return tt::samestri(m_psz, psz); }
	bool	samesubstr(const char* psz) { return tt::samesubstr(m_psz, psz); }
	bool	samesubstri(const char* psz) { return tt::samesubstri(m_psz, psz); }

	char*	nextnonspace() { return (char*) tt::nextnonspace(m_psz); }
	char*	nextspace() { return (char*) tt::nextspace(m_psz); }

	ptrdiff_t atoi() { return tt::atoi(m_psz); }

	char*	itoa(int32_t val)  { return tt::itoa(val, m_psz, tt::size(m_psz)); };
	char*	itoa(int64_t val)  { return tt::itoa(val, m_psz, tt::size(m_psz)); };
	char*	utoa(uint32_t val) { return tt::utoa(val, m_psz, tt::size(m_psz)); };
	char*	utoa(uint64_t val) { return tt::utoa(val, m_psz, tt::size(m_psz)); };

	void	trim_right() { tt::trim_right(m_psz); }

	bool	IsEmpty() { return (!m_psz || !*m_psz)  ? true : false; }
	bool	IsNonEmpty() const { return (m_psz && *m_psz) ? true : false; }
	bool	IsNull() const { return (m_psz == nullptr); }

	size_t	sizeBuffer() { return tt::size(m_psz); }	// returns 0 if m_psz is null

#ifdef _WINDOWS_
	char* getResource(size_t idString) {
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

#else
	char* getCWD() {
		resize(4096);
		char* psz = getcwd(m_psz, 4096);
		if (!psz)
			m_psz[0] = 0;	// in case getcwd() failed
		return m_psz;	// we leave the full buffer allocated in case you want to add a filename to the end
	}
#endif



	operator const char*() { return (const char*) m_psz; };
	operator char*()  { return (char*) m_psz; };
	operator void*()  { return (void*) m_psz; }

	char operator[](size_t pos) { return (m_psz ? m_psz[pos] : 0); }	// Beware! no check for pos beyond end of string!
	char operator[](int pos)    { return (m_psz ? m_psz[pos] : 0); }	// Beware! no check for pos beyond end of string!

	void operator=(const char* psz) { if (m_psz) tt::free(m_psz); m_psz = tt::strdup(psz); }

	bool operator==(const char* psz) { return (IsEmpty() || !psz) ? false : tt::samestr(m_psz, psz); }	// samestr will check for m_psz == null

	void operator+=(const char* psz) { tt::strcat_s(m_psz, tt::size(m_psz) - tt::strlen(m_psz), psz); }	// Does NOT reallocate string!
	void operator+=(char ch) {		// Does NOT reallocate string!
		char szTmp[2];
		szTmp[0] = ch;
		szTmp[1] = 0;
		tt::strcat_s(m_psz, tt::size(m_psz) - tt::strlen(m_psz), szTmp);
	}

	char* m_psz;
};

#endif	//__TTLIB_STR_H__
