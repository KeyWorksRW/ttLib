/////////////////////////////////////////////////////////////////////////////
// Name:		ttutil.h
// Purpose:		Various utility functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTUTIL_H__
#define __TTUTIL_H__

// ANSI versions

int		  ttstrcat(char* pszDst, const char* pszSrc);
int		  ttstrcat(char* pszDst, size_t cbDest, const char* pszSrc);
int		  ttstrcpy(char* pszDst, const char* pszSrc);
int		  ttstrcpy(char* pszDst, size_t cbDest, const char* pszSrc);
size_t	  ttstrlen(const char* psz);
char*	  ttstrchr(const char* psz, char ch);
char*	  ttstrrchr(const char* psz, char ch);
char*	  ttstrstr(const char* pszMain, const char* pszSub);
char*	  ttstristr(const char* pszMain, const char* pszSub);

char*	  ttFindExt(const char* pszPath, const char* pszExt);
char*	  ttFindLastSlash(const char* psz);
char*	  ttFindNonSpace(const char* psz);
char*	  ttFindSpace(const char* psz);
char*	  ttNextChar(const char*psz);
char*	  ttStepOver(const char* psz);
void	  ttTrimRight(char* psz);

bool	  ttIsSameStr(const char* psz1, const char* psz2);
bool	  ttIsSameStrI(const char* psz1, const char* psz2);
bool	  ttIsSameSubStr(const char* pszMain, const char* pszSub);
bool	  ttIsSameSubStrI(const char* pszMain, const char* pszSub);

ptrdiff_t ttAtoi(const char* psz);
char*	  ttHextoa(size_t val, char* pszDst, bool bUpperCase);
char*	  ttItoa(int32_t val, char* pszDst, size_t cbDst);
char*	  ttItoa(int64_t val, char* pszDst, size_t cbDst);
char*	  ttUtoa(uint32_t val, char* pszDst, size_t cbDst);
char*	  ttUtoa(uint64_t val, char* pszDst, size_t cbDst);

void	  ttAddTrailingSlash(char* psz);

// Use strLen() to get the number of characters without trailing zero, use strByteLen() to get the number of
// bytes including the terminating zero

inline size_t ttStrByteLen(const char* psz) { return ttstrlen(psz) * sizeof(char) + sizeof(char); }

inline bool ttIsAlpha(char ch) { return ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }
inline bool ttIsDigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }
inline bool ttIsEmpty(const char* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
inline bool ttIsNonEmpty(const char* psz) { return (psz != nullptr && psz[0]); }
inline bool ttIsPunct(char ch) { return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!'); }
inline bool ttIsUTF8(char ch) { return ((ch & 0xC0) != 0x80); }	// is ch the start of a utf8 sequence?
inline bool ttIsWhitespace(char ch) { return ttstrchr(" \t\r\n\f", ch) ? true : false; };

/////////// UNICODE versions /////////////////////

int		  ttstrcat(wchar_t* pszDst, const wchar_t* pszSrc);
int		  ttstrcat(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc);
int		  ttstrcpy(wchar_t* pszDst, const wchar_t* pszSrc);
int		  ttstrcpy(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc);
size_t	  ttstrlen(const wchar_t* pwsz);
wchar_t*  ttstrchr(const wchar_t* psz, wchar_t ch);
wchar_t*  ttstrrchr(const wchar_t* psz, wchar_t ch);
wchar_t*  ttstrstr(const wchar_t* pszMain, const wchar_t* pszSub);

wchar_t*  ttFindExt(const wchar_t* pszPath, const wchar_t* pszExt);
wchar_t*  ttFindSpace(const wchar_t* psz);
wchar_t*  ttFindNonSpace(const wchar_t* psz);
wchar_t*  ttStepOver(const wchar_t* psz);

bool	  ttIsSameStr(const wchar_t* psz1, const wchar_t* psz2);
bool	  ttIsSameStrI(const wchar_t* psz1, const wchar_t* psz2);
bool	  ttIsSameSubStr(const wchar_t* pszMain, const wchar_t* pszSub);
bool	  ttIsSameSubStrI(const wchar_t* pszMain, const wchar_t* pszSub);

ptrdiff_t ttAtoi(const wchar_t* psz);
wchar_t*  ttHextoa(size_t val, wchar_t* pszDst, bool bUpperCase);
wchar_t*  ttItoa(int32_t val, wchar_t* pszDst, size_t cbDst);
wchar_t*  ttItoa(int64_t val, wchar_t* pszDst, size_t cbDst);
wchar_t*  ttUtoa(uint32_t val, wchar_t* pszDst, size_t cbDst);
wchar_t*  ttUtoa(uint64_t val, wchar_t* pszDst, size_t cbDst);

inline size_t ttStrByteLen(const wchar_t* pwsz) { return ttstrlen(pwsz) * sizeof(wchar_t) + sizeof(wchar_t); }

inline bool ttIsAlpha(wchar_t ch) { return ( (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')); }
inline bool ttIsDigit(wchar_t ch) { return ((ch >= L'0' && ch <= L'9') || ch == L'-'); }
inline bool ttIsEmpty(const wchar_t* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
inline bool ttIsNonEmpty(const wchar_t* psz) { return (psz != nullptr && psz[0]); }
inline bool ttIsPunct(wchar_t ch) { return (ch == L'.' || ch == L',' || ch == L';' || ch == L':' || ch == L'?' || ch == L'!'); }
inline bool ttIsWhitespace(wchar_t ch) { return (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'\f') ? true : false; }

#endif // __TTUTIL_H__
