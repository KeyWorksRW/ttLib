/////////////////////////////////////////////////////////////////////////////
// Name:      ttCFileDlg
// Purpose:   Wrapper around Windows GetOpenFileName() API
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <vector>

#include <commdlg.h>

#include "ttcstr.h"      // cstr -- Classes for handling zero-terminated char strings.
#include "ttdebug.h"     // for ttASSERTS
#include "ttmultibtn.h"  // ttlib::MultiBtn

#ifndef OFN_DONTADDTORECENT
    #define OFN_DONTADDTORECENT 0x02000000
#endif

namespace ttpriv
{
    UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam);
}

// Wrapper around Windows GetOpenFileName() API
class ttCFileDlg
{
public:
    ttCFileDlg(HWND hwndParent = NULL);
    ~ttCFileDlg();

    // Class functions

    bool GetOpenName();  // call this or the following to launch the actual dialog box
    bool GetSaveName();

    const char* GetFileName()
    {
        return m_cszFileName.c_str();
    }  // call this after one of the above two functions has been called

    void AddToRecent() { m_ofn.Flags &= ~OFN_DONTADDTORECENT; }
    void SetFilter(int idResource);
    // separate filters with '|' character
    void SetFilter(const char* pszFilters);
    void SetInitialDir(const char* pszFolder);
    void SetInitialFileName(const char* psz)
    {
        m_cszFileName = psz;
        ttASSERT(m_cszFileName.length() < MAX_PATH);
    }
    void ShowCreatePrompt()
    {
        m_ofn.Flags &= ~OFN_FILEMUSTEXIST;
        m_ofn.Flags |= OFN_CREATEPROMPT;
    }
    void ShowReadOnlyBox() { m_ofn.Flags &= ~OFN_HIDEREADONLY; }
    void UseCurrentDirectory()
    {
        m_cszCurDir.assignCwd();
        m_ofn.lpstrInitialDir = m_cszCurDir.c_str();
    }
    void RestoreDirectory() { m_ofn.Flags |= OFN_NOCHANGEDIR; }

    void SetWindowRect(const RECT* prc)
    {
        ttASSERT(!IsRectEmpty(prc));
        CopyRect(&m_rcPosition, prc);
    }
    RECT* GetWindowRect() { return &m_rcPosition; }

    void EnableShadeBtns(bool bEnable = true) { m_bShadeBtns = bEnable; }
    void SetOpenIcon(UINT idIcon) { m_idOpenIcon = idIcon; }
    void SetCancelIcon(UINT idIcon) { m_idCancelIcon = idIcon; }

    // By default, the file must exist

    void SetFileMustExist(bool bMustExist = true)
    {
        if (bMustExist)
            m_ofn.Flags |= OFN_FILEMUSTEXIST;
        else
            m_ofn.Flags &= ~OFN_FILEMUSTEXIST;
    }

    // Adds one or more of the OFN_ flags
    void AddFlags(DWORD flags) { m_ofn.Flags |= flags; }

    OPENFILENAMEA* GetOF() { return &m_ofn; }
    operator OPENFILENAMEA*() { return &m_ofn; }

private:
    void FixExtension();

    friend UINT_PTR CALLBACK ttpriv::OFNHookProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Class members

protected:
    ttlib::cstr m_cszFileName;
    ttlib::cstr m_cszCurDir;
    ttlib::cstr m_cszFilter;
    ttlib::cstr m_cszSetDir;

    RECT m_rcPosition;

    OPENFILENAME m_ofn;

    ttlib::MultiBtn m_ShadedBtns;
    UINT m_idOpenIcon;
    UINT m_idCancelIcon;

    bool m_bRepositionWindow;
    bool m_bShadeBtns;
};
