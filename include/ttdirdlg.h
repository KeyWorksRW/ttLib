/////////////////////////////////////////////////////////////////////////////
// Name:      ttCDirDlg
// Purpose:   Class for displaying a dialog to select a directory
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <shlobj.h>

#include "ttstr.h"   // ttCStr
#include "ttwstr.h"  // ttCWStr

class ttCDirDlg
{
public:
    ttCDirDlg();

    // Class functions

    void SetTitle(const char* pszTitle) { m_cwszTitle = pszTitle; }
    void SetStartingDir(const char* pszDir) { m_cwszStartingDir = pszDir; }

    bool GetFolderName(HWND hwndParent = NULL);

    const char* c_str() { return m_cszDirName.c_str(); }
    operator char*() const { return (char*) m_cszDirName; }

protected:
    // Class members

    // Windows default title is "Select a Folder"
    ttCWStr m_cwszTitle;
    ttCWStr m_cwszStartingDir;
    ttCStr  m_cszDirName;
};
