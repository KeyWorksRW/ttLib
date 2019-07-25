/////////////////////////////////////////////////////////////////////////////
// Name:      ttdebug.cpp
// Purpose:   Various debugging functionality
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"    // precompiled header

#include <stdio.h>

#include "../include/ttdebug.h"         // ttASSERT macros
#include "../include/ttstr.h"           // ttCStr
#include "../include/ttcritsection.h"   // CCritSection

bool (_cdecl *pttAssertHandlerA)(const char* pszMsg, const char* pszFile, const char* pszFunction, int line) = nullptr;
bool (_cdecl *pttAssertHandlerW)(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction, int line) = nullptr;

const UINT tt::WMP_TRACE_GENERAL = WM_USER + 0x1f3;
const UINT tt::WMP_TRACE_MSG   = WM_USER + 0x1f5;
const UINT tt::WMP_CLEAR_TRACE = WM_USER + 0x1f9;       // clears the ttTrace window

    // DO NOT CHANGE THESE TWO NAMES! Multiple applications expect these names and will no longer send trace messages if you change them.

const char* tt::txtTraceClass     = "KeyViewMsgs";
const char* tt::txtTraceShareName = "hhw_share";
HWND tt::hwndTrace = NULL;

namespace ttdbg
{
    ttCCritSection crtAssert;
    bool bNoAssert = false;     // Setting this to true will cause AssertionMsg to return without doing anything
    bool bNoRecurse = false;

    HANDLE hTraceMapping = NULL;
    ttCCritSection g_csTrace;
    char* g_pszTraceMap = nullptr;
    DWORD g_cLastTickCheck = 0;     // used to determine whether to check for hwndTrace again
}

#if 0
// [randalphwa - 3/5/2019] We don't currently expose this, but if a caller needs access to these variables, they could
// copy this section into their own code to gain access. Alternatively, we could put it in ttdebug.h--I just don't think
// it's going to be used often enough, and I'd rather not have clutter in ttdebug.h that almost no one will use.

namespace ttdbg
{
    extern ttCCritSection crtAssert;
    extern bool bNoAssert;      // Setting this to true will cause AssertionMsg to return without doing anything
    extern bool bNoRecurse;

    extern HANDLE hTraceMapping;
    extern HWND hwndTrace;
    extern ttCCritSection g_csTrace;
    extern char* g_pszTraceMap;
}
#endif

using namespace ttdbg;

// Displays a message box displaying the ASSERT with an option to ignore, break into a debugger, or exit the program

bool ttAssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line)
{
    if (ttdbg::bNoAssert)
        return false;

    // if pttAssertHandlerA calls us, then we need to use a normal assert message

    if (pttAssertHandlerA && !bNoRecurse)
    {
        bNoRecurse = true;
        bool bResult = pttAssertHandlerA(pszMsg, pszFile, pszFunction, line);
        bNoRecurse = false;
        return bResult;
    }

    crtAssert.Lock();

    // We special case a null or empty pszMsg -- ttASSERT_NONEMPTY(ptr) takes advantage of this

    if (!pszMsg)
        pszMsg = "NULL pointer!";
    else if (!*pszMsg)
        pszMsg = "Empty string!";

    char szBuf[2048];
    sprintf_s(szBuf, sizeof(szBuf), "%s\r\n\r\n%s (%s): line %u", pszMsg, pszFile, pszFunction, line);

#if defined(_WIN32)

    // wxWidgets does not have an equivalent of MB_ABORTRETRYIGNORE, nor will it work properly in a CONSOLE
    // application, so we just call directly into the Windws API.

    int answer = MessageBoxA(GetActiveWindow(), szBuf, "Retry to call DebugBreak()", MB_ABORTRETRYIGNORE);

    if (answer == IDRETRY)
    {
        DebugBreak();
        crtAssert.Unlock();
        return false;
    }
    else if (answer == IDIGNORE)
    {
        crtAssert.Unlock();
        return false;
    }

    crtAssert.Unlock();
    ExitProcess((UINT) -1);

#else    // not defined(_WIN32)
    int answer = wxMessageBox(szBuf, "Click Yes to break into debugger", wxYES_NO | wxICON_ERROR);
    if (answer == wxYES)
        wxTrap();

    crtAssert.Unlock();
    return false;
#endif    // defined(_WIN32)
}

