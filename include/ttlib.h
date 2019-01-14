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

class ttString;			// forward definition

namespace tt {
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
	#include "ttheap.h" 		// memory management
	#include "ttstr.h"			// string handling
	#include "ttstring.h"		// string class
	#include "ttlist.h" 		// class for handling lists of strings
	#include "ttfile.h" 		// class for reading and writing to files
#endif
