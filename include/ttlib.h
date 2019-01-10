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

#include "ttdebug.h"	// Various debugging functionality (ttASSERT, ttReportLastError, etc.)
#include "ttheap.h" 	// ttHeap class
#include "ttstr.h"		// various functions that work with strings

class ttString;			// forward definition

namespace tt {

#ifdef _WINDOWS_
	extern HWND hwndParent;	// parent for MessageBox, and if Abort is requested in ttASSERT, will receive a WM_CLOSE message prior to shut down
	extern HINSTANCE hinstResources;	// handle to use to load resources
	extern const char* pszMsgTitle;		// title for message boxes
	extern size_t LanguageOffset;		// language offset used to load other languages from .rc file

	void InitCaller(HINSTANCE hinstRes, HWND hwndParent, const char* pszMsgTitle);
	int MsgBox(const char* pszMsg, UINT uType);
	int MsgBox(UINT idResource, UINT uType);
	int __cdecl MsgBoxFmt(const char* pszFormat, UINT uType, ...);
	int __cdecl MsgBoxFmt(int idResource, UINT uType, ...);
	HFONT CreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold = false, bool fItalics = false);
	ptrdiff_t CompareFileTime(FILETIME* pftSrc, FILETIME* pftDst);
	const char* LoadTxtResource(int idRes, uint32_t* pcbFile, HINSTANCE hinst);
	const char* GetResString(size_t idString);
	void __cdecl KeyTrace(const char* pszFormat, ...);

	inline int RC_HEIGHT(const RECT* prc) { return prc->bottom - prc->top; };
	inline int RC_HEIGHT(const RECT rc) { return rc.bottom - rc.top; };
	inline int RC_WIDTH(const RECT* prc) { return prc->right - prc->left; };
	inline int RC_WIDTH(const RECT rc) { return rc.right - rc.left; };

	inline bool	IsValidWindow(HWND hwnd) { return (bool) (hwnd && IsWindow(hwnd)); };

#endif	// _WINDOWS_

	void		AddTrailingSlash(char* psz);
	void		BackslashToForwardslash(char* psz);
	void		ConvertToRelative(const char* pszRoot, const char* pszFile, ttString& cszResult);
	bool		CreateDir(const char* pszDir);
	bool		CreateDir(const wchar_t* pszDir);
	bool		DirExists(const char* pszFolder);
	bool		DirExists(const wchar_t* pszFolder);
	bool		FileExists(const char* pszFile);
	bool		FileExists(const wchar_t* pszFile);
	const char* FindExtPortion(const char* pszFile);
	const char* FindFilePortion(const char* pszFile);
	void		ForwardslashToBackslash(char* psz);
	size_t		HashFromSz(const char* psz);
	bool		IsValidFileChar(const char* psz, size_t pos);
	size_t		HashFromSz(const wchar_t* psz);
	size_t		HashFromURL(const char* pszURL);
	size_t		HashFromURL(const wchar_t* pszURL);

} // end of tt namespace

#endif	// __TTLIB_H__