bool ttAssertionMsg(const wchar_t* pwszMsg, const char* pszFile, const char* pszFunction, int line)
{
    if (ttdbg::bNoAssert)
        return false;

    if (pttAssertHandlerW)
        return pttAssertHandlerW(pwszMsg, pszFile, pszFunction, line);

    crtAssert.Lock();
    bool bResult;
    {   // use a brace so that cszMsg gets deleted before we release the critical section
        ttCStr cszMsg;

        // We special case a null or empty pszMsg -- ttASSERT_NONEMPTY(ptr) takes advantage of this

        if (!pwszMsg)
            cszMsg = "NULL pointer!";
        else if (!*pwszMsg)
            cszMsg = "Empty string!";
        else
            cszMsg = pwszMsg;

        bResult = ttAssertionMsg((char*) cszMsg, pszFile, pszFunction, line);
    }
    crtAssert.Unlock();
    return bResult;
}

void ttSetAsserts(bool bDisable)
{
    ttdbg::bNoAssert = bDisable;
}

#if defined(_WIN32)

void ttdoReportLastError(const char* pszFile, const char* pszFunc, int line)
{
    char* pszMsg;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &pszMsg, 0, NULL);

    ttAssertionMsg(pszMsg, pszFile, pszFunc, line);

    LocalFree((HLOCAL) pszMsg);
}

DWORD tt::CheckItemID(HWND hwnd, int id, const char* pszID, const char* pszFile, const char* pszFunc, int line)
{
    if (::GetDlgItem(hwnd, id) == NULL)
    {
        ttCStr cszMsg;
        cszMsg.printf("Invalid dialog control id: %s (%u)", pszID, id);
        ttAssertionMsg(cszMsg, pszFile, pszFunc, line);
    }
    return id;
}

// WARNING! Do not call ttASSERT in this function or you will end up with a recursive call.

void __cdecl ttTrace(const char* pszFormat, ...)
{
    // We don't want two threads trying to send text at the same time, so we wrap the function in a critical section

    ttCCritLock lock(&g_csTrace);

    if (!pszFormat || !*pszFormat)
        return;

    if (!ttIsValidWindow(tt::hwndTrace))
    {
        // Trace could be called a lot, and we don't really want to be searching for the window constantly.

        DWORD cCurTick = GetTickCount();
        cCurTick /= 1000;   // convert to seconds

        if (g_cLastTickCheck == 0 || cCurTick > g_cLastTickCheck + 5)
        {
            tt::hwndTrace = FindWindowA(tt::txtTraceClass, NULL);
            if (!tt::hwndTrace)
            {
                g_cLastTickCheck = cCurTick;
                return;
            }
        }
    }

    ttCStr csz;
    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(csz.GetPPtr(), pszFormat, argList);
    va_end(argList);

    if (!hTraceMapping)
    {
        hTraceMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, tt::txtTraceShareName);
        if (!hTraceMapping)
        {
            tt::hwndTrace = NULL;
            return;
        }
    }
    if (!g_pszTraceMap)
    {
        g_pszTraceMap = (char*) MapViewOfFile(hTraceMapping, FILE_MAP_WRITE, 0, 0, 0);
        if (!g_pszTraceMap)
        {
            tt::hwndTrace = NULL;
            return;
        }
    }

    ttStrCpy(g_pszTraceMap, 4093, csz);
    ttStrCat(g_pszTraceMap, 4094, "\r\n");

    SendMessageA(tt::hwndTrace, tt::WMP_TRACE_MSG, 0, 0);

    UnmapViewOfFile(g_pszTraceMap);
    g_pszTraceMap = nullptr;
}

void ttTraceClear()
{
    ttCCritLock lock(&g_csTrace);
    if (!ttIsValidWindow(tt::hwndTrace))
        return;
    SendMessageA(tt::hwndTrace, tt::WMP_CLEAR_TRACE, 0, 0);
}

#endif    // defined(_WIN32)
