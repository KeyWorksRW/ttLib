/////////////////////////////////////////////////////////////////////////////
// Name:      ttCFileDlg
// Purpose:   Class for displaying Windows File Open dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include "ttfiledlg.h"

ttCFileDlg::ttCFileDlg(HWND hwndParent)
{
    m_idOpenIcon = (UINT) -1;
    m_idCancelIcon = (UINT) -1;
    m_bShadeBtns = false;

    m_cszFileName.reserve(MAX_PATH);

    m_ofn.lStructSize = sizeof(OPENFILENAME);
    m_ofn.hwndOwner = hwndParent ? hwndParent : GetActiveWindow();
    m_ofn.lpstrFile = m_cszFileName.data();
    m_ofn.nMaxFile = MAX_PATH;
    m_ofn.lpfnHook = ttpriv::OFNHookProc;
    m_ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
                  OFN_DONTADDTORECENT;
    m_ofn.lCustData = (LPARAM) this;

    m_bRepositionWindow = false;
    memset(&m_rcPosition, 0, sizeof(m_rcPosition));
}

ttCFileDlg::~ttCFileDlg() {}

bool ttCFileDlg::GetOpenName()
{
    if (!::GetOpenFileNameA(&m_ofn))
    {
#if !defined(NDEBUG)                           // Starts debug section.
        DWORD error = CommDlgExtendedError();  // Will be 0 if the user cancelled, else it's an actual error
        ttASSERT_MSG(error == 0, "An error occurred in the Open dialog box");
#endif
        return false;
    }

    if (m_ofn.Flags & OFN_FILEMUSTEXIST)
        FixExtension();
    return true;
}

bool ttCFileDlg::GetSaveName()
{
    m_ofn.Flags &= ~OFN_FILEMUSTEXIST;
    m_ofn.Flags |= OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (!::GetSaveFileNameA(&m_ofn))
    {
#if !defined(NDEBUG)                           // Starts debug section.
        DWORD error = CommDlgExtendedError();  // Will be 0 if the user cancelled, else it's an actual error
        ttASSERT_MSG(error == 0, "An error occurred in the Open dialog box");
#endif
        return false;
    }
    FixExtension();
    return true;
}

void ttCFileDlg::FixExtension()
{
    if (!m_cszFileName.extension().empty())
        return;  // we have an extension, return

    const char* psz = m_ofn.lpstrFilter;
    for (DWORD i = 1; i < m_ofn.nFilterIndex; i++)
    {
        psz = psz + (std::strlen(psz) + 1);
        psz = psz + (std::strlen(psz) + 1);
    }
    psz = psz + std::strlen(psz) + 1;
    ttASSERT(psz);
    char* pszTmp = const_cast<char*>(std::strchr(psz, ';'));
    if (pszTmp)
        *pszTmp = '\0';
    m_cszFileName.replace_extension(psz + 1);
}

void ttCFileDlg::SetFilter(const char* pszFilters)
{
    ttASSERT(pszFilters);
    if (!pszFilters)
        return;

    m_cszFilter = pszFilters;
    char* psz = std::strchr(m_cszFilter.data(), '|');
    while (psz)
    {
        *psz = '\0';
        psz = std::strchr(psz + 1, '|');
    }
    m_ofn.lpstrFilter = m_cszFilter.data();
}

void ttCFileDlg::SetFilter(int idResource)
{
    m_cszFilter.LoadStringEx(static_cast<WORD>(idResource));
    char* psz = std::strchr(m_cszFilter.data(), '|');
    while (psz)
    {
        *psz = '\0';
        psz = std::strchr(psz + 1, '|');
    }
    m_ofn.lpstrFilter = m_cszFilter.data();
}

UINT_PTR CALLBACK ttpriv::OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        SetWindowLongPtrA(hdlg, GWLP_USERDATA, ((OPENFILENAME*) lParam)->lCustData);
        ttCFileDlg* pThis = (ttCFileDlg*) ((OPENFILENAME*) lParam)->lCustData;

        if (pThis->m_bShadeBtns)
        {
            pThis->m_ShadedBtns.Initialize(GetParent(hdlg));
            if (pThis->m_idOpenIcon != (UINT) -1)
                pThis->m_ShadedBtns.SetIcon(IDOK, pThis->m_idOpenIcon);
            if (pThis->m_idCancelIcon != (UINT) -1)
                pThis->m_ShadedBtns.SetIcon(IDCANCEL, pThis->m_idCancelIcon);
        }

        if (!IsRectEmpty(&pThis->m_rcPosition))
            pThis->m_bRepositionWindow = true;
#if 0  // REVIEW: [randalphwa - 09-03-2018] enable this if we add CenterWindow()
        else
            CenterWindow(GetParent(GetParent(hdlg)), GetParent(hdlg));
#endif
        return TRUE;
    }

    if (uMsg == WM_NOTIFY)
    {
        OFNOTIFY* ofn = (OFNOTIFY*) lParam;
        switch (ofn->hdr.code)
        {
                // We cannot reposition the window during the CDN_INITDONE or WM_INITDIALOG messages because the
                // dialog will not be drawn correctly, and cannot be resized (system determines dialog
                // size/layout after this message is processed). It also doesn't work to hide the window and
                // display it later because the system will show the window after we tell it to be hidden. So
                // instead, we track the last notification message we receive before the dialog is displayed, and
                // set the dialog size/position there.

            case CDN_FOLDERCHANGE:
            {
                ttCFileDlg* pThis = (ttCFileDlg*) GetWindowLongPtr(hdlg, GWLP_USERDATA);
                if (pThis->m_bRepositionWindow)
                {
                    pThis->m_bRepositionWindow = false;
                    MoveWindow(GetParent(hdlg), pThis->m_rcPosition.left, pThis->m_rcPosition.top,
                               ttlib::rcWidth(pThis->m_rcPosition), ttlib::rcHeight(pThis->m_rcPosition), FALSE);
                }
            }
            break;
        }
    }
    else if (uMsg == WM_DESTROY)
    {
        ttCFileDlg* pThis = (ttCFileDlg*) GetWindowLongPtr(hdlg, GWLP_USERDATA);
        GetWindowRect(GetParent(hdlg), &pThis->m_rcPosition);
    }
    return 0;
}

void ttCFileDlg::SetInitialDir(const char* pszFolder)
{
    ttASSERT_MSG(pszFolder, "NULL pointer!");

    m_cszSetDir = pszFolder;
    m_cszSetDir.make_absolute();  // probably not necessary, but doesn't hurt
    ttlib::backslashestoforward(m_cszSetDir);
    m_ofn.lpstrInitialDir = m_cszSetDir.data();
}
