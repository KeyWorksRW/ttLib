/////////////////////////////////////////////////////////////////////////////
// Name:      ttassert.cpp
// Purpose:   Implements ttAssert function
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <wx/app.h>
#include <wx/debug.h>
#include <wx/richmsgdlg.h>
#include <wx/string.h>

#include <sstream>

#include "ttassert.h"

#if defined(NDEBUG)  // Release build.

void ttAssert(std::string_view, int, std::string_view, std::string_view, std::string_view)
{
    return;  // Do nothing if this is a release build.
}

#else  // Begins debug build section.

namespace ttwx
{
    bool bNoAssert = false;  // Setting this to true will cause ttAssert to return without doing anything.
}

void ttAssert(std::string_view file, int line, std::string_view func, std::string_view cond, std::string_view msg)
{
    if (ttwx::bNoAssert)
        return;  // We're either already in an assert, or user shut off all asserts.

    auto bSave = ttwx::bNoAssert;
    ttwx::bNoAssert = true;  // This avoids being called from another thread while processing this assert.

    // Worst-case is that wxString also asserts which generate it's own display before ours is displayed.

    try
    {
        std::ostringstream str;
        if (!cond.empty())
            str << cond << "\n\n";
        if (!msg.empty())
            str << msg << "\n\n";
        str << file << " (" << func << "): line " << line;

        // Accorinding to wxWidgets, wxIsDebuggerRunning() currently only works on Windows

    #if defined(__WIN32__)
        bool bDebuggerRunning = wxIsDebuggerRunning();
    #else
        bool bDebuggerRunning = true;
    #endif

        wxRichMessageDialog dlg(NULL, str.str(), "Assertion Failure",
                                wxICON_WARNING | wxCANCEL | (bDebuggerRunning ? wxYES_NO : wxOK));
        if (bDebuggerRunning)
            dlg.SetYesNoCancelLabels("Debug", "Continue", "Terminate");
        else
            dlg.SetOKCancelLabels("Continue", "Terminate");

        wxString szSysError("Last system error msg: ");
        szSysError += wxSysErrorMsgStr();
        dlg.ShowDetailedText(szSysError);

        auto result = dlg.ShowModal();

        if (result == wxID_CANCEL)
            wxExit();
        else if (bDebuggerRunning && result == wxID_YES)
            wxTrapInAssert = true;
    }
    catch (const std::exception& /* e */)
    {
        wxTrap();  // There's really nothing else we can do at this point
    }

    ttwx::bNoAssert = bSave;
}

#endif  // End debug-only section
