/////////////////////////////////////////////////////////////////////////////
// Name:		winmisc.cpp
// Purpose:		Miscellaneous functions for use on Windows
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// All the functions in this module will ONLY work on a Windows OS

#include "precomp.h"		// precompiled header

#include "../include/critsection.h"	// CCritSection, CCritLock
#include "../include/cstr.h"		// CStr

const char* _pszMsgTitle;

#ifdef _WINDOWS_

HWND		_hwndParent;
HINSTANCE	_hinstResources;		// Used to determine where to load resources from. If nullptr, it will us
size_t		_s_iLanguageOffset;		// language offset used to load other languages from .rc file

void InitCaller(HINSTANCE hinstResources, HWND hwndParent, const char* pszMsgTitle)
{
	_pszMsgTitle = kstrdup(pszMsgTitle ? pszMsgTitle : "");

	_hinstResources = hinstResources;
	_hwndParent = hwndParent;
}

// Unlike wxMessageDialog, these work in a console app

int MsgBox(const char* pszMsg, UINT uType)
{
	return MessageBoxA(GetActiveWindow(), pszMsg, (_pszMsgTitle ? _pszMsgTitle : ""), uType);
}

int MsgBox(UINT idResource, UINT uType)
{
	CStr csz, cszRes;
	cszRes.GetResString(idResource);
	return MessageBoxA(GetActiveWindow(), cszRes.IsNonEmpty() ? (char*) cszRes : "missing resource id", (_pszMsgTitle ? _pszMsgTitle : ""), uType);
}

int __cdecl MsgBoxFmt(const char* pszFormat, UINT uType, ...)
{
	va_list argList;
	va_start(argList, uType);
	CStr csz;
	csz.vprintf(pszFormat, argList);
	va_end(argList);
	return MessageBoxA(GetActiveWindow(), (char*) csz, _pszMsgTitle ? _pszMsgTitle : "", uType);
}

int __cdecl MsgBoxFmt(int idResource, UINT uType, ...)
{
	va_list argList;
	va_start(argList, uType);
	CStr cszFmt;
	cszFmt.GetResString(idResource);
	CStr csz;
	csz.vprintf(cszFmt, argList);
	va_end(argList);
	return MessageBoxA(GetActiveWindow(), (char*) csz, _pszMsgTitle ? _pszMsgTitle : "", uType);
}

HFONT CreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold, bool fItalics)
{
#ifdef _WINDOWS_
	HDC hdc = CreateCompatibleDC(NULL);
	SetMapMode(hdc, MM_TEXT);

	LOGFONTA lf;
	ZeroMemory(&lf, sizeof(LOGFONT));

	int ratio = MulDiv(GetDeviceCaps(hdc, LOGPIXELSY), 100, 72);
	lf.lfHeight = MulDiv((int) cPt, ratio, 100);
	if ((cPt * ratio) % 100 >= 50)
		lf.lfHeight++;

	lf.lfHeight = -lf.lfHeight;
	lf.lfItalic = (BYTE) fItalics;
	if (fBold)
		lf.lfWeight = FW_BOLD;
	kstrcpy(lf.lfFaceName, LF_FACESIZE, pszTypeFace);

	HFONT hfont = CreateFontIndirectA(&lf);
	DeleteDC(hdc);
#else	// not _WINDOWS_
	#error Need to implement wxWidgets version
#endif	// _WINDOWS_


	return hfont;
}

// The system API CompareFileTime() will say write access time was different if the files are only 2 seconds apart -- which they can be on networked or FAT drives.
// We roll our own to account for this.

ptrdiff_t kCompareFileTime(FILETIME* pftSrc, FILETIME* pftDst)
{
	SYSTEMTIME stSrc, stDst;
	FileTimeToSystemTime(pftSrc, &stSrc);
	FileTimeToSystemTime(pftDst, &stDst);

	if (stSrc.wYear < stDst.wYear)
		return -1;
	else if (stSrc.wYear > stDst.wYear)
		return 1;

	if (stSrc.wMonth < stDst.wMonth)
		return -1;
	else if (stSrc.wMonth > stDst.wMonth)
		return 1;

	if (stSrc.wDay < stDst.wDay)
		return -1;
	else if (stSrc.wDay > stDst.wDay)
		return 1;

	if (stSrc.wHour < stDst.wHour)
		return -1;
	else if (stSrc.wHour > stDst.wHour)
		return 1;

	if (stSrc.wMinute < stDst.wMinute)
		return -1;
	else if (stSrc.wMinute > stDst.wMinute)
		return 1;

	int diff = (int) stSrc.wSecond - (int) stDst.wSecond;

	if (diff < -2)
		return -1;
	else if (diff > 2)
		return 1;

	return 0;	// Note that we do NOT check milliseconds
}

