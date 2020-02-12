/////////////////////////////////////////////////////////////////////////////
// Name:      ttdebug.cpp
// Purpose:   Various debugging functionality
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"  // precompiled header

#include <stdio.h>

#include "../include/ttdebug.h"        // ttASSERT macros
#include "../include/ttstring.h"       // ttString, ttCwd, ttStrVector
#include "../include/ttcritsection.h"  // CCritSection

// Note that include this file locks us into a Windows-only build
#include "../include/ttlibwin.h"

#include "../utf8/unchecked.h"

const UINT tt::WMP_TRACE_GENERAL = WM_USER + 0x1f3;
const UINT tt::WMP_TRACE_MSG = WM_USER + 0x1f5;
const UINT tt::WMP_CLEAR_TRACE = WM_USER + 0x1f9;  // clears the ttTrace window

// DO NOT CHANGE THESE TWO NAMES! Multiple applications expect these names and will no longer send trace messages
// if you change them.

const char* tt::txtTraceClass = "KeyViewMsgs";
const char* tt::txtTraceShareName = "hhw_share";
HWND        tt::hwndTrace = NULL;

namespace ttdbg
{
    ttCCritSection crtAssert;
    bool bNoAssert = false;  // Setting this to true will cause AssertionMsg to return without doing anything
    bool bNoRecurse = false;

    HANDLE         hTraceMapping = NULL;
    ttCCritSection g_csTrace;
    char*          g_pszTraceMap = nullptr;
    DWORD          g_cLastTickCheck = 0;  // used to determine whether to check for hwndTrace again
}  // namespace ttdbg

#if 0
// [randalphwa - 3/5/2019] We don't currently expose this, but if a caller needs access to these variables, they
// could copy this section into their own code to gain access. Alternatively, we could put it in ttdebug.h--I just
// don't think it's going to be used often enough, and I'd rather not have clutter in ttdebug.h that almost no one
// will use.

namespace ttdbg
{
    extern ttCCritSection crtAssert;
    extern bool bNoAssert;  // Setting this to true will cause AssertionMsg to return without doing anything
    extern bool bNoRecurse;

    extern HANDLE         hTraceMapping;
    extern HWND           hwndTrace;
    extern ttCCritSection g_csTrace;
    extern char*          g_pszTraceMap;
}  // namespace ttdbg
#endif

using namespace ttdbg;

namespace tt
{
    extern std::wstring MsgBoxTitle;
}

// Don't use std::string_view for msg -- we special-case a null pointer
bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond, const char* msg)
{
    if (ttdbg::bNoAssert)
        return false;

    crtAssert.Lock();

    ttString fname(filename);
    fname.make_relative(std::filesystem::absolute(".").u8string());

    std::stringstream str;
    if (cond)
        str << "Expression: " << cond << "\n\n";
    if (msg)
        str << "Comment: " << msg << "\n\n";

    str << "File: " << fname << "\n";
    str << "Function: " << function << "\n";
    str << "Line: " << line << "\n\n";
    str << "Press Retry to break into a debugger.";

    // Converting to utf16 requires begin() and end() which stringstream doesn't support
    std::string copy(str.str());

    std::wstring str16;
    utf8::unchecked::utf8to16(copy.begin(), copy.end(), back_inserter(str16));
    auto answer =
        MessageBoxW(GetActiveWindow(), str16.c_str(), L"Assertion failed!", MB_ABORTRETRYIGNORE | MB_ICONSTOP);

    crtAssert.Unlock();
    if (answer == IDRETRY)
    {
        return true;
    }
    else if (answer == IDABORT)
    {
        ExitProcess(static_cast<UINT>(-1));
    }

    return false;
}

void ttSetAsserts(bool bDisable)
{
    ttdbg::bNoAssert = bDisable;
}

bool ttdoReportLastError(const char* filename, const char* function, int line)
{
    char* pszMsg;

    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &pszMsg, 0, NULL);

    auto result = ttAssertionMsg(filename, function, line, nullptr, pszMsg);

    LocalFree((HLOCAL) pszMsg);
    return result;
}

int tt::CheckItemID(HWND hwnd, int id, const char* pszID, const char* filename, const char* function, int line)
{
    if (::GetDlgItem(hwnd, id) == NULL)
    {
        std::stringstream msg;
        msg << "Invalid dialog control id: " << pszID << " (" << id << ')';
        if (ttAssertionMsg(filename, function, line, pszID, msg.str().c_str()))
        {
            DebugBreak();
        }
    }
    return id;
}

// WARNING! Do not call ttASSERT in this function or you will end up with a recursive call.

void __cdecl ttTrace(const char* pszFormat, ...)
{
    // We don't want two threads trying to send text at the same time, so we wrap the function in a critical
    // section

    ttCCritLock lock(&g_csTrace);

    if (!pszFormat || !*pszFormat)
        return;

    if (!ttIsValidWindow(tt::hwndTrace))
    {
        // Trace could be called a lot, and we don't really want to be searching for the window constantly.

        DWORD cCurTick = GetTickCount();
        cCurTick /= 1000;  // convert to seconds

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

    ttCStr  csz;
    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(csz.GetPPtr(), pszFormat, argList);
    va_end(argList);

    if (!hTraceMapping)
    {
        hTraceMapping =
            CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, tt::txtTraceShareName);
        if (!hTraceMapping)
        {
            tt::hwndTrace = NULL;
            return;
        }
    }

    if (!ttdbg::g_pszTraceMap)
    {
        ttdbg::g_pszTraceMap = (char*) MapViewOfFile(hTraceMapping, FILE_MAP_WRITE, 0, 0, 0);
        if (!ttdbg::g_pszTraceMap)
        {
            tt::hwndTrace = NULL;
            return;
        }
    }

    ttStrCpy(ttdbg::g_pszTraceMap, 4092, csz.c_str());
    ttStrCat(ttdbg::g_pszTraceMap, 4094, "\n");

    SendMessageA(tt::hwndTrace, tt::WMP_TRACE_MSG, 0, 0);

    UnmapViewOfFile(ttdbg::g_pszTraceMap);
    ttdbg::g_pszTraceMap = nullptr;
}

void ttTraceClear()
{
    ttCCritLock lock(&g_csTrace);
    if (!ttIsValidWindow(tt::hwndTrace))
        return;
    SendMessageA(tt::hwndTrace, tt::WMP_CLEAR_TRACE, 0, 0);
}
