/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::win
// Purpose:   Class for working with windows
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Do NOT use this class to create a MDI windows!

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <memory>
#include <string>
#include <string_view>

#include <wtypes.h>

#include "ttlibspace.h"

#if !defined(BEGIN_TTMSG_MAP)
    #include "ttcasemap.h"  // Macros for mapping Windows messages to functions
#endif

namespace ttlib
{
    LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Non-MDI window handling class.
    class win
    {
    public:
        win() { InitWndClass(); };
        ~win() = default;

    public:
        // Call these methods before calling CreateWnd

        /// Constructor will have set this to COLOR_WINDOW + 1.
        void SetClassBkgrnd(HBRUSH hbkgrnd) { m_WndClass.hbrBackground = hbkgrnd; }

        void SetClassCursor(HCURSOR hcur) { m_WndClass.hCursor = hcur; }
        void SetClassMenu(size_t idMenuResource) { m_WndClass.lpszMenuName = reinterpret_cast<LPCWSTR>(idMenuResource); }

        /// Name will be converted to UTF16 before storing it in WndClass()
        void SetClassName(std::string_view ClassName);

        /// Constructor will have set this to CS_HREDRAW | CS_VREDRAW.
        void SetClassStyle(DWORD style) { m_WndClass.style = style; }

        void SetWndExtra(int cbExtra) { m_WndClass.cbWndExtra = cbExtra; }
        void SetClsExtra(int cbExtra) { m_WndClass.cbWndExtra = cbExtra; }

        void SetClassIcon(size_t idIcon) { m_WndClass.hIcon = LoadIconW(ttlib::lang_info.hinstResource, (LPCWSTR) idIcon); }
        void SetClassIcon(HICON hIcon) { m_WndClass.hIcon = hIcon; }
        void SetClassSmallIcon(size_t idIcon) { m_WndClass.hIconSm = LoadIconW(ttlib::lang_info.hinstResource, (LPCWSTR) idIcon); }
        void SetClassSmallIcon(HICON hIcon) { m_WndClass.hIconSm = hIcon; }

        /// Title will automatically be converted to UTF16
        bool CreateWnd(const std::string& Title, DWORD dwExStyle, DWORD dwStyle, HWND hwndParent = NULL, RECT* prc = NULL,
                       HMENU hmenu = NULL);

        /// Attaches to a window not created by win, updates m_ClassName, m_hwnd and m_hwndParent.
        // bool AttachWnd(HWND hwnd);

        // if hwnd is NULL, subclass our own window.
        bool SubClass(HWND hwnd = NULL);

        template<typename T_MSG, typename T_WPARAM, typename T_LPARAM>
        LRESULT SendMsg(T_MSG msg, T_WPARAM wParam, T_LPARAM lParam) const
        {
            // C-style case used to let compiler determine which cast is needed for specific parameter type
            return ::SendMessageW(m_hwnd, (UINT) msg, (WPARAM) wParam, (LPARAM) lParam);
        }

        template<typename T_MSG, typename T_WPARAM>
        LRESULT SendMsg(T_MSG msg, T_WPARAM wParam) const
        {
            // C-style case used to let compiler determine which cast is needed for specific parameter type
            return ::SendMessageW(m_hwnd, (UINT) msg, (WPARAM) wParam, NULL);
        }

        template<typename T_MSG>
        LRESULT SendMsg(T_MSG msg) const
        {
            // C-style case used to let compiler determine which cast is needed for specific parameter type
            return ::SendMessageW(m_hwnd, (UINT) msg, NULL, NULL);
        }

        template<typename T_MSG, typename T_WPARAM, typename T_LPARAM>
        LRESULT PostMsg(T_MSG msg, T_WPARAM wParam, T_LPARAM lParam) const
        {
            // C-style case used to let compiler determine which cast is needed for specific parameter type
            return ::PostMessageW(m_hwnd, (UINT) msg, (WPARAM) wParam, (LPARAM) lParam);
        }

        template<typename T_MSG, typename T_WPARAM>
        LRESULT PostMsg(T_MSG msg, T_WPARAM wParam) const
        {
            // C-style case used to let compiler determine which cast is needed for specific parameter type
            return ::PostMessageW(m_hwnd, (UINT) msg, (WPARAM) wParam, NULL);
        }

        template<typename T_MSG>
        LRESULT PostMsg(T_MSG msg) const
        {
            // C-style case used to let compiler determine which cast is needed for specific parameter type
            return ::PostMessageW(m_hwnd, (UINT) msg, NULL, NULL);
        }

        void ShowWindow(int nCmdShow = SW_SHOW) { ::ShowWindow(m_hwnd, nCmdShow); }

        void SetTitle(std::string_view Title);

        // For other Windows functions requiring an HWND parameter, simply pass in *this as the HWND.

        operator HWND() const { return m_hwnd; }

    protected:
        LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

        // BEGIN_TTCMD_MAP in ttcasemap.h will override this.
        virtual bool OnCmdCaseMap(int /* id */, int /* NotifyCode */, LRESULT& /* lResult */) { return false; }

        // BEGIN_TTMSG_MAP in ttcasemap.h will override this.
        virtual bool OnMsgMap(UINT /* msg */, WPARAM /* wParam */, LPARAM /* lParam */, LRESULT& lResult)
        {
            lResult = 0;
            return false;
        }

        void InitWndClass();

        friend LRESULT WINAPI ttlib::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Use protected, not private because other classes inherit from this class, and currently expect access to
        // the class members.
    protected:
        std::wstring m_ClassName;

        WNDCLASSEXW m_WndClass;

        // m_hwnd vs m_hWnd -- SDK/include, ATL and WTL use both variants. We're sticking with all lowercase.

        HWND m_hwnd { NULL };
        HWND m_hwndParent { NULL };

        /// Previous window procedure before it was subclassed.
        WNDPROC m_SubClassProc { nullptr };

        LRESULT m_result;

        bool isRegistered { false };
    };

}  // namespace ttlib
