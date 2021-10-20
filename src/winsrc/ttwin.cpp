/////////////////////////////////////////////////////////////////////////////
// Name:      ttwin.cpp
// Purpose:   Class for working with windows
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include "ttcstr.h"   // cstr -- Classes for handling zero-terminated char strings.
#include "ttdebug.h"  // ttASSERT macros
#include "ttwin.h"    // ttlib::win

using namespace ttlib;

// This is the Window procedure used by all windows that ttlib::win created or subclassed.
LRESULT WINAPI ttlib::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ttlib::win* pThis;
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pcs = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<ttlib::win*>(pcs->lpCreateParams);
        ttASSERT_MSG(pThis, "this pointer not supplied to CREATESTRUCT");
        if (pThis)
        {
            pThis->m_hwnd = hwnd;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR) pThis);
        }
    }
    else
        pThis = reinterpret_cast<ttlib::win*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    // When the Window is created, WM_GETMINMAXINFO may get sent before WM_NCCREATE, in which case
    // GetWindowLongPtrW will return NULL.

    if (pThis)
    {
        LRESULT lResult = 0;
        // Both OnCmdCaseMap and OnMsgMap pass lResult by reference so it may change before the return.
        if (msg == WM_COMMAND && pThis->OnCmdCaseMap((int) LOWORD(wParam), (UINT) HIWORD(wParam), lResult))
            return lResult;
        if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
            return lResult;

        if (pThis->m_SubClassProc)
        {
            return CallWindowProcW(pThis->m_SubClassProc, hwnd, msg, wParam, lParam);
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void win::InitWndClass()
{
    std::memset(&m_WndClass, 0, sizeof(WNDCLASSEXW));

    m_WndClass.cbSize = sizeof(WNDCLASSEXW);
    m_WndClass.hInstance = GetModuleHandle(NULL);
    m_WndClass.lpfnWndProc = ttlib::WndProc;
    m_WndClass.style = CS_HREDRAW | CS_VREDRAW;
    m_WndClass.hbrBackground = (HBRUSH) (LONG_PTR) (COLOR_WINDOW + 1);
    m_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Note that m_WndClass.lpszClassName doesn't get set until the call to CreateWnd.
}

void win::SetClassName(std::string_view ClassName)
{
    ttASSERT(ClassName.length() < 256);
    m_ClassName.clear();
    ttlib::utf8to16(ClassName, m_ClassName);
}

// If not already registered, this will register the class and delete m_pwc before calling CreateWindowExA.
bool win::CreateWnd(const std::string& Title, DWORD dwExStyle, DWORD dwStyle, HWND hwndParent, RECT* prcPosition,
                    HMENU hmenu)
{
    if (!isRegistered)
    {
        if (m_ClassName.empty())
        {
            // attempt to make a unique class name
            ttlib::cstr Class;
            Class.Format("ttlibWin%x", GetTickCount());
            ttlib::utf8to16(Class, m_ClassName);
        }

        // Just in case m_ClassName got changed, set the pointer to be certain it is valid.
        m_WndClass.lpszClassName = m_ClassName.c_str();

        if (::RegisterClassExW(&m_WndClass) == 0)
            return false;
        isRegistered = true;
    }

    m_hwndParent = hwndParent;

    std::wstring title16;
    ttlib::utf8to16(Title, title16);

    if (prcPosition != NULL)
        ::CreateWindowExW(dwExStyle, m_ClassName.c_str(), title16.c_str(), dwStyle, prcPosition->left, prcPosition->top,
                          ttlib::rcWidth(prcPosition), ttlib::rcHeight(prcPosition), hwndParent, hmenu, m_WndClass.hInstance,
                          (void*) this);
    else
        ::CreateWindowExW(dwExStyle, m_ClassName.c_str(), title16.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, hmenu, m_WndClass.hInstance, (void*) this);

    return (m_hwnd && IsWindow(m_hwnd));
}

bool win::SubClass(HWND hwnd)
{
    ttASSERT_MSG(!m_SubClassProc, "You can only subclass a window once!");
    ttASSERT_MSG(hwnd, "You cannot subclass a NULL window handle");

    if (m_SubClassProc || !hwnd)
        return false;

    m_hwnd = hwnd;

    wchar_t ClassName[256];
    if (::GetClassNameW(hwnd, ClassName, sizeof(ClassName)) != 0)
        m_ClassName.assign(ClassName);

    m_hwndParent = GetParent(hwnd);

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR) this);

    m_SubClassProc = (WNDPROC) SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR) ttlib::WndProc);

    return (m_SubClassProc != nullptr);
}

void win::SetTitle(std::string_view Title)
{
    std::wstring str16;
    ttlib::utf8to16(Title, str16);
    ::SetWindowTextW(m_hwnd, str16.c_str());
}
