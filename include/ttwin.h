/////////////////////////////////////////////////////////////////////////////
// Name:      ttCWin
// Purpose:   Class for working with windows
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Do NOT use this class to create a MDI windows!

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <wtypes.h>

#if !defined(BEGIN_TTMSG_MAP)
    #include "ttcasemap.h"  // Macros for mapping Windows messages to functions
#endif

namespace ttpriv
{
    LRESULT WINAPI ttCWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

// Non-MDI window handling class.
class ttCWin
{
public:
    ttCWin();
    virtual ~ttCWin();

public:
    // Call these methods before calling CreateWnd

    // Constructor will have set this to COLOR_WINDOW + 1.
    void SetClassBkgrnd(HBRUSH hbkgrnd)
    {
        if (m_pwc)
            m_pwc->hbrBackground = hbkgrnd;
    }
    void SetClassCursor(HCURSOR hcur)
    {
        if (m_pwc)
            m_pwc->hCursor = hcur;
    }
    void SetClassMenu(size_t idMenuResource)
    {
        if (m_pwc)
            m_pwc->lpszMenuName = (const char*) idMenuResource;
    }

    // Returns false if strLen(pszClassName) > 255.
    bool SetClassName(const char* pszClassName);

    // Constructor will have set this to CS_HREDRAW | CS_VREDRAW.
    void SetClassStyle(DWORD style)
    {
        if (m_pwc)
            m_pwc->style = style;
    }
    void SetWndExtra(int cbExtra)
    {
        if (m_pwc)
            m_pwc->cbWndExtra = cbExtra;
    }
    void SetClsExtra(int cbExtra)
    {
        if (m_pwc)
            m_pwc->cbClsExtra = cbExtra;
    }

    void SetClassIcon(size_t idIcon)
    {
        if (m_pwc)
            m_pwc->hIcon = LoadIcon(m_pwc->hInstance, (LPCTSTR) idIcon);
    }
    void SetClassIcon(HICON hIcon)
    {
        if (m_pwc)
            m_pwc->hIcon = hIcon;
    }
    void SetClassSmallIcon(size_t idIcon)
    {
        if (m_pwc)
            m_pwc->hIconSm = LoadIcon(m_pwc->hInstance, (LPCTSTR) idIcon);
    }
    void SetClassSmallIcon(HICON hIcon)
    {
        if (m_pwc)
            m_pwc->hIconSm = hIcon;
    }

    bool CreateWnd(const char* pszTitle, DWORD dwExStyle, DWORD dwStyle, HWND hwndParent = NULL, RECT* prc = NULL,
                   HMENU hmenu = NULL);

    // Attaches to a window not created by ttCWin, updates m_pszClassName, m_hwnd and m_hwndParent.
    bool AttachWnd(HWND hwnd);

    // if hwnd is NULL, subclass our own window.
    bool SubClass(HWND hwnd = NULL);

    // Class functions

    LRESULT SendMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
    {
        return ::SendMessageA(*this, msg, wParam, lParam);
    }
    LRESULT PostMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
    {
        return ::PostMessageA(*this, msg, wParam, lParam);
    }

    void ShowWindow(int nCmdShow = SW_SHOW) { ::ShowWindow(*this, nCmdShow); }

    void SetTitle(const char* pszTitle) { ::SetWindowTextA(*this, pszTitle); }

    // For other Windows functions requiring an HWND parameter, simply pass in *this as the HWND.

    ////////// Operators //////////

    operator HWND() const { return m_hwnd; }
    operator WNDCLASSEXA*() { return m_pwc; }

protected:
    // BEGIN_TTCMD_MAP in ttcasemap.h will override this.
    virtual bool OnCmdCaseMap(int /* id */, int /* NotifyCode */, LRESULT& /* lResult */) { return false; }

    // BEGIN_TTMSG_MAP in ttcasemap.h will override this.
    virtual bool OnMsgMap(UINT /* msg */, WPARAM /* wParam */, LPARAM /* lParam */, LRESULT& lResult)
    {
        lResult = 0;
        return false;
    }

    friend LRESULT WINAPI ttpriv::ttCWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Use protected, not private because other classes inherit from this class, and currently expect access to the
    // class members.
protected:
    // Class members

    const char* m_pszClassName;  // Class name of the window we created or attached to.

    HWND m_hwnd;  // m_hwnd vs m_hWnd -- SDK/include, ATL and WTL use both variants. We're sticking with all
                  // lowercase.
    HWND m_hwndParent;

    // Instance used to create the class, can be used to load resources from the app.
    HINSTANCE m_hinst;

    // Created in constructor, deleted by CreateWnd and AttachWnd.
    WNDCLASSEXA* m_pwc;

    // Previous window procedure before it was subclassed.
    WNDPROC m_SubClassProc;
    LRESULT m_result;
};
