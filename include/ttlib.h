/////////////////////////////////////////////////////////////////////////////
// Name:		ttlib.h
// Purpose:		General Purpose library
// Author:		Ralph Walden
// Copyright:   Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTLIB_H__
#define __TTLIB_TTLIB_H__

#include <stdint.h>

__declspec(noreturn) void OOM(void);	// Called when memory allocation fails

#include "ttheap.h"		// CTTHeap -- need this for memory allocations
#include "asserts.h"	// Assertion handling
#include "kstr.h"		// various kstr...() functions

class CStr;	// forward definition

bool CreateDir(const char* pszDir);
bool CreateDir(const wchar_t* pszDir);
bool DirExists(const char* pszFolder);
bool DirExists(const wchar_t* pszFolder);
bool FileExists(const char* pszFile);
bool FileExists(const wchar_t* pszFile);

void	BackslashToForwardslash(char* psz);		// convert all back slashes in the string to forward slashes
void	ForwardslashToBackslash(char* psz);		// convert all forward slashes in the string to back slahes
void	AddTrailingSlash(char* psz);			// add a trailing forward slash if the string doesn't already end in a forward or back slash
char*	FindFilePortion(const char* pszFile);	// find the filename portion of a string
void 	ConvertToRelative(const char* pszRoot, const char* pszFile, CStr& cszResult);	// convert pszFile to a path relative to pszRoot

// IsValidFileChar() is used  to determine if character at current position is part of a filename versus a dir
// name (e.g., . is only valid if it is not preceeded or followed by another .)

bool IsValidFileChar(const char* psz, size_t pos);

