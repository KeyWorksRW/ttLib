/////////////////////////////////////////////////////////////////////////////
// Name:		kstr.h
// Purpose:		all the kstr...() functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// These functions offer a bit more functionality then either the C++ standard or C++ secure variants. In
// particular, they handle UTF8 strings and they will ASSERT when passed a null pointer when an valid pointer
// is required. If an argument is invalid, the function will attempt to continue without throwing an
// exception (on the assumption that you caught the bug in a _DEBUG build where the ASSERT macro would have
// fired). For example, calling kstrcat(pszMain, cchDest, nullptr) is perfectly valid (but will ASSERT), but
// strcat_s(pszMain, cchDest, nullptr) will throw an exception.

// Note that when compiled with _DEBUG under Windows, ASSERTS will display a MessageBox that will display
// what the problem is (null pointer, empty string, oversized string, etc.) and allow you to break directly
// into a debugger to look at the problem, ignore it if you want to continue running and fix the problem
// later, or simply exit the program.

// When compiled using wxWidgets, kstristr() and IsSameSubString() will do case-insensitive comparisons on
// UTF8 strings. Without wxWidgets, the ASCII portion of a UTF8 string will be case-insensitive, but any
// non-ASCII characters must have an exact case match to be considered equal

#pragma once

#ifndef __TTLIB_KSTR_H__
#define __TTLIB_KSTR_H__

#define _KSTRMAX 0x00FFFFFF		// strings limited to 16,777,215 bytes -- kstr functions will throw an exception if this is exceeded

namespace tt {
	// UTF8 versions:

	void		strcat(char* pszDst, size_t cchDest, const char* pszSrc);
	void		strcat(char* pszDst, const char* pszSrc);	// only use this if you are absolutley certain pszDst is large enough
	char*		strchr(const char* psz, char ch);
	char*		strchrR(const char* psz, char ch);	// returns nullptr if not found, works on UTF8 strings (unlike Windows StrChrR)
	bool		strcmp(const char* psz1, const char* psz2);
	void		strcpy(char* pszDst, size_t cchDest, const char* pszSrc);	// will ALWAYS null-terminate destination string (unlike std::strcpy())
	const char* strext(const char* pszPath, const char* pszExt);			// find a case-insensitive extension in a path string
	char*		stristr(const char* pszMain, const char* pszSub);
	char*		strstr(const char* pszMain, const char* pszSub);
	size_t		strlen(const char* psz);
	char*		nextchr(const char *psz);	// handles UTF8 strings

	// wide character versions:

