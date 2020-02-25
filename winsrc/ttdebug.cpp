/////////////////////////////////////////////////////////////////////////////
// Name:      ttdebug.cpp
// Purpose:   Various debugging functionality
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"  // precompiled header

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#if !defined(TTALL_LIB)
    #if defined(NDEBUG)
        #pragma comment(lib, "ttLibwin.lib")
    #else
        #pragma comment(lib, "ttLibwinD.lib")
    #endif
#endif

#include <stdio.h>
#include <stdexcept>

#include "../include/ttcritsection.h"  // CCritSection
#include "../include/ttcstr.h"         // Classes for handling zero-terminated char strings.
#include "../include/ttdebug.h"        // ttASSERT macros
#include "../include/utf8unchecked.h"  // provide UTF conversions

const UINT ttlib::WMP_TRACE_GENERAL = WM_USER + 0x1f3;
const UINT ttlib::WMP_TRACE_MSG = WM_USER + 0x1f5;
const UINT ttlib::WMP_CLEAR_TRACE = WM_USER + 0x1f9;  // clears the ttTrace window

// DO NOT CHANGE THESE TWO NAMES! Multiple applications expect these names and will no longer send trace messages
// if you change them.

const char* ttlib::txtTraceClass = "KeyViewMsgs";
const char* ttlib::txtTraceShareName = "hhw_share";
HWND ttlib::hwndTrace = NULL;

namespace ttdbg
{
    ttCCritSection crtAssert;
    bool allowAsserts{ true };  // Setting this to true will cause AssertionMsg to return without doing anything

    HANDLE hTraceMapping{ NULL };
    ttCCritSection g_csTrace;
    char* g_pszTraceMap{ nullptr };
    DWORD g_cLastTickCheck{ 0 };  // used to determine whether to check for hwndTrace again
}  // namespace ttdbg

// Don't use std::string_view for msg -- we special-case a null pointer
bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond, const char* msg)
{
    if (!ttdbg::allowAsserts)
        return false;

    ttdbg::crtAssert.Lock();

    ttlib::cstr fname(filename);
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

    ttdbg::crtAssert.Unlock();
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

void ttlib::allow_asserts(bool allowAsserts)
{
    ttdbg::allowAsserts = allowAsserts;
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

int ttlib::CheckItemID(HWND hwnd, int id, const char* pszID, const char* filename, const char* function, int line)
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

void ttlib::wintrace(ttlib::cview msg)
{
    // We don't want two threads trying to send text at the same time, so we wrap the function in a critical
    // section

    ttCCritLock lock(&ttdbg::g_csTrace);

    if (msg.empty())
        return;

    if (!(ttlib::hwndTrace && IsWindow(ttlib::hwndTrace)))
    {
        // Trace could be called a lot, and we don't really want to be searching for the window constantly.

        DWORD cCurTick = GetTickCount();
        cCurTick /= 1000;  // convert to seconds

        if (ttdbg::g_cLastTickCheck == 0 || cCurTick > ttdbg::g_cLastTickCheck + 5)
        {
            ttlib::hwndTrace = FindWindowA(ttlib::txtTraceClass, NULL);
            if (!ttlib::hwndTrace)
            {
                ttdbg::g_cLastTickCheck = cCurTick;
                return;
            }
        }
    }

    if (!ttdbg::hTraceMapping)
    {
        ttdbg::hTraceMapping =
            CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, ttlib::txtTraceShareName);
        if (!ttdbg::hTraceMapping)
        {
            ttlib::hwndTrace = NULL;
            return;
        }
    }

    if (!ttdbg::g_pszTraceMap)
    {
        ttdbg::g_pszTraceMap = (char*) MapViewOfFile(ttdbg::hTraceMapping, FILE_MAP_WRITE, 0, 0, 0);
        if (!ttdbg::g_pszTraceMap)
        {
            ttlib::hwndTrace = NULL;
            return;
        }
    }

    if (msg.size() > 4092)
        throw std::invalid_argument("msg must not exceed 4092 bytes");

    std::strcpy(ttdbg::g_pszTraceMap, msg.c_str());
    std::strcat(ttdbg::g_pszTraceMap, "\n");

    SendMessageA(ttlib::hwndTrace, ttlib::WMP_TRACE_MSG, 0, 0);

    UnmapViewOfFile(ttdbg::g_pszTraceMap);
    ttdbg::g_pszTraceMap = nullptr;
}

void ttlib::wintraceclear()
{
    ttCCritLock lock(&ttdbg::g_csTrace);
    if (!(ttlib::hwndTrace && IsWindow(ttlib::hwndTrace)))
        return;
    SendMessageA(ttlib::hwndTrace, ttlib::WMP_CLEAR_TRACE, 0, 0);
}
