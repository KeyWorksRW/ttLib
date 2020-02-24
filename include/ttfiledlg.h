/////////////////////////////////////////////////////////////////////////////
// Name:      ttCFileDlg
// Purpose:   Wrapper around Windows GetOpenFileName() API
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <commdlg.h>

#include "ttdebug.h"     // for ttASSERTS
#include "ttstr.h"       // ttCStr
#include "ttmultibtn.h"  // ttCMultiBtn

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

    char* GetFileName()
    {
        return m_cszFileName;
    }  // call this after one of the above two functions has been called

    void AddToRecent() { m_pofn->Flags &= ~OFN_DONTADDTORECENT; }
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
        m_pofn->Flags &= ~OFN_FILEMUSTEXIST;
        m_pofn->Flags |= OFN_CREATEPROMPT;
    }
    void ShowReadOnlyBox() { m_pofn->Flags &= ~OFN_HIDEREADONLY; }
    void UseCurrentDirectory()
    {
        m_cszCurDir.GetCWD();
        m_pofn->lpstrInitialDir = m_cszCurDir;
    }
    void RestoreDirectory() { m_pofn->Flags |= OFN_NOCHANGEDIR; }

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
            m_pofn->Flags |= OFN_FILEMUSTEXIST;
        else
            m_pofn->Flags &= ~OFN_FILEMUSTEXIST;
    }

    // Adds one or more of the OFN_ flags
    void AddFlags(DWORD flags) { m_pofn->Flags |= flags; }

    OPENFILENAMEA* GetOF() { return m_pofn; }
    operator OPENFILENAMEA*() const { return m_pofn; }
    operator char*() const { return (char*) m_cszFileName; }

private:
    void FixExtension();

    friend UINT_PTR CALLBACK ttpriv::OFNHookProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Class members

protected:
    ttCStr m_cszFileName;
    ttCStr m_cszCurDir;
    ttCStr m_cszFilter;
    ttCStr m_cszSetDir;

    RECT m_rcPosition;

    // This is Malloc'd because the size changes depending on the version of Windows
    OPENFILENAMEA* m_pofn;

    ttCMultiBtn m_ShadedBtns;
    UINT m_idOpenIcon;
    UINT m_idCancelIcon;

    bool m_bRepositionWindow;
    bool m_bShadeBtns;
};