	void		strcat(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc);
	wchar_t*	strchr(const wchar_t* psz, wchar_t ch);
	wchar_t*	strchrR(const wchar_t* psz, wchar_t ch);
	bool		strcmp(const wchar_t* psz1, const wchar_t* psz2);
	void		strcpy(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc);
	wchar_t*	stristr(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t*	strstr(const wchar_t* pszMain, const wchar_t* pszSub);
	size_t		strlen(const wchar_t* pwsz);

	// Use strlen() to get the number of characters without trailing zero, use strbyte() to get the number of
	// bytes including the terminating zero

	inline size_t	strbyte(const char* psz) { return tt::strlen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
	inline size_t	strbyte(const wchar_t* psz) { return tt::strlen(psz) * sizeof(wchar_t) + sizeof(wchar_t); }

	// The following functions allow you to call them without specifying the length of the destination buffer.
	// This is primarily for ease of converting non-secure strcpy() and strcat() functions -- simply add a 'k' in
	// front of the function name and you are good to go.

	inline void  strcpy(char* pszDst, const char* pszSrc) { tt::strcpy(pszDst, _KSTRMAX, pszSrc); }
	inline void  strcat(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcat(pwszDst, _KSTRMAX, pszSrc); }
	inline void  strcpy(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcpy(pwszDst, _KSTRMAX, pszSrc); }
}	// tt namespace

#ifndef __TTLIB_NOREMAP_KSTR_H__	// #define this if the function names below cause conflicts with your code, or you don't intend to use them

// UTF8 versions:

	inline void		kstrcat(char* pszDst, size_t cchDest, const char* pszSrc) { tt::strcat(pszDst, cchDest, pszSrc); }
	inline void		kstrcat(char* pszDst, const char* pszSrc) { tt::strcat(pszDst, pszSrc); } // only use this if you are absolutley certain pszDst is large enough
	inline char*	kstrchr(const char* psz, char ch) { return tt::strchr(psz, ch); }
	inline char*	kstrchrR(const char* psz, char ch) { return tt::strchrR(psz, ch); };	// returns nullptr if not found, works on UTF8 strings (unlike Windows StrChrR)
	inline bool		kstrcmp(const char* psz1, const char* psz2) { return tt::strcmp(psz1, psz2); }
	inline void		kstrcpy(char* pszDst, size_t cchDest, const char* pszSrc) { tt::strcpy(pszDst, cchDest, pszSrc); } // will ALWAYS null-terminate destination string (unlike std::strcpy())
	inline char*	kstristr(const char* pszMain, const char* pszSub) { return tt::stristr(pszMain, pszSub); }
	inline char*	kstrstr(const char* pszMain, const char* pszSub) { return tt::strstr(pszMain, pszSub); }
	inline size_t	kstrlen(const char* psz) { return tt::strlen(psz); }
	inline char*	knextchr(const char *psz) { return tt::nextchr(psz); }	// handles UTF8 strings
	inline const char* kstrext(const char* pszPath, const char* pszExt) { return tt::strext(pszPath, pszExt); }	// find a case-insensitive extension in a path string

	// wide character versions:

	inline void		kstrcat(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc) { tt::strcat(pwszDst, cchDest, pwszSrc); }
	inline wchar_t*	kstrchr(const wchar_t* pwsz, wchar_t ch) { return tt::strchr(pwsz, ch); }
	inline wchar_t*	kstrchrR(const wchar_t* pwsz, wchar_t ch) { return tt::strchrR(pwsz, ch); }
	inline bool		kstrcmp(const wchar_t* pwsz1, const wchar_t* pwsz2) { return tt::strcmp(pwsz1, pwsz2); }
	inline void		kstrcpy(wchar_t* pwszDst, size_t cchDst, const wchar_t* pwszSrc) { tt::strcpy(pwszDst, cchDst, pwszSrc); }
	inline wchar_t*	kstristr(const wchar_t* pwszMain, const wchar_t* pwszSub) { return tt::stristr(pwszMain, pwszSub); }
	inline wchar_t*	kstrstr(const wchar_t* pwszMain, const wchar_t* pwszSub) { return tt::strstr(pwszMain, pwszSub); }
	inline size_t	kstrlen(const wchar_t* pwsz) { return tt::strlen(pwsz); }

	// Use tt::strlen() to get the number of characters without trailing zero, use tt::strbyte() to get the number of
	// bytes including the terminating zero

	inline size_t	kstrbyte(const char* psz) { return tt::strlen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
	inline size_t	kstrbyte(const wchar_t* pwsz) { return tt::strlen(pwsz) * sizeof(wchar_t) + sizeof(wchar_t); }

	// The following functions allow you to call them without specifying the length of the destination buffer. This is
	// primarily for ease of converting non-secure strcpy() and strcat() functions -- simply add a 'k' in front of the
	// function name and you are good to go.

	inline void  kstrcpy(char* pszDst, const char* pszSrc) { tt::strcpy(pszDst, _KSTRMAX, pszSrc); }
	inline void  kstrcat(wchar_t* pwszDst, const wchar_t* pwszSrc) { tt::strcat(pwszDst, _KSTRMAX, pwszSrc); }
	inline void  kstrcpy(wchar_t* pwszDst, const wchar_t* pwszSrc) { tt::strcpy(pwszDst, _KSTRMAX, pwszSrc); }

#endif	// __TTLIB_NOREMAP_KSTR_H__
#endif	//__TTLIB_KSTR_H__
