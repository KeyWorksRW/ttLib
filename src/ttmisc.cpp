/////////////////////////////////////////////////////////////////////////////
// Name:      misc.cpp
// Purpose:   miscellaneous functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttdebug.h"  // for ttASSERTS
#include "../include/ttstr.h"    // ttCStr
#include "../include/ttwstr.h"   // ttCWStr

#if defined(_WX_DEFS_H_)
#include <wx/msgdlg.h>  // wxMessageBox
#endif

namespace tt
{
    const char*    pszMsgTitle;   // utf8  title for message boxes
    const wchar_t* pwszMsgTitle;  // utf16 title for message boxes
}  // namespace tt

size_t ttHashFromSz(const char* psz)
{
    ttASSERT_NONEMPTY(psz);

    if (!psz || !*psz)
        return 0;

    // djb2 hash algorithm

    size_t hash = 5381;

    while (*psz)
        hash = ((hash << 5) + hash) ^ (size_t) *psz++;

    return hash;
}

size_t ttHashFromSz(const wchar_t* psz)
{
    ttASSERT_NONEMPTY(psz);

    if (!psz || !*psz)
        return 5381;

    size_t hash = 5381;

    while (*psz)
        hash = ((hash << 5) + hash) + (size_t) *psz++;

    return hash;
}

// Unlike a "regular" hash, this version treats forward and backslashes identically, as well as upper and lower-case letters

size_t ttHashFromURL(const char* pszURL)
{
    ttCStr csz(pszURL);
    ttBackslashToForwardslash(csz);
    csz.MakeLower();
    return ttHashFromSz(csz);
}

size_t ttHashFromURL(const wchar_t* pszURL)
{
    ttCStr csz(pszURL);
    ttBackslashToForwardslash(csz);
    csz.MakeLower();
    return ttHashFromSz(csz);
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
    int answer = MessageBoxA(GetActiveWindow(), "Out of Memory!!!", "Do you want to call DebugBreak()?", MB_YESNO | MB_ICONERROR);

    if (answer == IDYES)
        DebugBreak();
#else
    int answer = wxMessageBox("Do you want to call DebugBreak()?", "Out of Memory!!!", wxYES_NO | wxICON_ERROR);
    if (answer == wxYES)
        wxTrap();
#endif
#endif  // _DEBUG

    // The advantage of exit() is functions registered by atexit() will be called, which might include deleting temporary
    // files, or other data that might otherwise persist after the program is exited.

    exit(-1);
}
