/////////////////////////////////////////////////////////////////////////////
// Name:      ttopenfile.cpp
// Purpose:   Wrapper around Windows GetOpenFileName() API
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include "ttopenfile.h"

using namespace ttlib;

openfile::openfile(HWND hwndParent) : OPENFILENAMEW()
{
    m_filename16 = std::make_unique<wchar_t[]>(MAX_PATH);

    lStructSize = sizeof(OPENFILENAMEW);
    hwndOwner = hwndParent ? hwndParent : GetActiveWindow();
    lpstrFile = m_filename16.get();
    nMaxFile = MAX_PATH;
    Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT;
    lCustData = reinterpret_cast<LPARAM>(this);

    ttlib::cstr cwd;
    cwd.assignCwd();
    SetInitialDir(cwd);
}

bool openfile::GetOpenName()
{
    m_filename.clear();

    if (!::GetOpenFileNameW(this))
    {
        return false;
    }

    ttlib::utf16to8(m_filename16.get(), m_filename);
    return true;
}

bool openfile::GetSaveName()
{
    m_filename.clear();

    Flags &= ~OFN_FILEMUSTEXIST;
    Flags |= OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (!::GetSaveFileNameW(this))
    {
        m_filename.clear();
        return false;
    }

    ttlib::utf16to8(m_filename16.get(), m_filename);
    return true;
}

void openfile::SetFilter(std::string_view filters)
{
    ttlib::utf8to16(filters, m_filters16);
    if (m_filters16.back() != L'|')
        m_filters16 += '|';

    // Windows wants each filter part to be zero-terminated, so replace | with a zero
    for (size_t pos = 0; m_filters16[pos]; ++pos)
    {
        if (m_filters16[pos] == L'|')
            m_filters16[pos] = 0;
    }
    lpstrFilter = m_filters16.data();
}

void openfile::SetInitialDir(std::string_view dir)
{
    ttlib::utf8to16(dir, m_initialDir16);
    lpstrInitialDir = m_initialDir16.data();
}

void openfile::SetInitialFileName(std::string_view filename)
{
    std::wstring str16;
    ttlib::utf8to16(filename, str16);
    ttASSERT(str16.length() < MAX_PATH);
    lstrcpynW(m_filename16.get(), str16.c_str(), MAX_PATH);
    lpstrFile = m_filename16.get();
}

void openfile::EnableShadeBtns(bool Enable)
{
    m_ShadeBtns = Enable;
    if (Enable)
    {
        lpfnHook = ttlib::OFNHookProc;
        Flags |= OFN_ENABLEHOOK;
    }
    else
    {
        lpfnHook = NULL;
        Flags &= ~OFN_ENABLEHOOK;
    }
}

UINT_PTR CALLBACK ttlib::OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        SetWindowLongPtrW(hdlg, GWLP_USERDATA, ((OPENFILENAMEW*) lParam)->lCustData);
        auto pthis = reinterpret_cast<ttlib::openfile*>(GetWindowLongPtr(hdlg, GWLP_USERDATA));

        if (pthis->m_ShadeBtns)
        {
            pthis->m_ShadedBtns.Initialize(GetParent(hdlg));
            if (pthis->m_idOpenIcon != static_cast<UINT>(-1))
                pthis->m_ShadedBtns.SetIcon(IDOK, pthis->m_idOpenIcon);
            if (pthis->m_idCancelIcon != static_cast<UINT>(-1))
                pthis->m_ShadedBtns.SetIcon(IDCANCEL, pthis->m_idCancelIcon);
        }
        return TRUE;
    }
    return 0;
}
