/////////////////////////////////////////////////////////////////////////////
// Name:      winmisc.cpp
// Purpose:   Miscellaneous functions for use on Windows
// Author:    Ralph Walden
// Copyright: Copyright (c) 1992-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// All the functions in this module will ONLY work on a Windows OS

#include "pch.h"  // precompiled header

#if defined(_WIN32)

#include "../include/ttcritsection.h"  // CCritSection, CCritLock
#include "../include/ttstr.h"          // ttCStr
#include "../include/ttdebug.h"        // ttASSERT macros
#include "../include/ttwstr.h"   // ttCWStr

#if defined(_WX_DEFS_H_)
#include <wx/msgdlg.h>  // wxMessageBox
#endif

namespace tt
{
    const char*    pszMsgTitle;   // utf8  title for message boxes
    const wchar_t* pwszMsgTitle;  // utf16 title for message boxes
}  // namespace tt

namespace tt
{
    HINSTANCE hinstResources;  // Used to determine where to load resources from. If nullptr, it will us
}

void ttInitCaller(HINSTANCE hinstRes, HWND /* hwnd */, const char* pszTitle)
{
    ttInitCaller(pszTitle);

    tt::hinstResources = hinstRes;  // in the off chance it's different
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
    return MessageBoxA(GetActiveWindow(), strRes.IsNonEmpty() ? (char*) strRes : "missing resource id",
                       (tt::pszMsgTitle ? tt::pszMsgTitle : ""), uType);
}

int __cdecl ttMsgBoxFmt(const char* pszFormat, UINT uType, ...)
{
    ttCStr  csz;
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

    ttCStr  csz;
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
    ZeroMemory(&lf, sizeof(LOGFONTA));

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

// The system API CompareFileTime() will say write access time was different if the files are only 2 seconds apart --
// which they can be on networked or FAT drives. We roll our own to account for this.

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

    return 0;  // Note that we do NOT check milliseconds
}

const char* ttLoadTxtResource(int idRes, uint32_t* pcbFile, HINSTANCE hinst)
{
    HRSRC hrsrc = FindResourceA(hinst, MAKEINTRESOURCEA(idRes), (char*) RT_RCDATA);
    if (!hrsrc)
        return nullptr;

    if (pcbFile)
        *pcbFile = SizeofResource(hinst, hrsrc);

    HGLOBAL hglb = LoadResource(hinst, hrsrc);
    if (!hglb)
        return nullptr;
    return (const char*) LockResource(hglb);  // This doesn't actually lock anything, it simply returns a pointer to the data
}

const char* ttGetResString(size_t idString)
{
    static char szStringBuf[1024];

    if (LoadStringA(tt::hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0)
    {
#ifdef _DEBUG
        ttCStr cszMsg;
        cszMsg.printf("Invalid string id: %u", (UINT) idString);
        ttFAIL(cszMsg);
#endif
        szStringBuf[0] = '\0';
    }
    return (const char*) szStringBuf;
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

    if (tt::pwszMsgTitle)
        ttFree((void*) tt::pwszMsgTitle);
    if (!pszTitle)
        tt::pwszMsgTitle = ttStrDup(L"");
    else
    {
        ttCWStr cwsz(pszTitle);
        tt::pwszMsgTitle = ttStrDup((wchar_t*) cwsz);
    }
}

#ifndef _INC_STDLIB
__declspec(noreturn) void __cdecl exit(int _Code);
#endif

__declspec(noreturn) void ttOOM(void)
{
#if defined(_DEBUG)
#if defined(_WIN32)
    int answer = MessageBoxA(GetActiveWindow(), "Out of Memory!!!", "Do you want to call DebugBreak()?",
                             MB_YESNO | MB_ICONERROR);

    if (answer == IDYES)
        DebugBreak();
#else
    int answer = wxMessageBox("Do you want to call DebugBreak()?", "Out of Memory!!!", wxYES_NO | wxICON_ERROR);
    if (answer == wxYES)
        wxTrap();
#endif
#endif  // _DEBUG

    // The advantage of exit() is functions registered by atexit() will be called, which might include deleting
    // temporary files, or other data that might otherwise persist after the program is exited.

    exit(-1);
}

#endif  // defined(_WIN32)
