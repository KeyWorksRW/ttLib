/////////////////////////////////////////////////////////////////////////////
// Name:		winmisc.cpp
// Purpose:		Miscellaneous functions for use on Windows
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1992-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// All the functions in this module will ONLY work on a Windows OS

#include "pch.h"		// precompiled header

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include "../include/ttcritsection.h"	// CCritSection, CCritLock
#include "../include/ttstring.h"		// ttCStr
#include "../include/ttstr.h"			// ttStr

namespace tt {
	const char* pszMsgTitle;
	HWND		hwndParent;
	HINSTANCE	hinstResources;		// Used to determine where to load resources from. If nullptr, it will us
	size_t		LanguageOffset;		// language offset used to load other languages from .rc file
}

void tt::InitCaller(HINSTANCE hinstRes, HWND hwnd, const char* pszTitle)
{
	tt::pszMsgTitle = tt::strdup(pszTitle ? pszTitle : "");

	tt::hinstResources = hinstRes;
	tt::hwndParent = hwnd;
}

// Note that these message boxes will work in a console app as well as a windowed app

int tt::MsgBox(const char* pszMsg, UINT uType)
{
	return MessageBoxA(GetActiveWindow(), pszMsg, (tt::pszMsgTitle ? tt::pszMsgTitle : ""), uType);
}

int tt::MsgBox(UINT idResource, UINT uType)
{
	ttStr strRes;
	strRes.GetResString(idResource);
	return MessageBoxA(GetActiveWindow(), strRes.isnonempty() ? (char*) strRes : "missing resource id", (tt::pszMsgTitle ? tt::pszMsgTitle : ""), uType);
}

int __cdecl tt::MsgBoxFmt(const char* pszFormat, UINT uType, ...)
{
	ttStr csz;
	va_list argList;
	va_start(argList, uType);
	tt::vprintf(&csz.m_psz, pszFormat, argList);
	va_end(argList);

	return MessageBoxA(GetActiveWindow(), csz, tt::pszMsgTitle ? tt::pszMsgTitle : "", uType);
}

int __cdecl tt::MsgBoxFmt(int idResource, UINT uType, ...)
{
	ttStr cszTmp;
	cszTmp.GetResString(idResource);

	ttStr csz;
	va_list argList;
	va_start(argList, uType);
	tt::vprintf(&csz.m_psz, cszTmp, argList);
	va_end(argList);

	return MessageBoxA(GetActiveWindow(), csz, tt::pszMsgTitle ? tt::pszMsgTitle : "", uType);
}

HFONT tt::CreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold, bool fItalics)
{
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
	tt::strcpy_s(lf.lfFaceName, LF_FACESIZE, pszTypeFace);

	HFONT hfont = CreateFontIndirectA(&lf);
	DeleteDC(hdc);

	return hfont;
}

// The system API CompareFileTime() will say write access time was different if the files are only 2 seconds apart -- which they can be on networked or FAT drives.
// We roll our own to account for this.

ptrdiff_t tt::CompareFileTime(FILETIME* pftSrc, FILETIME* pftDst)
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

const char* tt::LoadTxtResource(int idRes, uint32_t* pcbFile, HINSTANCE hinst)
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

const char* tt::GetResString(size_t idString)
{
	static char szStringBuf[1024];

	if (LoadStringA(tt::hinstResources, (UINT) (idString + tt::LanguageOffset), szStringBuf, (int) sizeof(szStringBuf)) == 0) {
		// Is the English resource available?
		if (tt::LanguageOffset && LoadStringA(tt::hinstResources, (UINT) idString, szStringBuf, sizeof(szStringBuf)) != 0) {
#ifdef _DEBUG
			ttCStr strMsg;
			strMsg.printf("Non-localized resource id: %zu", idString);
			ttFAIL(strMsg);
#endif
			return (const char*) szStringBuf;
		}
#ifdef _DEBUG
		ttCStr strMsg;
		strMsg.printf("Invalid string id: %zu", idString);
		ttFAIL(strMsg);
#endif
		szStringBuf[0] = '\0';
	}
	return (const char*) szStringBuf;
}

// KeyTrace can be used to send text messages that will be displayed by KeyView.exe (if it is running)
// See https://github.com/Randalphwa/KeyHelp/KeyView for details of how KeyView works

namespace {
	HANDLE hKeyViewMapping;
	HWND   hwndKeyView;
	ttCCritSection g_csKeyView;
	char*  g_pszKeyViewMap;		 // points to data in shared memory
}

#define WMP_GENERAL_MSG		 (WM_USER + 0x1f3)
#define WMP_KEY_TRACE_MSG	 (WM_USER + 0x1f5)
#define WMP_CLEAR_KEYVIEW	 (WM_USER + 0x1f9)	// clear the KeyView window

void __cdecl tt::KeyTrace(const char* pszFormat, ...)
{
	if (!pszFormat || !*pszFormat)
		return;

	if (!IsWindow(hwndKeyView)) {
		hwndKeyView = FindWindowA("KeyViewMsgs", NULL);
		if (!hwndKeyView)
			return;
	}

	ttStr csz;
	va_list argList;
	va_start(argList, pszFormat);
	tt::vprintf(&csz.m_psz, pszFormat, argList);
	va_end(argList);

	// We don't want two threads trying to send text at the same time, so we wrap the rest of this in a critical section

	ttCCritLock lock(&g_csKeyView);

	if (!hKeyViewMapping) {
		hKeyViewMapping = CreateFileMappingA((HANDLE) -1, NULL, PAGE_READWRITE, 0, 4096, "hhw_share");
		if (!hKeyViewMapping) {
			hwndKeyView = NULL;
			return;
		}
		g_pszKeyViewMap = (PSTR) MapViewOfFile(hKeyViewMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (!g_pszKeyViewMap) {
			hwndKeyView = NULL;
			return;
		}
	}

	ttASSERT(tt::strlen(csz) < 4094);

	if (tt::strlen(csz) >= 4094)
		csz.m_psz[4093] = 0;	// truncate to size KeyView can handle

	tt::strcpy_s(g_pszKeyViewMap, 4093, csz);

	SendMessage(hwndKeyView, WMP_KEY_TRACE_MSG, 0, 0);
}
