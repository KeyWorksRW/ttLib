/////////////////////////////////////////////////////////////////////////////
// Name:      ttdebug.h
// Purpose:   ttASSERT macros
// Author:    Ralph Walden
// Copyright: Copyright (c) 2000-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Under a _DEBUG build, these ASSERTS will display a message box giving you the
// option to ignore the assert, break into a debugger, or exit the program.

// Note that all functions listed here are available in release builds. Only the macros are removed in release
// builds

#pragma once

#if !defined(_WIN32)
    #error Include ttassert.h and use wxWidgets if you want ttASSERT macros on non-Windows OS
#endif  // _WIN32

#if defined(ttASSERT)
    #error Either include ttassert.h or ttdebug.h -- don't include both!
#endif

// formats a string and displays it in a ttTrace window (if ttTrace is running)
void cdecl ttTrace(const char* pszFormat, ...);

// clears the ttTrace window if ttTrace is running
void ttTraceClear();

bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond,
                    const char* msg = nullptr);
bool ttdoReportLastError(const char* filename, const char* function, int line);

// Enables disables all assertion messages
void ttSetAsserts(bool bDisable);

__declspec(noreturn) void ttOOM(void);

namespace tt
{
    // handle to ttTrace main window (if it was running when ttTrace was called
    extern HWND hwndTrace;

    extern const UINT WMP_TRACE_GENERAL;  // WM_USER + 0x1f3;    // general message to send to ttTrace
    extern const UINT WMP_TRACE_MSG;      // WM_USER + 0x1f5;    // trace message to send to ttTrace
    extern const UINT WMP_CLEAR_TRACE;    // WM_USER + 0x1f9;    // clears the ttTrace window

    // class name of window to send trace messages to
    extern const char* txtTraceClass;
    // name of shared memory to write to
    extern const char* txtTraceShareName;

    int CheckItemID(HWND hwnd, int id, const char* pszID, const char* pszFile, const char* pszFunc, int line);
}  // namespace tt

#ifdef _DEBUG
    #define ttASSERT(cond)                                                       \
        {                                                                        \
            if (!(cond) && ttAssertionMsg(__FILE__, __func__, __LINE__, #cond)) \
            {                                                                    \
                DebugBreak();                                                    \
            }                                                                    \
        }

    #define ttASSERT_MSG(cond, msg)                                            \
        {                                                                      \
            if (!(cond) && ttAssertionMsg(__FILE__, __func__, __LINE__, #cond, msg)) \
            {                                                                  \
                DebugBreak();                                                  \
            }                                                                  \
        }

    #define ttFAIL(msg)                                                 \
        if (ttAssertionMsg(__FILE__, __func__, __LINE__, nullptr, msg)) \
        {                                                               \
            DebugBreak();                                               \
        }

    #define ttFAIL_MSG(msg)                                                 \
        {                                                                   \
            if (ttAssertionMsg(__FILE__, __func__, __LINE__, nullptr, msg)) \
            {                                                               \
                DebugBreak();                                               \
            }                                                               \
        }

    #define ttASSERT_HRESULT(hr, msg)                                                     \
        {                                                                                 \
            if (FAILED(hr) && ttAssertionMsg(__FILE__, __func__, __LINE__, nullptr, msg)) \
            {                                                                             \
                DebugBreak();                                                             \
            }                                                                             \
        }

    #define ttASSERT_NONEMPTY(psz)                                                                               \
        {                                                                                                        \
            if ((!psz || !*psz) && ttAssertionMsg(__FILE__, __func__, __LINE__, #psz, "Null or empty pointer!")) \
            {                                                                                                    \
                DebugBreak();                                                                                    \
            }                                                                                                    \
        }

    #define ttASSERT_STRING(str)                                                                    \
        {                                                                                           \
            if (str.empty() && ttAssertionMsg(__FILE__, __func__, __LINE__, #str, "Empty string!")) \
            {                                                                                       \
                DebugBreak();                                                                       \
            }                                                                                       \
        }

    /// In _DEBUG builds this will display an assertion dialog first then it will throw
    /// an excpetion. In Release builds, only the exception is thrown.
    #define ttTHROW(msg)                                                    \
        {                                                                   \
            if (ttAssertionMsg(__FILE__, __func__, __LINE__, nullptr, msg)) \
            {                                                               \
                DebugBreak();                                               \
            }                                                               \
            throw msg;                                                      \
        }

    /// Display the last system error from GetLastError()
    #define ttLAST_ERROR()                                         \
        {                                                          \
            if (ttdoReportLastError(__FILE__, __func__, __LINE__)) \
            {                                                      \
                DebugBreak();                                      \
            }                                                      \
        }

// this still executes the expression in non-DEBUG build, it just doesn't check result
// #define ttVERIFY(exp) (void)((!!(exp)) || ttAssertionMsg(#exp, __FILE__, __func__, __LINE__))

    #define ttTRACE(msg)    ttTrace(msg)
    #define ttTRACE_CLEAR() ttTraceClear();

    #define ttDISABLE_ASSERTS ttSetAsserts(true)
    #define ttENABLE_ASSERTS  ttSetAsserts(false)

#else  // not _DEBUG

    #define ttASSERT(cond)
    #define ttASSERT_MSG(cond, msg)
    #define ttFAIL(msg)
    #define ttFAIL_MSG(msg)

// #define ttVERIFY(exp) ((void)(exp))

    #define ttTRACE(msg)
    #define ttTRACE_CLEAR()

    #define ttASSERT_NONEMPTY(ptr)
    #define ttASSERT_STRING(str)

    #define ttDISABLE_ASSERTS
    #define ttENABLE_ASSERTS

    #define ttASSERT_HRESULT(hr, pszMsg)
    #define ttLAST_ERROR()

    #define ttTHROW(msg) \
        {                \
            throw msg;   \
        }

#endif  // _DEBUG
