/////////////////////////////////////////////////////////////////////////////
// Purpose:   Various debugging functionality
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

// This module is *NOT* compiled into any of the library variations -- it is designed for programs that
// compile ttLib source code directly rather than using a library.

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <mutex>

#include "ttcstr.h"   // Classes for handling zero-terminated char strings.
#include "ttdebug.h"  // ttASSERT macros

namespace ttdbg
{
    std::mutex mutexAssert;
}  // namespace ttdbg

bool ttAssertionMsg(const char* filename, const char* function, int line, const char* cond, const std::string& msg)
{
    std::unique_lock<std::mutex> classLock(ttdbg::mutexAssert);

    ttlib::cstr str;
    // Start by creating a string to send to the debugger
    if (cond)
        str << "Expression: " << cond << "\n";
    if (!msg.empty())
        str << "Comment: " << msg << "\n";
    str << filename << '(' << line << ')' << "\n";
    str << "Function: " << function << "\n";
    OutputDebugStringW(str.to_utf16().c_str());
    str.clear();

    if (cond)
        str << "Expression: " << cond << "\n\n";
    if (!msg.empty())
        str << "Comment: " << msg << "\n\n";

    str << "File: " << filename << "\n";
    str << "Function: " << function << "\n";
    str << "Line: " << line << "\n\n";
    str << "Press Retry to break into a debugger.";

    auto answer =
        MessageBoxW(GetActiveWindow(), str.to_utf16().c_str(), L"Assertion failed!", MB_ABORTRETRYIGNORE | MB_ICONSTOP);

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
