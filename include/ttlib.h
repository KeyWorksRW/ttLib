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

class ttCStr;	 // forward definition

extern bool (_cdecl *pttAssertHandlerA)(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);
extern bool (_cdecl *pttAssertHandlerW)(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction, int line);

typedef bool (_cdecl *TTASSERTHANDLERA)(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);
typedef bool (_cdecl *TTASSERTHANDLERW)(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction, int line);

// The tt namespace is used in other ttLib header files as well, so this is not a complete list.

namespace tt {
	const size_t MAX_STRING_LEN = 0x00FFFFFF;	// strings limited to 16,777,215 bytes (16 megabytes)

	char*	 FindChar(const char* psz, char ch);
	char*	 FindLastChar(const char* psz, char ch);				// returns nullptr if not found, works on UTF8 strings
	char*	 FindStr(const char* pszMain, const char* pszSub);
	char*	 FindStrI(const char* pszMain, const char* pszSub);
	char*	 FindNonSpace(const char* psz);		// returns pointer to the next non-space character
	char*	 FindSpace(const char* psz);			// returns pointer to the next space character

	wchar_t* FindChar(const wchar_t* psz, wchar_t ch);
	wchar_t* FindLastChar(const wchar_t* psz, wchar_t ch);	// returns nullptr if not found, works on UTF8 strings
	wchar_t* FindStr(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* FindStrI(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* FindNonSpace(const wchar_t* psz);	// returns pointer to the next non-space character
	wchar_t* FindSpace(const wchar_t* psz);		// returns pointer to the next space character

	bool IsSameStr(const char* psz1, const char* psz2);			// same as strcmp, but returns true/false
	bool IsSameStrI(const char* psz1, const char* psz2);			// case-insensitive comparison
	bool IsSameSubStr(const char* pszMain, const char* pszSub);	// true if sub string matches first part of main string
	bool IsSameSubStrI(const char* pszMain, const char* pszSub);	// case-insensitive comparison

	bool IsSameStr(const wchar_t* psz1, const wchar_t* psz2);			// same as strcmp, but returns true/false
	bool IsSameStrI(const wchar_t* psz1, const wchar_t* psz2);			// case-insensitive comparison
	bool IsSameSubStr(const wchar_t* pszMain, const wchar_t* pszSub);		// true if sub string matches first part of main string
	bool IsSameSubStrI(const wchar_t* pszMain, const wchar_t* pszSub);	// case-insensitive comparison

	char*	 NextChar(const char * psz);			// handles UTF8 strings

	char*	 StepOver(const char* psz);		// equivalent to FindNonSpace(FindSpace(psz))
	wchar_t* StepOver(const wchar_t* pwsz);

	inline	bool IsAlpha(char ch) { return ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }
	inline	bool IsDigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }
	inline	bool IsEmpty(const char* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool IsNonEmpty(const char* psz) { return (psz != nullptr && psz[0]); }
	inline	bool IsPunct(char ch) { return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!'); }
	inline	bool IsUTF8(char ch) { return ((ch & 0xC0) != 0x80); }	// is ch the start of a utf8 sequence?
	inline	bool IsWhitespace(char ch) { return tt::FindChar(" \t\r\n\f", ch) ? true : false; };

	inline	bool IsAlpha(wchar_t ch) { return ( (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')); }
	inline	bool IsDigit(wchar_t ch) { return ((ch >= L'0' && ch <= L'9') || ch == L'-'); }
	inline	bool IsEmpty(const wchar_t* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool IsNonEmpty(const wchar_t* psz) { return (psz != nullptr && psz[0]); }
	inline	bool IsPunct(wchar_t ch) { return (ch == L'.' || ch == L',' || ch == L';' || ch == L':' || ch == L'?' || ch == L'!'); }
	inline	bool IsWhitespace(wchar_t ch) { return (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'\f') ? true : false; }

	int			StrCat(char* pszDst, size_t cbDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	int			StrCopy(char* pszDst, size_t cbDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	size_t		StrLen(const char* psz);

	int			StrCat(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc);
	int			StrCopy(wchar_t* pwszDst, size_t cbDest, const wchar_t* pwszSrc);
	size_t		StrLen(const wchar_t* pwsz);

	// force "normal" calls to secure version -- possible buffer overflow if destination isn't large enough

	inline int	StrCat(char* pszDst, const char* pszSrc) { return tt::StrCat(pszDst, tt::MAX_STRING_LEN, pszSrc); }
	inline int	StrCopy(char* pszDst, const char* pszSrc) { return tt::StrCopy(pszDst, tt::MAX_STRING_LEN, pszSrc); }

	inline int	StrCat(wchar_t* pwszDst, const wchar_t* pwszSrc) { return tt::StrCat(pwszDst, tt::MAX_STRING_LEN, pwszSrc); }
	inline int	StrCopy(wchar_t* pwszDst, const wchar_t* pwszSrc) { return tt::StrCopy(pwszDst, tt::MAX_STRING_LEN, pwszSrc); }

	// Use strLen() to get the number of characters without trailing zero, use strByteLen() to get the number of
	// bytes including the terminating zero

	inline size_t StrByteLen(const char* psz) { return tt::StrLen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
	inline size_t StrByteLen(const wchar_t* pwsz) { return tt::StrLen(pwsz) * sizeof(wchar_t) + sizeof(wchar_t); }

	void	  TrimRight(char* psz);

	ptrdiff_t Atoi(const char* psz);
	char*	  Hextoa(size_t val, char* pszDst, bool bUpperCase);
	char*	  Itoa(int32_t val, char* pszDst, size_t cbDst);
	char*	  Itoa(int64_t val, char* pszDst, size_t cbDst);
	char*	  Utoa(uint32_t val, char* pszDst, size_t cbDst);
	char*	  Utoa(uint64_t val, char* pszDst, size_t cbDst);

	ptrdiff_t Atoi(const wchar_t* psz);
	wchar_t*  Hextoa(size_t val, wchar_t* pszDst, bool bUpperCase);
	wchar_t*  Itoa(int32_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*  Itoa(int64_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*  Utoa(uint32_t val, wchar_t* pszDst, size_t cbDst);
	wchar_t*  Utoa(uint64_t val, wchar_t* pszDst, size_t cbDst);

	// printf/vprintf provides a sub-set of the standard sprintf format codes, with automatic allocation of sufficient memory to hold
	// the result, along with some special format specifiers.
	//
	//		standard: c, C, d, i, u, x, X, s, S		(no floating point, precision or padding)
	//
	//		%kd -  formats an integer with commas. I.e., 54321 would be formatted as 54,321
	//		%kq -  outputs quotation marks around the string
	//		%ks -  adds a 's' to the current buffer if the integer is zero or greater then 1, e.g., printf("item%ks", cItems);
	//		%kS -  adds a 's' to the current buffer if the __int64 is zero or greater then 1
	//		%kls - adds a 's' to the current buffer if the last numeric argument is zero or greater then 1 (printf("%d item%kls", cItems);
	//		%kt -  formats a size_t value with commas
	//		%ku -  formats an unsigned integer with commas

	//		%kI64d -- handles int64_t, adding commas if needed
	//		%kI64u -- handles uint64_t, adding commas if needed

	// The following are only valid when compiling for _WINDOWS_

	//		%ke - formats a system message assuming the argument is an error number
	//		%kr - argument is a resource identifier to a string

	char* cdecl printf(char** ppszDst, const char* pszFormat, ...);		// CAUTION! The memory ppszDst points to will be modified by ttHeap functions
	void		vprintf(char** ppszDst, const char* pszFormat, va_list argList);

	void	ConvertToRelative(const char* pszRoot, const char* pszFile, ttCStr& cszResult);
	bool	CreateDir(const char* pszDir);
	bool	CreateDir(const wchar_t* pszDir);

	// The exists functions return true/false base on whether the file or directory actually exists
	bool	DirExists(const char* pszFolder);
	bool	DirExists(const wchar_t* pszFolder);
	bool	FileExists(const char* pszFile);
	bool	FileExists(const wchar_t* pszFile);

	bool	IsValidFileChar(const char* psz, size_t pos);

	void	BackslashToForwardslash(char* pszPath);		// converts all backslashes in the string to forward slashes
	void	ForwardslashToBackslash(char* pszPath);

	void	AddTrailingSlash(char* pszPath);

	// FindExtPortion and FindFilePortion properly handle directories and filenames that start with '.' -- i.e.,
	// tt::FindExtPortion(".gitignore") will return nullptr

	char*	 FindExtPortion(const char* pszPath);	// returns pointer to the '.' that begins a file name extension, or nullptr
	char*	 FindFilePortion(const char* pszPath);	// returns pointer to the filename portion of a path

	char*	 FindExt(const char* pszPath, const char* pszExt);		// find a case-insensitive extension in a path string
	wchar_t* FindExt(const wchar_t* pszPath, const wchar_t* pszExt);
	char*	 FindLastSlash(const char* pszPath);	// handles both forward and back slashes

	size_t	 HashFromSz(const char* psz);			// creates a hash number from an string
	size_t	 HashFromSz(const wchar_t* psz);
	size_t	 HashFromURL(const char* pszURL);		// creates a hash number from a url or filename--'/' and '\' are considered the same, letter case doesn't matter
	size_t	 HashFromURL(const wchar_t* pszURL);

	inline void	SetAssertHandlerA(TTASSERTHANDLERA pFunc) { pttAssertHandlerA = pFunc; }	// Replace the ttLib assertion handler with your own
	inline void	SetAssertHandlerW(TTASSERTHANDLERW pFunc) { pttAssertHandlerW = pFunc; }

#ifdef _WINDOWS_
	extern HWND hwndMsgBoxParent;		// parent for MessageBox--if Abort is requested in ttASSERT, this window will receive a WM_CLOSE message prior to shut down
	extern HINSTANCE hinstResources;	// handle to use to load resources
	extern const char* pszMsgTitle;		// title for message boxes
	extern size_t LanguageOffset;		// language offset used to load other languages from .rc file

	// InitCaller is equivalent to calling setResInst(hinstRes), setMsgBoxParent(hwndParent) and setMsgBoxTitle(pszMsgTitle)
	void		InitCaller(HINSTANCE hinstRes, HWND hwndParent, const char* pszMsgTitle);
	inline void	InitCaller(const char* pszTitle) { InitCaller(GetModuleHandle(nullptr), nullptr, pszTitle); }	// use this for console apps

	inline HINSTANCE   GetResInst() { return hinstResources; }
	inline void		   SetResInst(HINSTANCE hinst) { hinstResources = hinst; }
	inline void		   SetMsgBoxParent(HWND hwnd) { hwndMsgBoxParent = hwnd; }
		   void		   SetMsgBoxTitle(const char* pszMsgTitle);
	inline const char* GetMsgBoxTitle() { return pszMsgTitle; }

	const char* GetResString(size_t idString);
	const char* LoadTxtResource(int idRes, uint32_t* pcbFile = nullptr, HINSTANCE hinst = tt::hinstResources);

	int			MsgBox(UINT idResource, UINT uType = MB_OK | MB_ICONWARNING);
	int			MsgBox(const char* pszMsg, UINT uType = MB_OK | MB_ICONWARNING);
	int cdecl	MsgBoxFmt(const char* pszFormat, UINT uType, ...);
	int cdecl	MsgBoxFmt(int idResource, UINT uType, ...);

	ptrdiff_t	CompareFileTime(FILETIME* pftSrc, FILETIME* pftDst);
	HFONT		CreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold = false, bool fItalics = false);

	inline int	RC_HEIGHT(const RECT* prc) { return prc->bottom - prc->top; };
	inline int	RC_HEIGHT(const RECT rc) { return rc.bottom - rc.top; };
	inline int	RC_WIDTH(const RECT* prc) { return prc->right - prc->left; };
	inline int	RC_WIDTH(const RECT rc) { return rc.right - rc.left; };

	inline bool IsPosInRect(const RECT* prc, int xPos, int yPos) { return (xPos >= prc->left && xPos <= prc->right && yPos >= prc->top && yPos <= prc->bottom); }
	inline bool	IsValidWindow(HWND hwnd) { return (bool) (hwnd && IsWindow(hwnd)); };
#endif	// _WINDOWS_

} // end of tt namespace

namespace ttch {
	const char CH_OPEN_PAREN =	'(';
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
