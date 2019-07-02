/////////////////////////////////////////////////////////////////////////////
// Name:      winmisc.cpp
// Purpose:   Miscellaneous functions for use on Windows
// Author:    Ralph Walden
// Copyright: Copyright (c) 1992-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// All the functions in this module will ONLY work on a Windows OS

#include "pch.h"        // precompiled header

#ifndef _WINDOWS_
    #error This code will only work on Windows
#endif

#include "../include/ttcritsection.h"   // CCritSection, CCritLock
#include "../include/ttstr.h"       // ttCStr
#include "../include/ttdebug.h"         // ttASSERT macros

namespace tt {
    const char* pszMsgTitle;
    HWND        hwndMsgBoxParent;
    HINSTANCE   hinstResources;     // Used to determine where to load resources from. If nullptr, it will us
    size_t      LanguageOffset;     // language offset used to load other languages from .rc file
}

void ttInitCaller(HINSTANCE hinstRes, HWND hwnd, const char* pszTitle)
{
    tt::pszMsgTitle = ttStrDup(pszTitle ? pszTitle : "");

    tt::hinstResources = hinstRes;
    tt::hwndMsgBoxParent = hwnd;
}

void ttInitCaller(const char* pszTitle)
{
#if defined(_WIN32)
    tt::hinstResources = GetModuleHandle(NULL);
#endif
     ttSetMsgBoxTitle(pszTitle);
}

void ttSetMsgBoxTitle(const char* pszTitle)
{
    if (tt::pszMsgTitle)
        ttFree((void*) tt::pszMsgTitle);
    tt::pszMsgTitle = ttStrDup(pszTitle ? pszTitle : "");
}

// Note that these message boxes will work in a console app as well as a windowed app

int ttMsgBox(const char* pszMsg, UINT uType)
{
    return MessageBoxA(GetActiveWindow(), pszMsg, (tt::pszMsgTitle ? tt::pszMsgTitle : ""), uType);
}

int ttMsgBox(UINT idResource, UINT uType)
{
    ttCStr strRes;
    strRes.GetResString(idResource);
    return MessageBoxA(GetActiveWindow(), strRes.IsNonEmpty() ? (char*) strRes : "missing resource id", (tt::pszMsgTitle ? tt::pszMsgTitle : ""), uType);
}

int __cdecl ttMsgBoxFmt(const char* pszFormat, UINT uType, ...)
{
    ttCStr csz;
    va_list argList;
    va_start(argList, uType);
    ttVPrintf(csz.GetPPtr(), pszFormat, argList);
    va_end(argList);

    return MessageBoxA(GetActiveWindow(), csz, tt::pszMsgTitle ? tt::pszMsgTitle : "", uType);
}

int __cdecl ttMsgBoxFmt(int idResource, UINT uType, ...)
{
    ttCStr cszTmp;
    cszTmp.GetResString(idResource);

    ttCStr csz;
    va_list argList;
    va_start(argList, uType);
    ttVPrintf(csz.GetPPtr(), cszTmp, argList);
    va_end(argList);

    return MessageBoxA(GetActiveWindow(), csz, tt::pszMsgTitle ? tt::pszMsgTitle : "", uType);
}

HFONT ttCreateLogFont(const char* pszTypeFace, size_t cPt, bool fBold, bool fItalics)
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
    ttStrCpy(lf.lfFaceName, LF_FACESIZE, pszTypeFace);

    HFONT hfont = CreateFontIndirectA(&lf);
    DeleteDC(hdc);

    return hfont;
}

// The system API CompareFileTime() will say write access time was different if the files are only 2 seconds apart -- which they can be on networked or FAT drives.
// We roll our own to account for this.

ptrdiff_t ttCompareFileTime(FILETIME* pftSrc, FILETIME* pftDst)
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

    return 0;   // Note that we do NOT check milliseconds
}

const char* ttLoadTxtResource(int idRes, uint32_t* pcbFile, HINSTANCE hinst)
{
    HRSRC hrsrc  = FindResource(hinst, MAKEINTRESOURCE(idRes), RT_RCDATA);
    if (!hrsrc)
        return nullptr;

    if (pcbFile)
        *pcbFile = SizeofResource(hinst, hrsrc);

    HGLOBAL hglb = LoadResource(hinst, hrsrc);
    if (!hglb)
        return nullptr;
    return (const char*) LockResource(hglb);    // This doesn't actually lock anything, it simply returns a pointer to the data
}

/*

    Multiple language strings can be stored in the .rc file, and a language offset can be used to display them. For example:

    Header file:

        #define FRENCH_S_OFFSET 4096

        #define IDS_ENG_HELLO   1024
        #define IDS_FRN_HELLO   IDS_ENG_HELLO + FRENCH_S_OFFSET

    Resource file:

        IDS_ENG_HELLO "Hello"
        IDS_FRN_HELLO "Bonjour"

*/

const char* ttGetResString(size_t idString)
{
    static char szStringBuf[1024];

    if (LoadStringA(tt::hinstResources, (UINT) (idString + tt::LanguageOffset), szStringBuf, (int) sizeof(szStringBuf)) == 0)
    {
        // Is the English resource available?
        if (tt::LanguageOffset && LoadStringA(tt::hinstResources, (UINT) idString, szStringBuf, sizeof(szStringBuf)) != 0)
        {
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
