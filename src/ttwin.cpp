/////////////////////////////////////////////////////////////////////////////
// Name:      ttCWin
// Purpose:   Class for working with windows
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttdebug.h"  // ttASSERT macros
#include "../include/ttstr.h"    // ttCStr
#include "../include/ttwin.h"    // ttCWin

// This is the Window procedure used by all windows that ttCWin created or subclassed.
LRESULT WINAPI ttpriv::ttCWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ttCWin* pThis;
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCTA* pcs = (CREATESTRUCTA*) lParam;
        pThis = (ttCWin*) pcs->lpCreateParams;
        ttASSERT_MSG(pThis, "this pointer not supplied to CREATESTRUCT");
        if (pThis)
        {
            pThis->m_hwnd = hwnd;
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR) pThis);
        }
    }
    else
        pThis = (ttCWin*) GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    if (pThis)
    {
        LRESULT lResult = 0;
        // Both OnCmdCaseMap and OnMsgMap pass lResult by reference so it may change before the return.
        if (msg == WM_COMMAND && pThis->OnCmdCaseMap((int) LOWORD(wParam), (UINT) HIWORD(wParam), lResult))
            return lResult;
        if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
            return lResult;

        if (pThis->m_SubClassProc)
            return CallWindowProcA(pThis->m_SubClassProc, hwnd, msg, wParam, lParam);
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

ttCWin::ttCWin()
{
    m_hwnd = NULL;
    m_hwndParent = NULL;
    m_SubClassProc = nullptr;
    m_pszClassName = nullptr;

    // We create this here so the caller can modify it before calling CreateWnd

    m_pwc = new WNDCLASSEXA;
    memset(m_pwc, 0, sizeof(WNDCLASSEXA));
    m_pwc->cbSize = sizeof(WNDCLASSEXA);
    m_pwc->lpfnWndProc = ttpriv::ttCWinProc;
    m_pwc->style = CS_HREDRAW | CS_VREDRAW;
    m_pwc->hbrBackground = (HBRUSH)(LONG_PTR)(COLOR_WINDOW + 1);
    m_pwc->hCursor = LoadCursor(NULL, IDC_ARROW);
}

ttCWin::~ttCWin()
{
    if (m_pwc)  // this will have already been deleted if CreateWnd or Attach was called
        delete m_pwc;
    if (m_pszClassName)
        ::free((void*) m_pszClassName);
}

bool ttCWin::SetClassName(const char* pszClassName)
{
    ttASSERT_NONEMPTY(pszClassName);

    if (!pszClassName || !*pszClassName ||
        ttStrLen(pszClassName) > 256)  // Windows limits class names to 256 characters
        return false;

    if (m_pszClassName)
        ::free((void*) m_pszClassName);
    m_pszClassName = _strdup(pszClassName);
    if (m_pwc)
        m_pwc->lpszClassName = m_pszClassName;
    return true;
}

// If not already registered, this will register the class and delete m_pwc before calling CreateWindowExA.
bool ttCWin::CreateWnd(const char* pszTitle, DWORD dwExStyle, DWORD dwStyle, HWND hwndParent, RECT* prcPosition,
                       HMENU hmenu)
{
    if (m_pwc)
    {  // means the class hasn't been registered yet
        if (!m_pszClassName)
        {
            // attempt to make a unique class name
            ttCStr cszClass(32);
            cszClass.StrCopy("ttCWin");
            ttHextoa(GetTickCount(), (char*) cszClass + cszClass.StrLen(), true);
            m_pszClassName = _strdup(cszClass);
            m_pwc->lpszClassName = m_pszClassName;
        }
        m_hinst = m_pwc->hInstance = GetModuleHandle(NULL);
        if (::RegisterClassExA(m_pwc) == 0)
            return false;

        delete m_pwc;
        m_pwc = nullptr;
    }

    m_hwndParent = hwndParent;

    if (prcPosition != NULL)
        ::CreateWindowExA(dwExStyle, m_pszClassName, pszTitle, dwStyle, prcPosition->left, prcPosition->top,
                          ttRC_WIDTH(prcPosition), ttRC_HEIGHT(prcPosition), hwndParent, hmenu, m_hinst,
                          (void*) this);
    else
        ::CreateWindowExA(dwExStyle, m_pszClassName, pszTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, hmenu, m_hinst, (void*) this);

    return ttIsValidWindow(m_hwnd);
}

bool ttCWin::SubClass(HWND hwnd)
{
    ttASSERT_MSG(!m_SubClassProc, "You can only subclass a window once!");

    if (m_SubClassProc)
        return false;

    if (hwnd != NULL)
        AttachWnd(hwnd);

    SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR) this);

    m_SubClassProc = (WNDPROC) SetWindowLongPtrA(hwnd, GWLP_WNDPROC, (LONG_PTR) ttpriv::ttCWinProc);

    return (m_SubClassProc != nullptr);
}

bool ttCWin::AttachWnd(HWND hwnd)
{
    if (hwnd == NULL || !IsWindow(hwnd))
        return false;
    m_hwnd = hwnd;

    // Deleting m_pwc and getting the window's class name makes it possible to call CreateWnd and create another
    // window of the same type that we attached to.

    ttCStr cszClassName(256);
    if (::GetClassNameA(hwnd, cszClassName, (int) cszClassName.SizeBuffer()) != 0)
    {
        if (m_pwc)
        {
            delete m_pwc;
            m_pwc = nullptr;
        }
        SetClassName(cszClassName);  // we store the name separately from m_pwc
    }
    m_hwndParent = GetParent(hwnd);

    return true;
}
