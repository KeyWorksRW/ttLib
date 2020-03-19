/////////////////////////////////////////////////////////////////////////////
// Name:      ttCDirDlg
// Purpose:   Class for displaying a dialog to select a directory
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// Caution! This class will call COM functions, which means you must have called CoInitializeEx for the thread
/// that is using this class.

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include "ttcstr.h"  // cstr -- Classes for handling zero-terminated char strings.

namespace ttlib
{
    /// Class displaying a dialog that asks the user to select a Folder.
    class DirDlg : public cstr
    {
    public:
        DirDlg();

        // Class functions

        /// Default title is "Select a Folder" -- call this to change it.
        void SetTitle(std::string_view Title) { m_Title = Title; };
        void SetStartingDir(std::string_view StartingDir) { m_StartingDir = StartingDir; };

        bool GetFolderName(HWND hwndParent = NULL);

    private:
        // Class members

        ttlib::cstr m_Title;
        ttlib::cstr m_StartingDir;
    };
}  // end namespace ttlib
