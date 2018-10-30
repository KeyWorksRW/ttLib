/////////////////////////////////////////////////////////////////////////////
// Name:		kstr.h
// Purpose:		all the kstr...() functions
// Author:		Ralph Walden
// Copyright:   Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
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

// UTF8 versions:

void		kstrcat(char* pszDst, size_t cchDest, const char* pszSrc);
void		kstrcat(char* pszDst, const char* pszSrc);	// only use this if you are absolutley certain pszDst is large enough
char*		kstrchr(const char* psz, char ch);
char*		kstrchrR(const char* psz, char ch);	// returns nullptr if not found, works on UTF8 strings (unlike Windows StrChrR)
bool		kstrcmp(const char* psz1, const char* psz2);
void		kstrcpy(char* pszDst, size_t cchDest, const char* pszSrc);	// will ALWAYS null-terminate destination string (unlike std::strcpy())
const char* kstrext(const char* pszPath, const char* pszExt);			// find a case-insensitive extension in a path string
char*		kstristr(const char* pszMain, const char* pszSub);
char*		kstrstr(const char* pszMain, const char* pszSub);
size_t		kstrlen(const char* psz);
char*		knextchr(const char *psz);	// handles UTF8 strings

// wide character versions:

void		kstrcat(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc);
wchar_t*	kstrchr(const wchar_t* psz, wchar_t ch);
wchar_t*	kstrchrR(const wchar_t* psz, wchar_t ch);
bool		kstrcmp(const wchar_t* psz1, const wchar_t* psz2);
void		kstrcpy(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc);
wchar_t*	kstristr(const wchar_t* pszMain, const wchar_t* pszSub);
wchar_t*	kstrstr(const wchar_t* pszMain, const wchar_t* pszSub);
size_t		kstrlen(const wchar_t* pwsz);

// Use kstrlen() to get the number of characters without trailing zero, use kstrbyte() to get the number of
// bytes including the terminating zero

__inline size_t	kstrbyte(const char* psz) { return kstrlen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
__inline size_t	kstrbyte(const wchar_t* psz) { return kstrlen(psz) * sizeof(wchar_t) + sizeof(wchar_t); }

// The following functions allow you to call them without specifying the length of the destination buffer.
// This is primarily for ease of converting non-secure strcpy() and strcat() functions -- simply add a 'k' in
// front of the function name and you are good to go.

__inline void  kstrcpy(char* pszDst, const char* pszSrc) { kstrcpy(pszDst, _KSTRMAX, pszSrc); }
__inline void  kstrcat(wchar_t* pszDst, const wchar_t* pszSrc) { kstrcat(pszDst, _KSTRMAX, pszSrc); }
__inline void  kstrcpy(wchar_t* pszDst, const wchar_t* pszSrc) { kstrcpy(pszDst, _KSTRMAX, pszSrc); }

#endif	//__TTLIB_KSTR_H__
