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

	char* findChar(const char* psz, char ch);
	char* findLastChar(const char* psz, char ch);				// returns nullptr if not found, works on UTF8 strings
	char* findStr(const char* pszMain, const char* pszSub);
	char* findStri(const char* pszMain, const char* pszSub);
	char* findNonSpace(const char* psz);		// returns pointer to the next non-space character
	char* findSpace(const char* psz);			// returns pointer to the next space character

	wchar_t* findChar(const wchar_t* psz, wchar_t ch);
	wchar_t* findLastChar(const wchar_t* psz, wchar_t ch);	// returns nullptr if not found, works on UTF8 strings
	wchar_t* findStr(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* findStri(const wchar_t* pszMain, const wchar_t* pszSub);
	wchar_t* findNonSpace(const wchar_t* psz);	// returns pointer to the next non-space character
	wchar_t* findSpace(const wchar_t* psz);		// returns pointer to the next space character

	bool isSameStr(const char* psz1, const char* psz2);			// same as strcmp, but returns true/false
	bool isSameStri(const char* psz1, const char* psz2);			// case-insensitive comparison
	bool isSameSubStr(const char* pszMain, const char* pszSub);	// true if sub string matches first part of main string
	bool isSameSubStri(const char* pszMain, const char* pszSub);	// case-insensitive comparison

	bool isSameStr(const wchar_t* psz1, const wchar_t* psz2); 			// same as strcmp, but returns true/false
	bool isSameStri(const wchar_t* psz1, const wchar_t* psz2);			// case-insensitive comparison
	bool isSameSubStr(const wchar_t* pszMain, const wchar_t* pszSub);		// true if sub string matches first part of main string
	bool isSameSubStri(const wchar_t* pszMain, const wchar_t* pszSub);	// case-insensitive comparison

	char* nextChar(const char * psz);			// handles UTF8 strings

	char*	 stepOver(const char* psz);		// equivalent to findNonSpace(findSpace(psz))
	wchar_t* stepOver(const wchar_t* pwsz);

	inline	bool isAlpha(char ch) { return ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }
	inline	bool isDigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }
	inline	bool isEmpty(const char* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool isNonEmpty(const char* psz) { return (psz != nullptr && psz[0]); }
	inline	bool isPunct(char ch) { return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!'); }
	inline	bool isUTF8(char ch) { return ((ch & 0xC0) != 0x80); }	// is ch the start of a utf8 sequence?
	inline	bool isWhitespace(char ch) { return tt::findChar(" \t\r\n\f", ch) ? true : false; };

	inline	bool isAlpha(wchar_t ch) { return ( (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')); }
	inline	bool isDigit(wchar_t ch) { return ((ch >= L'0' && ch <= L'9') || ch == L'-'); }
	inline	bool isEmpty(const wchar_t* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
	inline	bool isNonEmpty(const wchar_t* psz) { return (psz != nullptr && psz[0]); }
	inline	bool isPunct(wchar_t ch) { return (ch == L'.' || ch == L',' || ch == L';' || ch == L':' || ch == L'?' || ch == L'!'); }
	inline	bool isWhitespace(wchar_t ch) { return (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'\f') ? true : false; }

	int			strCat_s(char* pszDst, size_t cbDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	int			strCopy_s(char* pszDst, size_t cbDest, const char* pszSrc);	// always zero-terminates, returns EOVERFLOW if truncated
	size_t		strLen(const char* psz);

	int			strCat_s(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc);
	int			strCopy_s(wchar_t* pwszDst, size_t cbDest, const wchar_t* pwszSrc);
	size_t		strLen(const wchar_t* pwsz);

	// force "normal" calls to secure version -- possible buffer overflow if destination isn't large enough

	inline char* strCat(char* pszDst, const char* pszSrc) { strCat_s(pszDst, tt::MAX_STRING_LEN, pszSrc); return pszDst; }
	inline char* strCopy(char* pszDst, const char* pszSrc) { strCopy_s(pszDst, tt::MAX_STRING_LEN, pszSrc); return pszDst; }

	inline void  strCat(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strCat_s(pwszDst, tt::MAX_STRING_LEN, pszSrc); }
	inline void  strCopy(wchar_t* pwszDst, const wchar_t* pszSrc) { tt::strCopy_s(pwszDst, tt::MAX_STRING_LEN, pszSrc); }

	// Use strLen() to get the number of characters without trailing zero, use strByteLen() to get the number of
	// bytes including the terminating zero

	inline size_t	strByteLen(const char* psz) { return tt::strLen(psz) * sizeof(char) + sizeof(char); }	// char is 1 in SBCS builds, 2 in UNICODE builds
	inline size_t	strByteLen(const wchar_t* pwsz) { return tt::strLen(pwsz) * sizeof(wchar_t) + sizeof(wchar_t); }

	void trimRight(char* psz);

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
	extern HWND hwndMsgBoxParent;		// parent for MessageBox--if Abort is requested in ttASSERT, this window will receive a WM_CLOSE message prior to shut down
	extern HINSTANCE hinstResources;	// handle to use to load resources
	extern const char* pszMsgTitle;		// title for message boxes
	extern size_t LanguageOffset;		// language offset used to load other languages from .rc file

	// InitCaller is equivalent to calling setResInst(hinstRes), setMsgBoxParent(hwndParent) and setMsgBoxTitle(pszMsgTitle)
	void		InitCaller(HINSTANCE hinstRes, HWND hwndParent, const char* pszMsgTitle);
	inline void	InitCaller(const char* pszTitle) { InitCaller(GetModuleHandle(nullptr), nullptr, pszTitle); }	// use this for console apps

	inline HINSTANCE getResInst() { return hinstResources; }
	inline void		 setResInst(HINSTANCE hinst) { hinstResources = hinst; }
	inline void		 setMsgBoxParent(HWND hwnd) { hwndMsgBoxParent = hwnd; }
		   void		 setMsgBoxTitle(const char* pszMsgTitle);
	inline const char* getMsgBoxTitle() { return pszMsgTitle; }
#endif

	void	ConvertToRelative(const char* pszRoot, const char* pszFile, ttCStr& cszResult);
	bool	CreateDir(const char* pszDir);
	bool	CreateDir(const wchar_t* pszDir);

	// The exists functions return true/false base on whether the file or directory actually exists
	bool	DirExists(const char* pszFolder);
	bool	DirExists(const wchar_t* pszFolder);
	bool	FileExists(const char* pszFile);
	bool	FileExists(const wchar_t* pszFile);

	bool	isValidFileChar(const char* psz, size_t pos);

	void	BackslashToForwardslash(char* pszPath);		// converts all backslashes in the string to forward slashes
	void	ForwardslashToBackslash(char* pszPath);

	void	AddTrailingSlash(char* pszPath);

	// findExtPortion and findFilePortion properly handle directories and filenames that start with '.' -- i.e., tt::findExtPortion(".gitignore") will return nullptr
	char*	findExtPortion(const char* pszPath);		// returns pointer to the '.' that begins a file name extension, or nullptr
	char*	findFilePortion(const char* pszPath);		// returns pointer to the filename portion of a path

	char*	 findExt(const char* pszPath, const char* pszExt);		// find a case-insensitive extension in a path string
	wchar_t* findExt(const wchar_t* pszPath, const wchar_t* pszExt);
	char* 	 findLastSlash(const char* pszPath);		// handles both forward and back slashes

	size_t	HashFromSz(const char* psz);
	size_t	HashFromSz(const wchar_t* psz);
	size_t	HashFromURL(const char* pszURL);
	size_t	HashFromURL(const wchar_t* pszURL);

	inline void	SetAssertHandlerA(TTASSERTHANDLERA pFunc) { pttAssertHandlerA = pFunc; }	// Replace the ttLib assertion handler with your own
	inline void	SetAssertHandlerW(TTASSERTHANDLERW pFunc) { pttAssertHandlerW = pFunc; }

#ifdef _WINDOWS_
	const char* getResString(size_t idString);
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

	inline bool isPosInRect(const RECT* prc, int xPos, int yPos) { return (xPos >= prc->left && xPos <= prc->right && yPos >= prc->top && yPos <= prc->bottom); }

	inline bool	isValidWindow(HWND hwnd) { return (bool) (hwnd && IsWindow(hwnd)); };
#endif	// _WINDOWS_

// Remove these after all KeyWorks projects compile cleanly without them

[[deprecated]]	inline char* strchr(const char* psz, char ch) { return findChar(psz, ch); }
[[deprecated]]	inline char* strchrR(const char* psz, char ch) { return findLastChar(psz, ch); }
[[deprecated]]	inline char* strstr(const char* pszMain, const char* pszSub) { return findStr(pszMain, pszSub); }
[[deprecated]]	inline char* stristr(const char* pszMain, const char* pszSub){ return findStri(pszMain, pszSub); }

[[deprecated]]	inline wchar_t* strchr(const wchar_t* psz, char ch) { return findChar(psz, ch); }
[[deprecated]]	inline wchar_t* strchrR(const wchar_t* psz, char ch) { return findLastChar(psz, ch); }
[[deprecated]]	inline wchar_t* strstr(const wchar_t* pszMain, const wchar_t* pszSub) { return findStr(pszMain, pszSub); }
[[deprecated]]	inline wchar_t* stristr(const wchar_t* pszMain, const wchar_t* pszSub){ return findStri(pszMain, pszSub); }

[[deprecated]]	inline char* FindFilePortion(const char* pszPath) { return tt::findFilePortion(pszPath); }
[[deprecated]]	inline char* FindExtPortion(const char* pszPath)  { return tt::findExtPortion(pszPath); }

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
