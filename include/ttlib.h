/////////////////////////////////////////////////////////////////////////////
// Name:		ttlib.h
// Purpose:		Master header file for ttLib
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_H__
#define __TTLIB_H__

class ttString;	 // forward definition

// The tt namespace is used in other ttLib header files as well, so this is not a complete list. It is STRONGLY
// recommended that you reference all functions in ttLib with "tt::". Do NOT declare "using namespace tt" unless you want
// other function calls in your code to be replaced. See https://github.com/KeyWorksRW/ttLib/issues/12 for discussion of
// the issue.

namespace tt {
	const size_t MAX_STRING_LEN = 0x00FFFFFF;	// strings limited to 16,777,215 bytes

	char* findchr(const char* psz, char ch);
	char* findlastchr(const char* psz, char ch);				// returns nullptr if not found, works on UTF8 strings
	char* findstr(const char* pszMain, const char* pszSub);
	char* findstri(const char* pszMain, const char* pszSub);
	char* findext(const char* pszPath, const char* pszExt);		// find a case-insensitive extension in a path string

	wchar_t* findchr(const wchar_t* psz, wchar_t ch);
	wchar_t* findlastchr(const wchar_t* psz, wchar_t ch);	// returns nullptr if not found, works on UTF8 strings
	wchar_t* findstr(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* findstri(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* findext(const wchar_t* pszPath, const wchar_t* pszExt);	// find a case-insensitive extension in a path string

	bool samestr(const char* psz1, const char* psz2);			// same as strcmp, but returns true/false
	bool samestri(const char* psz1, const char* psz2);			// case-insensitive comparison
	bool samesubstr(const char* pszMain, const char* pszSub);	// true if sub string matches first part of main string
	bool samesubstri(const char* pszMain, const char* pszSub);	// case-insensitive comparison

	bool samestr(const wchar_t* psz1, const wchar_t* psz2); 			// same as strcmp, but returns true/false
	bool samestri(const wchar_t* psz1, const wchar_t* psz2);			// case-insensitive comparison
	bool samesubstr(const wchar_t* pszMain, const wchar_t* pszSub);		// true if sub string matches first part of main string
	bool samesubstri(const wchar_t* pszMain, const wchar_t* pszSub);	// case-insensitive comparison

	char* nextchr(const char * psz);			// handles UTF8 strings
	char* nextnonspace(const char* psz);		// returns pointer to the next non-space character
	char* nextspace(const char* psz);			// returns pointer to the next space character
	char* stepover(const char*);				// find next space character, and return pointer to first non-space character after that

	wchar_t* nextnonspace(const wchar_t* psz);	// returns pointer to the next non-space character
	wchar_t* nextspace(const wchar_t* psz);		// returns pointer to the next space character
	wchar_t* stepover(const wchar_t*);			// find next space character, and return pointer to first non-space character after that

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

	int			strcat_s(char* pszDst, size_t cbDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	int			strcpy_s(char* pszDst, size_t cbDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	size_t		strlen(const char* psz);

	int			strcat_s(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc);
	int			strcpy_s(wchar_t* pwszDst, size_t cbDest, const wchar_t* pwszSrc);
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

	// printf/vprintf provides a sub-set of the standard sprintf format codes, with automatic allocation of sufficient memory to hold
	// the result, along with some special format specifiers.
	//
	//		standard: c, C, d, i, u, x, X, s, S 	(no floating point, precision or padding)
	//
	//		%kd - formats an integer with commas. I.e., 54321 would be formatted as 54,321
	//		%kq - outputs quotation marks around the string
	//		%ks - adds a 's' to the current buffer if the integer is zero or greater then 1, e.g., printf("item%ks", cItems);
	//		%kS - adds a 's' to the current buffer if the __int64 is zero or greater then 1
	//		%kt - formats a size_t value with commas
	//		%ku - formats an unsigned integer with commas

	//		%kI64d -- handles int64_t, adding commas if needed
	//		%kI64u -- handles uint64_t, adding commas if needed

	// The following are only valid when compiling for _WINDOWS_

	//		%ke - formats a system message assuming the argument is an error number
	//		%kr - argument is a resource identifier to a string

	char* cdecl printf(char** ppszDst, const char* pszFormat, ...);		// CAUTION! The memory ppszDst points to will be modified by ttHeap functions
	void vprintf(char** ppszDst, const char* pszFormat, va_list argList);

#ifdef _WINDOWS_
	extern HWND hwndParent;	// parent for MessageBox, and if Abort is requested in ttASSERT, will receive a WM_CLOSE message prior to shut down
	extern HINSTANCE hinstResources;	// handle to use to load resources
	extern const char* pszMsgTitle;		// title for message boxes
	extern size_t LanguageOffset;		// language offset used to load other languages from .rc file
#endif

	void		ConvertToRelative(const char* pszRoot, const char* pszFile, ttString& cszResult);
	bool		CreateDir(const char* pszDir);
	bool		CreateDir(const wchar_t* pszDir);
	bool		DirExists(const char* pszFolder);
	bool		DirExists(const wchar_t* pszFolder);
	bool		FileExists(const char* pszFile);
	bool		FileExists(const wchar_t* pszFile);
	bool		IsValidFileChar(const char* psz, size_t pos);

	void		BackslashToForwardslash(char* pszPath);
	void		ForwardslashToBackslash(char* pszPath);

	void		AddTrailingSlash(char* pszPath);
	char* 		FindLastSlash(const char* pszPath);		// handles both forward and back slashes

	// fndExtension() properly handles directories and filenames that start with '.' -- i.e., tt::fndExtension(".gitignore") will return nullptr
	char*		fndExtension(const char* pszPath);		// returns pointer to the '.' that begins a file name extension, or nullptr

	// fndFilename() properly handles a path with either forward and back slashes (or a mix of both)
	char*		fndFilename(const char* pszPath);		// returns pointer to the filename portion of a path

	// Temporarily here for backward compatibility
	inline char* FindExtPortion(const char* pszPath)  { return tt::fndExtension(pszPath); }
	inline char* FindFilePortion(const char* pszPath) { return tt::fndFilename(pszPath); }

	size_t		HashFromSz(const char* psz);
	size_t		HashFromSz(const wchar_t* psz);
	size_t		HashFromURL(const char* pszURL);
	size_t		HashFromURL(const wchar_t* pszURL);

#ifdef _WINDOWS_
	void		InitCaller(HINSTANCE hinstRes, HWND hwndParent, const char* pszMsgTitle);
	inline void	InitCaller(const char* pszTitle) { InitCaller(GetModuleHandle(nullptr), nullptr, pszTitle); }	// use this for console apps

	const char* GetResString(size_t idString);
	const char* LoadTxtResource(int idRes, uint32_t* pcbFile = nullptr, HINSTANCE hinst = tt::hinstResources);

	int 		MsgBox(UINT idResource, UINT uType = MB_OK | MB_ICONWARNING);
	int 		MsgBox(const char* pszMsg, UINT uType = MB_OK | MB_ICONWARNING);
	int cdecl	MsgBoxFmt(const char* pszFormat, UINT uType, ...);
	int cdecl	MsgBoxFmt(int idResource, UINT uType, ...);

	ptrdiff_t	CompareFileTime(FILETIME* pftSrc, FILETIME* pftDst);
	HFONT		CreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold = false, bool fItalics = false);

	void cdecl	KeyTrace(const char* pszFormat, ...);	// formats a string and displays it in a KeyView window (if KeyView is running)

	inline int	RC_HEIGHT(const RECT* prc) { return prc->bottom - prc->top; };
	inline int	RC_HEIGHT(const RECT rc) { return rc.bottom - rc.top; };
	inline int	RC_WIDTH(const RECT* prc) { return prc->right - prc->left; };
	inline int	RC_WIDTH(const RECT rc) { return rc.right - rc.left; };

	inline bool PosInRect(const RECT* prc, int xPos, int yPos) { return (xPos >= prc->left && xPos <= prc->right && yPos >= prc->top && yPos <= prc->bottom); }

	inline bool	IsValidWindow(HWND hwnd) { return (bool) (hwnd && IsWindow(hwnd)); };
#endif	// _WINDOWS_
} // end of tt namespace

namespace ttch {
	const char CH_OPEN_PAREN =  '(';
	const char CH_CLOSE_PAREN = ')';
	const char CH_COLON =		':';
	const char CH_SEMICOLON =	';';
	const char CH_START_QUOTE = '`';
	const char CH_SQUOTE =		'\'';
	const char CH_END_QUOTE =	'\'';
	const char CH_QUOTE =		'"';
	const char CH_EQUAL =		'=';
	const char CH_SPACE =		' ';
	const char CH_COMMA =		',';
	const char CH_LEFT_BRACKET = '[';
	const char CH_RIGHT_BRACKET = ']';
	const char CH_TAB =			'\t';
	const char CH_BACKSLASH =	 '\\';
	const char CH_FORWARDSLASH = '/';
} // end of ttch namespace

#endif	// __TTLIB_H__

#ifdef __TTLIB_INCLUDE_MAJOR__	// include major ttLib header files
	#include "ttdebug.h"		// ttASSERT and friends
	#include "ttstr.h"			// light weight string class
	#include "ttstring.h"		// full-blown string class
	#include "ttlist.h" 		// class for handling lists of strings
	#include "ttfile.h" 		// class for reading and writing to files
#endif
