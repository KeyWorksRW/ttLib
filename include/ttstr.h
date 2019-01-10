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

namespace tt
{
	const size_t MAX_STRING_LEN = 0x00FFFFFF;	// strings limited to 16,777,215 bytes

	// UTF8 versions:

	int			strcat_s(char* pszDst, size_t cchDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	char*		strchr(const char* psz, char ch);
	char*		strchrR(const char* psz, char ch);	// returns nullptr if not found, works on UTF8 strings
	int			strcpy_s(char* pszDst, size_t cchDest, const char* pszSrc);	// will ALWAYS null-terminate destination string (unlike std::strcpy_s())
	const char* strext(const char* pszPath, const char* pszExt);			// find a case-insensitive extension in a path string
	char*		stristr(const char* pszMain, const char* pszSub);
	char*		strstr(const char* pszMain, const char* pszSub);
	size_t		strlen(const char* psz);

	bool		samestr(const char* psz1, const char* psz2);	// same as strcmp, but returns true/false
	bool		samestri(const char* psz1, const char* psz2);	// case-insensitive comparison
	bool 		samesubstr(const char* pszMain, const char* pszSub);	// true if sub string matches first part of main string
	bool 		samesubstri(const char* pszMain, const char* pszSub);	// case-insensitive comparison

	const char*	nextchr(const char * psz);		// handles UTF8 strings
	const char* nextnonspace(const char* psz);	// returns pointer to the next non-space character
	const char* nextspace(const char* psz);		// returns pointer to the next space character

	const wchar_t* nextnonspace(const wchar_t* psz);	// returns pointer to the next non-space character
	const wchar_t* nextspace(const wchar_t* psz);		// returns pointer to the next space character

	inline	bool isalpha(char ch) { return ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }
	inline	bool isdigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }
	inline	bool isempty(const char* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool isnonempty(const char* psz) { return (psz != nullptr && psz[0]); }
	inline	bool ispunct(char ch) { return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!'); }
	inline	bool isutf8(char ch) { return ((ch & 0xC0) != 0x80); }	// is ch the start of a utf8 sequence?
	inline	bool iswhitespace(char ch) { return tt::strchr(" \t\r\n\f", ch) ? true : false; };

	inline	bool isalpha(wchar_t ch) { return ( (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')); }
	inline	bool isdigit(wchar_t ch) { return ((ch >= L'0' && ch <= L'9') || ch == L'-'); }
	inline	bool isempty(const wchar_t* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool isnonempty(const wchar_t* psz) { return (psz != nullptr && psz[0]); }
	inline	bool ispunct(wchar_t ch) { return (ch == L'.' || ch == L',' || ch == L';' || ch == L':' || ch == L'?' || ch == L'!'); }
	inline	bool iswhitespace(wchar_t ch) { return (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'\f') ? true : false; }

	// force "normal" calls to secure version

	inline char* strcat(char* pszDst, const char* pszSrc) { strcat_s(pszDst, MAX_STRING_LEN, pszSrc); return pszDst; }
	inline char* strcpy(char* pszDst, const char* pszSrc) { strcpy_s(pszDst, MAX_STRING_LEN, pszSrc); return pszDst; }

	// wide character versions:

	int			strcat_s(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc);
	wchar_t*	strchr(const wchar_t* psz, wchar_t ch);
	wchar_t*	strchrR(const wchar_t* psz, wchar_t ch);
	int			strcpy_s(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc);
	wchar_t*	stristr(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t*	strstr(const wchar_t* pszMain, const wchar_t* pszSub);
	size_t		strlen(const wchar_t* pwsz);

	bool		samestr(const wchar_t* psz1, const wchar_t* psz2);	// same as strcmp, but returns true/false
	bool 		samestri(const wchar_t* psz1, const wchar_t* psz2);	// case-insensitive comparison
	bool 		samesubstr(const wchar_t* pszMain, const wchar_t* pszSub);	// true if sub string matches first part of main string
	bool		samesubstri(const wchar_t* pszMain, const wchar_t* pszSub);	// case-insensitive comparison

	// Use strlen() to get the number of characters without trailing zero, use strbyte() to get the number of
	// bytes including the terminating zero

	inline size_t	strbyte(const char* psz) { return tt::strlen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
	inline size_t	strbyte(const wchar_t* psz) { return tt::strlen(psz) * sizeof(wchar_t) + sizeof(wchar_t); }

	// The following functions allow you to call them without specifying the length of the destination buffer.
	// This is primarily for ease of converting non-secure strcpy() and strcat() functions -- simply add 'tt::' in
	// front of the function name and you are good to go.

	inline void  strcat(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcat_s(pwszDst, tt::MAX_STRING_LEN, pszSrc); }
	inline void  strcpy(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcpy_s(pwszDst, tt::MAX_STRING_LEN, pszSrc); }

	void trim_right(char* psz);

	ptrdiff_t atoi(const char* psz);
	char*	  hextoa(size_t val, char* pszDst, bool bUpperCase);
	char*	  itoa(int32_t val, char* pszDst, size_t cbDst);
	char*	  itoa(int64_t val, char* pszDst, size_t cbDst);
	char*	  utoa(uint32_t val, char* pszDst, size_t cbDst);
	char*	  utoa(uint64_t val, char* pszDst, size_t cbDst);

	// wide-char versions

	ptrdiff_t	atoi(const wchar_t* psz);
	wchar_t*	hextoa(size_t val, wchar_t* pszDst, bool bUpperCase);
	wchar_t*	itoa(int32_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*	itoa(int64_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*	utoa(uint32_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*	utoa(uint64_t val, wchar_t* pszDst, size_t cbDst);

} // end of tt namespace

#endif	//__TTLIB_STR_H__
