/////////////////////////////////////////////////////////////////////////////
// Name:      ttdebug.h
// Purpose:   ttASSERT macros
// Author:    Ralph Walden
// Copyright: Copyright (c) 2000-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Under a _DEBUG build, these ASSERTS will display a message box giving you the
// option to ignore the assert, break into a debugger, or exit the program.

#pragma once

#ifndef __TTLIB_ASSERTS_H__
#define __TTLIB_ASSERTS_H__

// Note that all functions listed here are available in release builds. Only the macros are removed in release builds

void cdecl ttTrace(const char* pszFormat, ...);  // formats a string and displays it in a ttTrace window (if ttTrace is running)
void       ttTraceClear();                       // clears the ttTrace window if ttTrace is running
bool       ttAssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);
bool       ttAssertionMsg(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction, int line);
void       ttdoReportLastError(const char* pszFile, const char* pszFunc, int line);
void       ttSetAsserts(bool bDisable);  // enables disables all assertion messages

__declspec(noreturn) void ttOOM(void);

namespace tt
{
#if defined(_WIN32)
    extern HWND hwndTrace;  // handle to ttTrace main window (if it was running when ttTrace was called;

    extern const UINT WMP_TRACE_GENERAL;  // WM_USER + 0x1f3;    // general message to send to ttTrace
    extern const UINT WMP_TRACE_MSG;      // WM_USER + 0x1f5;    // trace message to send to ttTrace
    extern const UINT WMP_CLEAR_TRACE;    // WM_USER + 0x1f9;    // clears the ttTrace window

    extern const char* txtTraceClass;      // class name of window to send trace messages to
    extern const char* txtTraceShareName;  // name of shared memory to write to

    int CheckItemID(HWND hwnd, int id, const char* pszID, const char* pszFile, const char* pszFunc, int line);
#endif  // defined(_WIN32)
}  // namespace tt

// clang-format off
#ifdef _DEBUG
    #define ttASSERT(exp) (void)((!!(exp)) || ttAssertionMsg(#exp, __FILE__, __func__, __LINE__))
    #define ttASSERT_MSG(exp, pszMsg) { if (!(exp)) ttAssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
    #define ttFAIL(pszMsg) ttAssertionMsg(pszMsg, __FILE__, __func__, __LINE__)

    #define ttVERIFY(exp) (void)((!!(exp)) || ttAssertionMsg(#exp, __FILE__, __func__, __LINE__))       // this still executes the expression in non-DEBUG build, it just doesn't check result

    #define ttTRACE(msg) ttTrace(msg)
    #define ttTRACE_CLEAR() ttTraceClear();

    // checks for both ptr == NULL and *ptr == NULL
    #define ttASSERT_NONEMPTY(ptr) { if (!ptr || !*ptr) ttAssertionMsg(ptr, __FILE__, __func__, __LINE__); }    // AssertionMsg figures out if it's nullptr or just empty

    #define ttDISABLE_ASSERTS ttSetAsserts(true)
    #define ttENABLE_ASSERTS  ttSetAsserts(false)

#if defined(_WIN32)
    #define ttASSERT_HRESULT(hr, pszMsg) { if (FAILED(hr)) ttAssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
    #define ttReportLastError() { ttdoReportLastError(__FILE__, __func__, __LINE__); }
#endif    // defined(_WIN32)

#else   // not _DEBUG

    #define ttASSERT(exp)
    #define ttASSERT_MSG(exp, pszMsg)
    #define ttFAIL(pszMsg)

    #define ttVERIFY(exp) ((void)(exp))

    #define ttTRACE(msg)
    #define ttTRACE_CLEAR()

    #define ttASSERT_NONEMPTY(ptr)

    #define ttDISABLE_ASSERTS
    #define ttENABLE_ASSERTS

#if defined(_WIN32)
    #define ttASSERT_HRESULT(hr, pszMsg)
    #define ttReportLastError()
#endif    // defined(_WIN32)
// clang-format on

#endif  // _DEBUG

#endif  // __TTLIB_ASSERTS_H__
