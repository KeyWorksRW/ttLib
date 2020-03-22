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
    #error The functions and macros in this header file are only available on Windows
#endif  // _WIN32

// With #pragma once, a header guard shouldn't be necessary and causes unwanted indentation by clang-format. The
// following #if/#endif check verifies that the file wasn't read twice.

#if defined(_TT_LIB_DEBUG_H_GUARD_)
    #error "#pragma once failed -- header is being read a second time!"
#else
    #define _TT_LIB_DEBUG_H_GUARD_  // sanity check to confirm that #pragma once is working as expected
#endif

#include <sstream>

#include "../include/ttcview.h"

namespace ttlib
{
    /// Displays message in ttTrace.exe window if it is running.
    void wintrace(ttlib::cview msg);
    /// Clears all text in ttTrace.exe window (if it is running).
    void wintraceclear();

    // ttASSERTS are enabled by default -- you can change that state with this function.
    void allow_asserts(bool allowasserts = false);
}  // namespace ttlib

// bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond, const char* msg);
bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond,
                    const std::string& msg);
bool ttdoReportLastError(const char* filename, const char* function, int line);

inline bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond,
                           const std::stringstream& msg)
{
    return ttAssertionMsg(filename, function, line, cond, msg.str().c_str());
}

__declspec(noreturn) void ttOOM(void);

namespace ttlib
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
}  // namespace ttlib

#ifdef _DEBUG
    #define ttASSERT(cond)                                                               \
        {                                                                                \
            if (!(cond) && ttAssertionMsg(__FILE__, __func__, __LINE__, #cond, nullptr)) \
            {                                                                            \
                DebugBreak();                                                            \
            }                                                                            \
        }

    #define ttASSERT_MSG(cond, msg)                                                  \
        {                                                                            \
            if (!(cond) && ttAssertionMsg(__FILE__, __func__, __LINE__, #cond, msg)) \
            {                                                                        \
                DebugBreak();                                                        \
            }                                                                        \
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
    #define ttVERIFY(exp) (void) ((!!(exp)) || ttAssertionMsg(__FILE__, __func__, __LINE__, #exp, nullptr))

    #define ttTRACE(msg)    ttTrace(msg)
    #define ttTRACE_CLEAR() ttTraceClear();

    #define ttDISABLE_ASSERTS ttSetAsserts(true)
    #define ttENABLE_ASSERTS  ttSetAsserts(false)

#else  // not _DEBUG

    #define ttASSERT(cond)
    #define ttASSERT_MSG(cond, msg)
    #define ttFAIL(msg)
    #define ttFAIL_MSG(msg)

    #define ttVERIFY(exp) ((void) (exp))

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