const char* LoadTxtResource(int idRes, uint32_t* pcbFile, HINSTANCE hinst)
{
	HRSRC hrsrc	 = FindResource(hinst, MAKEINTRESOURCE(idRes), RT_RCDATA);
	if (!hrsrc)
		return nullptr;

	if (pcbFile)
		*pcbFile = SizeofResource(hinst, hrsrc);

	HGLOBAL hglb = LoadResource(hinst, hrsrc);
	if (!hglb)
		return nullptr;
	return (const char*) LockResource(hglb);	// This doesn't actually lock anything, it simply returns a pointer to the data
}

/*

	Multiple language strings can be stored in the .rc file, and a language offset can be used to display them. For example:

	Header file:

		#define FRENCH_S_OFFSET 4096

		#define IDS_ENG_HELLO	1024
		#define IDS_FRN_HELLO	IDS_ENG_HELLO + FRENCH_S_OFFSET

	Resource file:

		IDS_ENG_HELLO "Hello"
		IDS_FRN_HELLO "Bonjour"

*/

const char* GetResString(size_t idString)
{
	static char szStringBuf[1024];

	if (LoadStringA(_hinstResources, (UINT) (idString + _s_iLanguageOffset), szStringBuf, (int) sizeof(szStringBuf)) == 0) {
		// Is the English resource available?
		if (_s_iLanguageOffset && LoadStringA(_hinstResources, (UINT) idString, szStringBuf, sizeof(szStringBuf)) != 0) {
#ifdef _DEBUG
			CStr cszMsg;
			cszMsg.printf("Non-localized resource id: %zu", idString);
			FAIL(cszMsg);
#endif
			return (const char*) szStringBuf;
		}
#ifdef _DEBUG
		CStr cszMsg;
		cszMsg.printf("Invalid string id: %zu", idString);
		FAIL(cszMsg);
#endif
		szStringBuf[0] = '\0';
	}
	return (const char*) szStringBuf;
}

// KeyTrace can be used to send text messages that will be displayed by KeyView.exe (if it is running)
// See https://github.com/Randalphwa/KeyHelp/KeyView for details of how KeyView works

HANDLE g_hKeyViewMapping;
HWND   g_hwndKeyView;
CCritSection g_csKeyView;
char*  g_pszKeyViewMap;		 // points to data in shared memory

void __cdecl KeyTrace(const char* pszFormat, ...)
{
	if (!pszFormat || !*pszFormat)
		return;

	if (!IsWindow(g_hwndKeyView)) {
		g_hwndKeyView = FindWindowA("KeyViewMsgs", NULL);
		if (!g_hwndKeyView)
			return;
	}

	va_list args;
	va_start(args, pszFormat);

	// CStr csz("KeyTrace: ");
	CStr csz;
	csz.vprintf(pszFormat, args);
	// csz += "\r\n";

	// We don't want two threads trying to send text at the same time, so we wrap the rest of this in a critical section

	CCritLock lock(&g_csKeyView);

	if (!g_hKeyViewMapping) {
		g_hKeyViewMapping = CreateFileMappingA((HANDLE) -1, NULL, PAGE_READWRITE, 0, 4096, "hhw_share");
		if (!g_hKeyViewMapping) {
			g_hwndKeyView = NULL;
			return;
		}
		g_pszKeyViewMap = (PSTR) MapViewOfFile(g_hKeyViewMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (!g_pszKeyViewMap) {
			g_hwndKeyView = NULL;
			return;
		}
	}

	ASSERT(kstrlen(csz) < 4094);

	if (kstrlen(csz) >= 4094)
		csz.getptr()[4093] = 0;	// truncate to size KeyView can handle

	kstrcpy(g_pszKeyViewMap, 4093, csz);

	SendMessage(g_hwndKeyView, WMP_KEY_TRACE_MSG, 0, 0);
}

#else	// not _WINDOWS_

////////////////////////////////////////////////////// non-Windows section //////////////////////////////////////////////////////

void InitCaller(void* /* hinstCaller */, void* /* hwndParent */, const char* pszMsgTitle)
{
	_pszMsgTitle = kstrdup(pszMsgTitle ? pszMsgTitle : "");
}


#endif // _WINDOWS_