inline bool IsAlpha(char ch) { return ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }
inline bool IsDigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }
inline bool IsPunct(char ch) { return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!'); }
inline bool IsWhiteSpace(char ch) { return kstrchr(" \t\r\n\f", ch) ? true : false; };	// Don't use strchr() or you'll get true if ch == 0

inline bool IsEmptyString(const char* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
inline bool IsNonEmptyString(const char* psz) { return (psz != nullptr && psz[0]); }

inline bool IsAlpha(wchar_t ch) { return ( (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')); }
inline bool IsDigit(wchar_t ch) { return ((ch >= L'0' && ch <= L'9') || ch == L'-'); }
inline bool IsPunct(wchar_t ch) { return (ch == L'.' || ch == L',' || ch == L';' || ch == L':' || ch == L'?' || ch == L'!'); }
inline bool IsWhiteSpace(wchar_t ch) { return (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'\f') ? true : false; }

inline bool IsEmptyString(const wchar_t* psz) { return (bool) ((psz == nullptr) || (!psz[0])); }
inline bool IsNonEmptyString(const wchar_t* psz) { return (psz != nullptr && psz[0]); }

char* FindNonSpace(const char* psz);
char* FindNextSpace(const char* psz);

bool IsSameSubString(const char* pszMain, const char* pszSub);		// is the sub string a case-insensitive match to the first part of the main string?
bool IsCSSameSubString(const char* pszMain, const char* pszSub);	// case-sensitive version
bool IsSameString(const char* psz1, const char* psz2);				// case-insensitive comparison -- use kstrcmp() if you need case-sensitive

bool IsSameSubString(const wchar_t* pszMain, const wchar_t* pszSub);	// is the sub string a case-insensitive match to the first part of the main string?
bool IsCSSameSubString(const wchar_t* pszMain, const wchar_t* pszSub);	// case-sensitive version
bool IsSameString(const wchar_t* psz1, const wchar_t* psz2);			// case-insensitive comparison -- use kstrcmp() if you need case-sensitive

// Following are identical to above but starting with "is" instead of "Is" (in case you don't remember which case to use)

inline bool isSameSubString(const char* pszMain, const char* pszSub) { return IsSameSubString(pszMain, pszSub); }
inline bool isCSSameString(const char* pszMain, const char* pszSub)  { return IsCSSameSubString(pszMain, pszSub); }
inline bool isSameString(const char* psz1, const char* psz2) { return IsSameString(psz1, psz2); }

inline bool isSameSubString(const wchar_t* pszMain, const wchar_t* pszSub)   { return IsSameSubString(pszMain, pszSub); }
inline bool isCSSameSubString(const wchar_t* pszMain, const wchar_t* pszSub) { return IsCSSameSubString(pszMain, pszSub); }
inline bool isSameString(const wchar_t* psz1, const wchar_t* psz2) { return IsSameString(psz1, psz2); }

// Globals

#ifdef _WINDOWS_
	extern HWND		 	_hwndParent;		// used by any function that needs a handle to the top-level parent window
	extern HINSTANCE 	_hinstResources;	// where resources are loaded from -- handle from LoadLibrary() if they are in a dll
	extern const char*	_pszMsgTitle;		// Default title for message boxes
	extern size_t		_s_iLanguageOffset; // language offset used to load other languages from .rc file

	const char* GetResString(size_t idString);	// set _s_iLanguageOffset beforehand if you want to offset to a different language
	inline int RECT_HEIGHT(const RECT* prc) { return prc->bottom - prc->top; };
	inline int RECT_HEIGHT(const RECT rc) { return rc.bottom - rc.top; };
	inline int RECT_WIDTH(const RECT* prc) { return prc->right - prc->left; };
	inline int RECT_WIDTH(const RECT rc) { return rc.right - rc.left; };
#endif	// _WINDOWS_

// Constants

#define CH_OPEN_PAREN		'('
#define CH_CLOSE_PAREN		')'
#define CH_COLON			':'
#define CH_SEMICOLON		';'
#define CH_START_QUOTE		'`'
#define CH_SQUOTE			'\''
#define CH_END_QUOTE		'\''
#define CH_QUOTE			'"'
#define CH_EQUAL			'='
#define CH_SPACE			' '
#define CH_COMMA			','
#define CH_LEFT_BRACKET		'['
#define CH_RIGHT_BRACKET	']'
#define CH_TAB				'\t'
#define CH_BACKSLASH		'\\'
#define CH_FORWARDSLASH		'/'

#define isutf(c) (((c) & 0xC0) != 0x80)	// is c the start of a utf8 sequence?

#ifndef max
	#define max(a, b)  (((a) > (b)) ? (a) : (b))
	#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

typedef size_t HASH;

ptrdiff_t	Atoi(const char* psz);
ptrdiff_t	Atoi(const wchar_t* psz);
int			GetCPUCount();
HASH		HashFromSz(const char* pszKey);
HASH		HashFromURL(const char* pszURL);
HASH		HashFromSz(const wchar_t* pszKey);
HASH		HashFromURL(const wchar_t* pszURL);
char*		Hextoa(size_t val, char* pszDst, bool bUpperCase);	// convert number into hexadecimal string
wchar_t*	Hextoa(size_t val, wchar_t* pszDst, bool bUpperCase);	// convert number into hexadecimal string
void		trim(char* psz);
void		AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst);	// inserts commas in a number (5432 becomes 5,432). Okay for pszDst == pszNum
void		AddCommasToNumber(wchar_t* pszNum, wchar_t* pszDst, size_t cbDst);

// Having both 32 and 64 bit versions of Utoa and Itoa means we can handle 64-bit integers in a 32-bit build,
// and 32-bit integers in a 64-bit build. It also means calling using "int" or "unsigned int" will go to the
// correct function no matter what the size of an int is (which varies according to OS). This also saves us
// from trying to figure out whether this is a 64-bit build (there is no standard that	works with all C++
// compilers to determine 64 bit builds)

char* Utoa(uint32_t val, char* pszDst, size_t cbDst);	// converts using base 10
char* Utoa(uint64_t val, char* pszDst, size_t cbDst);	// converts using base 10

wchar_t* Utoa(uint32_t val, wchar_t* pszDst, size_t cbDst);	// converts using base 10
wchar_t* Utoa(uint64_t val, wchar_t* pszDst, size_t cbDst);	// converts using base 10

char* Itoa(int32_t val, char* pszDst, size_t cbDst);	// converts using base 10
char* Itoa(int64_t val, char* pszDst, size_t cbDst);	// converts using base 10

wchar_t* Itoa(int32_t val, wchar_t* pszDst, size_t cbDst);	// converts using base 10
wchar_t* Itoa(int64_t val, wchar_t* pszDst, size_t cbDst);	// converts using base 10

#ifdef _WINDOWS_
 	void			InitCaller(HINSTANCE hinstCaller, HWND hwndParent, const char* pszMsgTitle);
	HFONT			CreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold = false, bool fItalics = false);
	const char* 	LoadTxtResource(int idRes, uint32_t* pcbFile = nullptr, HINSTANCE hinst = _hinstResources);

	// These Message Box functions will display a message using the active window of the calling thread (if any) as the parent . On Windows, they
	// work fine even in a console app. On a non-Windows build, you will need to link with wxWidgets, and you must have an event handler.

	int MsgBox(const char* pszMsg, UINT uType = MB_OK | MB_ICONWARNING);
	int	MsgBox(UINT idResource, UINT uType = MB_OK | MB_ICONWARNING);
	int __cdecl MsgBoxFmt(const char* pszFormat, UINT uType, ...);
	int __cdecl MsgBoxFmt(int idResource, UINT uType, ...);

	ptrdiff_t	kCompareFileTime(FILETIME* pftSrc, FILETIME* pftDst);	// More accurate than Windows API CompareFileTime()

	inline HWND	GetCurrentWindowHandle() { return GetActiveWindow(); }
	inline bool	IsValidWindow(HWND hwnd) { return (bool) (hwnd && IsWindow(hwnd)); };

	// The following are used for talking to KeyView.exe

	#define WMP_GENERAL_MSG		 (WM_USER + 0x1f3)
	#define WMP_KEY_EVENT_MSG	 (WM_USER + 0x1f4)
	#define WMP_KEY_TRACE_MSG	 (WM_USER + 0x1f5)
	#define WMP_KEY_PROPERTY_MSG (WM_USER + 0x1f6)
	#define WMP_KEY_SCRIPT_MSG	 (WM_USER + 0x1f7)
	#define WMP_KEY_ERROR_MSG	 (WM_USER + 0x1f8)
	#define WMP_CLEAR_KEYVIEW	 (WM_USER + 0x1f9)	// clear the KeyView window
	#define WMP_SET_TITLE   	 (WM_USER + 0x1fa)

	void __cdecl KeyTrace(const char* lpszFormat, ...); 	// send formatted string to KeyView window

	class CHourGlass	// changes cursor to an hourglass until destructor to indicate busy condition
	{
	public:
		CHourGlass() { m_hcur = SetCursor(LoadCursor(NULL, (LPCTSTR) IDC_WAIT)); }
		~CHourGlass() { SetCursor(m_hcur); }
		void Restore() { SetCursor(m_hcur); }

	protected:
		HCURSOR m_hcur;
	};

#else	// not _WINDOWS_

	void InitCaller(void* hinstCaller, void* hwndParent, const char* pszMsgTitle);

#endif	// _WINDOWS_

/*
  CMem and CTMem allow you to allocate memory that will automatically be freed when the class gets destroyed.

  if (some condition) {
	CMem szBuf(256);
	strcpy(szBuf, "text");
	strcat(szBuf, "more text");
	puts(szBuf);
  } // szBuf is freed because it went out of scope
*/

class CMem	// Header-only class
{
public:
	CMem(void) { pb = nullptr; }
	CMem(size_t size) { pb = (uint8_t*) kmalloc(size); }
	~CMem(void) {
		if (pb)
			kfree(pb);
		pb = nullptr;
	}
	void resize(size_t cb) {
		if (!pb)
			pb = (uint8_t*) kmalloc(cb);
		else {
			pb = (uint8_t*) krealloc(pb, cb);
		}
	}

	operator void*() { return (void*) pb; };
	operator const char*() { return (const char*) pb; };
	operator char*()  { return (char*) pb; };
	operator uint8_t*() { return pb; };

	uint8_t* pb;
};

template <typename T> class CTMem	// Header-only class
{
public:
	CTMem() { p = NULL; }
	CTMem(size_t size) { p = (T) kmalloc(size); }
	~CTMem() {
		if (p)
			kfree(p);
	}

	void resize(size_t cb) {
		if (!p)
			p = (T) kmalloc(cb);
		else
			p = (T) krealloc(p, cb);
	}
	size_t size() { return ksize(p); }

	operator T()	{ ASSERT(p); return p; };
	T operator->() { ASSERT(p); return p; };

	T p;
};

#endif	// __TTLIB_TTLIB_H__