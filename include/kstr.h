/////////////////////////////////////////////////////////////////////////////
// Name:		kstr.h
// Purpose:		all the kstr...() functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018 KeyWorks Software (Ralph Walden)
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

#ifndef __TTLIB_KSTR_H__
#define __TTLIB_KSTR_H__

namespace tt {
	const size_t MAX_STRING_LEN = 0x00FFFFFF;	// strings limited to 16,777,215 bytes

	// UTF8 versions:

	int			strcat_s(char* pszDst, size_t cchDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	char*		strchr(const char* psz, char ch);
	char*		strchrR(const char* psz, char ch);	// returns nullptr if not found, works on UTF8 strings
	bool		strcmp(const char* psz1, const char* psz2);
	int			strcpy_s(char* pszDst, size_t cchDest, const char* pszSrc);	// will ALWAYS null-terminate destination string (unlike std::strcpy_s())
	const char* strext(const char* pszPath, const char* pszExt);			// find a case-insensitive extension in a path string
	char*		stristr(const char* pszMain, const char* pszSub);
	char*		strstr(const char* pszMain, const char* pszSub);
	size_t		strlen(const char* psz);
	char*		nextchr(const char *psz);	// handles UTF8 strings

	[[deprecated("use strcat_s instead")]]
	inline int	strcat(char* pszDst, size_t cchDest, const char* pszSrc) { return strcat_s(pszDst, cchDest, pszSrc); }
	[[deprecated("use strcpy_s instead")]]
	inline int	strcpy(char* pszDst, size_t cchDest, const char* pszSrc) { return strcpy_s(pszDst, cchDest, pszSrc); }

	inline char* strcat(char* pszDst, const char* pszSrc) { strcat_s(pszDst, MAX_STRING_LEN, pszSrc); return pszDst; }
	inline char* strcpy(char* pszDst, const char* pszSrc) { strcpy_s(pszDst, MAX_STRING_LEN, pszSrc); return pszDst; }

	// wide character versions:

	int			strcat_s(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc);
	wchar_t*	strchr(const wchar_t* psz, wchar_t ch);
	wchar_t*	strchrR(const wchar_t* psz, wchar_t ch);
	bool		strcmp(const wchar_t* psz1, const wchar_t* psz2);
	int			strcpy_s(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc);
	wchar_t*	stristr(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t*	strstr(const wchar_t* pszMain, const wchar_t* pszSub);
	size_t		strlen(const wchar_t* pwsz);

	inline int	strcat(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc) { return strcat_s(pszDst, cchDest, pszSrc); }
	inline int	strcpy(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc) { return strcpy_s(pszDst, cchDest, pszSrc); }

	// Use strlen() to get the number of characters without trailing zero, use strbyte() to get the number of
	// bytes including the terminating zero

	inline size_t	strbyte(const char* psz) { return tt::strlen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
	inline size_t	strbyte(const wchar_t* psz) { return tt::strlen(psz) * sizeof(wchar_t) + sizeof(wchar_t); }

	// The following functions allow you to call them without specifying the length of the destination buffer.
	// This is primarily for ease of converting non-secure strcpy() and strcat() functions -- simply add 'tt::' in
	// front of the function name and you are good to go.

	inline void  strcat(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcat(pwszDst, tt::MAX_STRING_LEN, pszSrc); }
	inline void  strcpy(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strcpy(pwszDst, tt::MAX_STRING_LEN, pszSrc); }
}	// tt namespace

// The following is for backwards compatibility with older code that used these. Newer code should use the namespace
// versions above. Older code that is being updated should #define __TTLIB_NOREMAP_KSTR_H__ to find calls to the old
// versions that need updating. Be aware that all of these will ultimately be removed -- no new code should be using them.

#ifndef __TTLIB_NOREMAP_KSTR_H__

// UTF8 versions:

	inline void		kstrcat(char* pszDst, size_t cchDest, const char* pszSrc) { tt::strcat_s(pszDst, cchDest, pszSrc); }
	inline void		kstrcat(char* pszDst, const char* pszSrc) { tt::strcat_s(pszDst, tt::MAX_STRING_LEN, pszSrc); } // only use this if you are absolutley certain pszDst is large enough
	inline char*	kstrchr(const char* psz, char ch) { return tt::strchr(psz, ch); }
	inline char*	kstrchrR(const char* psz, char ch) { return tt::strchrR(psz, ch); };	// returns nullptr if not found, works on UTF8 strings (unlike Windows StrChrR)
	inline bool		kstrcmp(const char* psz1, const char* psz2) { return tt::strcmp(psz1, psz2); }
	inline void		kstrcpy(char* pszDst, size_t cchDest, const char* pszSrc) { tt::strcpy_s(pszDst, cchDest, pszSrc); } // will ALWAYS null-terminate destination string (unlike std::strcpy())
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

	inline void  kstrcpy(char* pszDst, const char* pszSrc) { tt::strcpy_s(pszDst, tt::MAX_STRING_LEN, pszSrc); }
	inline void  kstrcat(wchar_t* pwszDst, const wchar_t* pwszSrc) { tt::strcat(pwszDst, tt::MAX_STRING_LEN, pwszSrc); }
	inline void  kstrcpy(wchar_t* pwszDst, const wchar_t* pwszSrc) { tt::strcpy(pwszDst, tt::MAX_STRING_LEN, pwszSrc); }

#endif	// __TTLIB_NOREMAP_KSTR_H__
#endif	//__TTLIB_KSTR_H__
