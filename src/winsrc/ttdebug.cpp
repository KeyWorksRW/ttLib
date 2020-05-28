/////////////////////////////////////////////////////////////////////////////
// Name:      ttdebug.cpp
// Purpose:   Various debugging functionality
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"  // precompiled header

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <mutex>

#include <stdexcept>
#include <stdio.h>

#include "ttcstr.h"   // Classes for handling zero-terminated char strings.
#include "ttdebug.h"  // ttASSERT macros

// DO NOT CHANGE THESE TWO NAMES! Multiple applications expect these names and will no longer display trace messages
// if you change them.

const char* ttlib::txtTraceClass = "KeyViewMsgs";
const char* ttlib::txtTraceShareName = "hhw_share";
HWND ttlib::hwndTrace = NULL;

namespace ttdbg
{
    std::mutex mutexAssert;
    std::mutex mutexTrace;

    bool allowAsserts { true };  // Setting this to true will cause AssertionMsg to return without doing anything

    HANDLE hTraceMapping { NULL };
    char* g_pszTraceMap { nullptr };
    DWORD g_cLastTickCheck { 0 };  // used to determine whether to check for hwndTrace again
}  // namespace ttdbg

// Don't use std::string_view for msg -- we special-case a null pointer
// bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond, const char* msg)
bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond, const std::string& msg)
{
    if (!ttdbg::allowAsserts)
        return false;

    std::unique_lock<std::mutex> classLock(ttdbg::mutexAssert);

    ttlib::cstr fname(filename);
    fname.make_relative(std::filesystem::absolute(".").u8string());

    std::stringstream str;
    if (cond)
        str << "Expression: " << cond << "\n\n";
    if (!msg.empty())
        str << "Comment: " << msg << "\n\n";

    str << "File: " << fname << "\n";
    str << "Function: " << function << "\n";
    str << "Line: " << line << "\n\n";
    str << "Press Retry to break into a debugger.";

    // Converting to utf16 requires begin() and end() which stringstream doesn't support
    std::string copy(str.str());

    std::wstring str16;
    ttlib::utf8to16(copy, str16);
    auto answer = MessageBoxW(GetActiveWindow(), str16.c_str(), L"Assertion failed!", MB_ABORTRETRYIGNORE | MB_ICONSTOP);

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

void ttlib::wintrace(const std::string& msg, unsigned int type)
{
    // We don't want two threads trying to send text at the same time. The ttCCritLock class prevents a second call
    // to the function until the first call has completed.

    std::unique_lock<std::mutex> classLock(ttdbg::mutexTrace);

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
        ttdbg::hTraceMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, ttlib::txtTraceShareName);
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
        throw std::invalid_argument("wintrace msg must not exceed 4092 bytes");

    std::strcpy(ttdbg::g_pszTraceMap, msg.c_str());
    std::strcat(ttdbg::g_pszTraceMap, "\n");

    SendMessageW(ttlib::hwndTrace, type, 0, 0);

    UnmapViewOfFile(ttdbg::g_pszTraceMap);
    ttdbg::g_pszTraceMap = nullptr;
}

void ttlib::wintrace(unsigned int type)
{
    std::unique_lock<std::mutex> classLock(ttdbg::mutexTrace);

    if (!ttlib::hwndTrace)
        ttlib::hwndTrace = FindWindowA(ttlib::txtTraceClass, NULL);

    if (!ttlib::hwndTrace || !IsWindow(ttlib::hwndTrace))
        return;

    SendMessageW(ttlib::hwndTrace, type, 0, 0);
}
