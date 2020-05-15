/////////////////////////////////////////////////////////////////////////////
// Name:      ttwindlg.h
// Purpose:   Class for displaying a dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <wtypes.h>

#include <sstream>

// This dialog class has no base requirements other than compiling for Windows. It can be used whether your app is
// using ATL, WTL, wxWidgets, or is just a console app that needs a dialog box.

// Classes are also provided for some dialog controls: dlgCombo, dlgListBox, dlgListView

#include <CommCtrl.h>

#include "ttcstr.h"      // cstr -- Classes for handling zero-terminated char strings.
#include "ttdebug.h"     // ttASSERT macros
#include "ttmultibtn.h"  // ttlib::MultiBtn

#ifndef BEGIN_TTMSG_MAP
    #include "ttcasemap.h"  // Macros for mapping Windows messages to functions
#endif

#if !defined(CHECK_DLG_ID)
    #if !defined(NDEBUG)
        /// Place this in OnBegin() to verify a control exists.
        #define CHECK_DLG_ID(id)                                                  \
            if (!::GetDlgItem(*this, id))                                         \
            {                                                                     \
                std::stringstream msg;                                            \
                msg << "Invalid dialog control id: " << #id << " (" << id << ')'; \
                if (ttAssertionMsg(__FILE__, __func__, __LINE__, #id, msg))       \
                {                                                                 \
                    DebugBreak();                                                 \
                }                                                                 \
            }
    #else
        #define CHECK_DLG_ID(id)
    #endif
#endif

namespace ttlib
{
    INT_PTR WINAPI DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

    // Class for displaying a dialog
    class dlg
    {
    public:
        dlg(UINT idTemplate);

        INT_PTR DoModal(HWND hwndParent);
        // You must use returned handle in IsDialogMessage for keys to work in dialog
        HWND DoModeless(HWND hwndParent);

        // called when dialog is initialized, override if you need to do something during dialog initialization
        virtual void OnBegin() {}
        // called when IDOK button is pressed--call CancelEnd() before return to prevent closing the dialog
        virtual void OnOK() {}
        // called when IDCANCEL button is pressed--call CancelEnd() before return to prevent closing the dialog
        virtual void OnCancel() {}

        // Call this in OnBegin to center dialog in owner window or desktop
        void CenterWindow(bool bCenterOnDesktop = false);
        // Call this in OnBegin to convert all buttons in the dialog to 3D shaded buttons
        void EnableShadeBtns();
        // Call this in OnBegin to add an Icon to a 3D shaded button
        void SetBtnIcon(int idBtn, int idIcon, UINT nIconAlign = BS_LEFT);
        // Call this in OnBegin to add an Icon to a 3D shaded button
        void SetBtnIcon(int idBtn, const std::string& IconName, UINT nIconAlign = BS_LEFT);

        // Call within OnEnd() to cancel ending the dialog
        void CancelEnd() { m_isCancelEnd = true; }
        BOOL CloseDialog(INT_PTR result = IDOK) { return (m_isModeless ? DestroyWindow(m_hwnd) : ::EndDialog(m_hwnd, result)); }

        template<typename T_ID>
        /// Get the window handle of a control
        HWND gethwnd(T_ID id) const
        {
            return ::GetDlgItem(m_hwnd, (int) id);
        }

        // The following get/set functions automatically convert to/from UTF16/UTF8 allowing calls to the
        // UNICODE versions of the Windows API while still using. 8-bit UTF8 strings

        template<typename T_ID>
        bool GetControlText(T_ID id, std::string& str)
        {
            return ttlib::GetWndText(gethwnd(id), str);
        }

        template<typename T_ID>
        ttlib::cstr GetControlText(T_ID id)
        {
            return ttlib::GetWndText(gethwnd(id));
        }

        template<typename T_ID>
        int GetControlTextLength(T_ID id) const
        {
            return ::GetWindowTextLengthW(gethwnd(id));
        }

        template<typename T_ID>
        void SetControlText(T_ID id, std::string_view utf8str)
        {
            ttlib::SetWndText(gethwnd(id), utf8str);
        }

        void SetDlgTitle(std::string_view utf8str) { ttlib::SetWndText(m_hwnd, utf8str); }

        template<typename T_ID>
        BOOL GetControlRect(T_ID id, RECT* prc) const
        {
            return ::GetWindowRect(gethwnd(id), prc);
        }

        template<typename T_ID>
        int GetControlInteger(T_ID id) const
        {
            char buffer[32];
            if (::GetWindowTextA(gethwnd(id), buffer, sizeof(buffer) - 1) == 0)
                return -1;
            return ttlib::atoi(buffer);
        }

        template<typename T_ID, typename T_VAL>
        void SetControlInteger(T_ID id, T_VAL val) const
        {
            std::stringstream str;
            str << val;
            SetWindowTextA(gethwnd(id), str.str().c_str());
        }

        template<typename T_ID>
        void EnableControl(T_ID id, BOOL isEnable = TRUE) const
        {
            ::EnableWindow(gethwnd(id), isEnable);
        }

        template<typename T_ID>
        void DisableControl(T_ID id) const
        {
            ::EnableWindow(gethwnd(id), FALSE);
        }

        template<typename T_ID>
        void ShowControl(T_ID id) const
        {
            ::ShowWindow(gethwnd(id), SW_SHOW);
        }

        template<typename T_ID>
        void HideControl(T_ID id) const
        {
            ::ShowWindow(gethwnd(id), SW_HIDE);
        }

        template<typename T_ID>
        bool GetCheck(T_ID id) const
        {
            return (SendItemMsg(id, BM_GETCHECK) == BST_CHECKED);
        }

        template<typename T_ID>
        void GetCheck(T_ID id, bool& checked) const
        {
            checked =  (SendItemMsg(id, BM_GETCHECK) == BST_CHECKED);
        }

        template<typename T_ID>
        bool IsChecked(T_ID id) const
        {
            return GetCheck(id);
        }

        template<typename T_ID>
        void SetCheck(T_ID id, bool checked = true) const
        {
            SendItemMsg(id, BM_SETCHECK, checked);
        }

        template<typename T_ID>
        void UnCheck(T_ID id) const
        {
            SendItemMsg(id, BM_SETCHECK, FALSE);
        }

        template<typename T_ID, typename T_MSG, typename T_WPARAM, typename T_LPARAM>
        LRESULT SendItemMsg(T_ID id, T_MSG msg, T_WPARAM wParam, T_LPARAM lParam) const
        {
            return ::SendMessage(gethwnd(id), (UINT) msg, (WPARAM) wParam, (LPARAM) lParam);
        }

        template<typename T_ID, typename T_MSG, typename T_WPARAM>
        LRESULT SendItemMsg(T_ID id, T_MSG msg, T_WPARAM wParam) const
        {
            return ::SendMessage(gethwnd(id), (UINT) msg, (WPARAM) wParam, NULL);
        }

        template<typename T_ID, typename T_MSG>
        LRESULT SendItemMsg(T_ID id, T_MSG msg) const
        {
            return ::SendMessage(gethwnd(id), (UINT) msg, NULL, NULL);
        }

        template<typename T_ID, typename T_MSG, typename T_WPARAM, typename T_LPARAM>
        LRESULT PostItemMsg(T_ID id, T_MSG msg, T_WPARAM wParam, T_LPARAM lParam) const
        {
            return ::PostMessage(gethwnd(id), (UINT) msg, (WPARAM) wParam, (LPARAM) lParam);
        }

        template<typename T_ID, typename T_MSG, typename T_WPARAM>
        LRESULT PostItemMsg(T_ID id, T_MSG msg, T_WPARAM wParam) const
        {
            return ::PostMessage(gethwnd(id), (UINT) msg, (WPARAM) wParam, NULL);
        }

        template<typename T_ID, typename T_MSG>
        LRESULT PostItemMsg(T_ID id, T_MSG msg) const
        {
            return ::PostMessage(gethwnd(id), (UINT) msg, NULL, NULL);
        }

        template<typename T_MSG, typename T_WPARAM, typename T_LPARAM>
        LRESULT SendMsg(T_MSG msg, T_WPARAM wParam, T_LPARAM lParam) const
        {
            return ::SendMessage(m_hwnd, (UINT) msg, (WPARAM) wParam, (LPARAM) lParam);
        }

        template<typename T_MSG, typename T_WPARAM>
        LRESULT SendMsg(T_MSG msg, T_WPARAM wParam) const
        {
            return ::SendMessage(m_hwnd, (UINT) msg, (WPARAM) wParam, NULL);
        }

        template<typename T_MSG>
        LRESULT SendMsg(T_MSG msg) const
        {
            return ::SendMessage(m_hwnd, (UINT) msg, NULL, NULL);
        }

        template<typename T_MSG, typename T_WPARAM, typename T_LPARAM>
        LRESULT PostMsg(T_MSG msg, T_WPARAM wParam, T_LPARAM lParam) const
        {
            return ::PostMessage(m_hwnd, (UINT) msg, (WPARAM) wParam, (LPARAM) lParam);
        }

        template<typename T_MSG, typename T_WPARAM>
        LRESULT PostMsg(T_MSG msg, T_WPARAM wParam) const
        {
            return ::PostMessage(m_hwnd, (UINT) msg, (WPARAM) wParam, NULL);
        }

        template<typename T_MSG>
        LRESULT PostMsg(T_MSG msg) const
        {
            return ::PostMessage(m_hwnd, (UINT) msg, NULL, NULL);
        }

        template<typename T_HANDLE>
        HICON SetIcon(T_HANDLE hIcon, BOOL BigIcon = TRUE)
        {
            return reinterpret_cast<HICON>(SendMsg(WM_SETICON, BigIcon, hIcon));
        }

        template<typename T_ID>
        void SetFocus(T_ID idControl) const
        {
            ::SetFocus(gethwnd(idControl));
        }

        void EndDialog(INT_PTR nResult = IDCANCEL) const { ::EndDialog(m_hwnd, nResult); }
        void FadeWindow();

        HWND GetParent() { return m_hwndParent; }
        operator HWND() const { return m_hwnd; }

    protected:
        // BEGIN_TTCMD_MAP in ttcasemap.h will override this
        virtual bool OnCmdCaseMap(int /* id */, int /* NotifyCode */, LRESULT& /* lResult */) { return false; }

        // BEGIN_TTMSG_MAP in ttcasemap.h will override this
        virtual bool OnMsgMap(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, LRESULT& lResult)
        {
            lResult = 0;
            return false;
        }

        friend INT_PTR WINAPI ttlib::DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

        // Class members

        // m_hwnd vs m_hWnd -- SDK/include, ATL and WTL use both variants. We're sticking with all
        // lowercase.
        HWND m_hwnd { NULL };

        HWND m_hwndParent { NULL };

        ttlib::MultiBtn* m_pShadedBtns { nullptr };

        int m_idTemplate { 0 };

        bool m_isCancelEnd { false };
        bool m_isInitializing { false };
        bool m_isModeless { false };
    };  // end of dlg

    class dlgCombo
    {
    public:
        dlgCombo() { m_hwnd = NULL; }
        dlgCombo(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
        dlgCombo(HWND hwndParent, int id)
        {
            m_hwnd = ::GetDlgItem(hwndParent, id);
            ttASSERT_MSG(m_hwnd, "Invalid Combo-box id");
        }

        void Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
        void Initialize(int id)
        {
            ttASSERT(m_hwnd);
            m_hwnd = ::GetDlgItem(GetParent(m_hwnd), id);
        }
        void Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); }

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

        void Enable(BOOL isEnable = TRUE) const { EnableWindow(m_hwnd, isEnable); }

        bool GetText(std::string& str) { return ttlib::GetWndText(m_hwnd, str); }
        ttlib::cstr GetText() { return ttlib::GetWndText(m_hwnd); }

        template<typename T_INDEX>
        bool GetLBText(T_INDEX index, std::string& str)
        {
            return ttlib::GetComboLBText(m_hwnd, (WPARAM) index, str);
        }

        template<typename T_INDEX>
        ttlib::cstr GetLBText(T_INDEX index)
        {
            return ttlib::GetComboLBText(m_hwnd, (WPARAM) index);
        }

        void SetText(std::string_view str) { ttlib::SetWndText(m_hwnd, str); }

        LRESULT append(std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(CB_ADDSTRING, 0, str16.c_str());
        }

        template<typename T_INDEX>
        LRESULT insert(T_INDEX index, std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(CB_INSERTSTRING, index, str16.c_str());
        }

        LRESULT find(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(CB_FINDSTRINGEXACT, iStart, str16.c_str());
        }

        LRESULT findprefix(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(CB_FINDSTRING, iStart, str16.c_str());
        }

        LRESULT select(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(CB_SELECTSTRING, iStart, str16.c_str());
        }

        LRESULT size() const { return SendMsg(CB_GETCOUNT); }
        LRESULT clear() const { return SendMsg(CB_RESETCONTENT); }

        LRESULT GetCount() const { return SendMsg(CB_GETCOUNT); }
        void ResetContent() const { SendMsg(CB_RESETCONTENT); }
        void Reset() const { SendMsg(CB_RESETCONTENT); }

        template<typename T_INDEX>
        LRESULT DeleteString(T_INDEX index) const
        {
            return SendMsg(CB_DELETESTRING, index);
        }

        template<typename T_INDEX>
        LRESULT GetItemData(T_INDEX index) const
        {
            return SendMsg(CB_GETITEMDATA, index);
        }

        template<typename T_INDEX>
        LRESULT SetItemData(T_INDEX index, LPARAM data) const
        {
            return SendMsg(CB_SETITEMDATA, index, data);
        }

        LRESULT GetCurSel() const { return SendMsg(CB_GETCURSEL); }

        LRESULT SetCurSel() const { return SendMsg(CB_SETCURSEL, 0); }

        template<typename T_INDEX>
        LRESULT SetCurSel(T_INDEX index) const
        {
            return SendMsg(CB_SETCURSEL, index);
        }

        LRESULT GetEditSel(DWORD* pStart, DWORD* pEnd) const { return SendMsg(CB_GETEDITSEL, (WPARAM) pStart, (LPARAM) pEnd); }
        void SetEditSel(int iStart, int iEnd) const { (void) SendMsg(CB_SETEDITSEL, 0, MAKELPARAM(iStart, iEnd)); }
        void SelectEditContol(void) const { SendMsg(CB_SETEDITSEL, 0, MAKELPARAM(0, -1)); }

        void SetFont(HFONT hfont) { SendMsg(WM_SETFONT, (WPARAM) hfont); }

        void InValidate(BOOL erase = TRUE)
        {
            ttASSERT(m_hwnd);
            InvalidateRect(m_hwnd, nullptr, erase);
        }

        void operator+=(std::string_view str) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            SendMsg(CB_ADDSTRING, 0, str16.c_str());
        }

        void operator+=(const std::wstring& str16) const
        {
            if (!str16.empty())
                SendMsg(CB_ADDSTRING, 0, str16.c_str());
        }

        operator HWND() const { return m_hwnd; }

        HWND m_hwnd;
    };  // end of dlgCombo

    class dlgListBox
    {
    public:
        dlgListBox() { m_hwnd = NULL; }
        dlgListBox(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
        dlgListBox(HWND hwndParent, int id)
        {
            m_hwnd = GetDlgItem(hwndParent, id);
            ttASSERT_MSG(m_hwnd, "Invalid Listbox id");
        }

        void Initialize(int id)
        {
            ttASSERT(m_hwnd);
            m_hwnd = GetDlgItem(GetParent(m_hwnd), id);
        }
        void Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); }
        void Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }

        bool GetText(std::string& str) { return ttlib::GetWndText(m_hwnd, str); }

        template<typename T_INDEX>
        bool GetLBText(T_INDEX index, std::string& str)
        {
            return ttlib::GetListboxText(m_hwnd, (WPARAM) index, str);
        }

        ttlib::cstr GetText() { return ttlib::GetWndText(m_hwnd); }

        template<typename T_INDEX>
        ttlib::cstr GetLBText(T_INDEX index)
        {
            return ttlib::GetListboxText(m_hwnd, (WPARAM) index);
        }

        void SetText(std::string_view str) { ttlib::SetWndText(m_hwnd, str); }

        LRESULT append(std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(LB_ADDSTRING, 0, str16.c_str());
        }

        template<typename T_INDEX>
        LRESULT insert(T_INDEX index, std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(LB_INSERTSTRING, index, str16.c_str());
        }

        LRESULT find(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(LB_FINDSTRINGEXACT, iStart, str16.c_str());
        }

        LRESULT findprefix(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(LB_FINDSTRING, iStart, str16.c_str());
        }

        LRESULT select(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMsg(LB_SELECTSTRING, iStart, str16.c_str());
        }

        LRESULT size() const { return SendMsg(LB_GETCOUNT); }
        LRESULT clear() const { return SendMsg(LB_RESETCONTENT); }

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

        void Enable(BOOL enable = TRUE) const { EnableWindow(m_hwnd, enable); }

        LRESULT GetCount() const { return SendMsg(LB_GETCOUNT); }
        LRESULT GetSelCount() const
        {
            ttASSERT((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)));
            return SendMsg(LB_GETSELCOUNT);
        }
        void ResetContent() const { SendMsg(LB_RESETCONTENT); }
        void Reset() const { SendMsg(LB_RESETCONTENT); }

        void SetCount(int cItems)
        {
#if !defined(NDEBUG)  // Starts debug section.
            LRESULT result =
#endif
                SendMsg(LB_SETCOUNT, cItems);
            ttASSERT_MSG(result != LB_ERR, "SetCount failed. Does listbox have LBS_NODATA style?");
        }

        void SetFont(HFONT hfont, bool redraw = false) { SendMsg(WM_SETFONT, hfont, (redraw ? TRUE : FALSE)); }

        template<typename T_INDEX>
        LRESULT DeleteString(T_INDEX index) const
        {
            return SendMsg(LB_DELETESTRING, index);
        }

        // void  RemoveListItem();  // removes currently selected item

        template<typename T_INDEX>
        LRESULT GetItemData(T_INDEX index) const
        {
            return SendMsg(LB_GETITEMDATA, index);
        }

        template<typename T_INDEX>
        LRESULT SetItemData(T_INDEX index, int data) const
        {
            return SendMsg(LB_SETITEMDATA, index, data);
        }

        LRESULT GetItemRect(RECT* prc) const { return SendMsg(LB_GETITEMRECT, GetCurSel(), prc); }

        template<typename T_INDEX>
        LRESULT GetItemRect(RECT* prc, T_INDEX index) const
        {
            return SendMsg(LB_GETITEMRECT, index, prc);
        }

        // Works on single selection listbox only
        LRESULT GetCurSel() const
        {
            ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "GetCurSel() only works on single selection listbox");
            return SendMsg(LB_GETCURSEL);
        }

        // Works on single selection listbox only
        LRESULT SetCurSel() const
        {
            ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "SetCurSel() only works on single selection listbox");
            return SendMsg(LB_SETCURSEL, 0);
        }

        template<typename T_INDEX>
        // Works on single selection listbox only
        LRESULT SetCurSel(T_INDEX index) const
        {
            ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "SetCurSel() only works on single selection listbox");
            return SendMsg(LB_SETCURSEL, index);
        }

        LRESULT GetTopIndex(void) const { return SendMsg(LB_GETTOPINDEX); }

        template<typename T_INDEX>
        void SetTopIndex(T_INDEX index) const
        {
            SendMsg(LB_SETTOPINDEX, index);
        }

        template<typename T_INDEX>
        // For multi-select list boxes
        LRESULT GetSel(T_INDEX index) const
        {
            ttASSERT_MSG((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "GetSel() only works on multiple-select list box");
            return SendMsg(LB_GETSEL, index);
        }

        template<typename T_INDEX>
        void SetSel(T_INDEX index, BOOL select = TRUE) const
        {
            ttASSERT_MSG((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "SetSel() only works on multiple-select list box");
            SendMsg(LB_SETSEL, select, MAKELPARAM(index, 0));
        }

        void InValidate(BOOL erase = TRUE) { InvalidateRect(m_hwnd, NULL, erase); }
        void DisableRedraw(void) { SendMsg(WM_SETREDRAW, FALSE); }
        void EnableRedraw(void) { SendMsg(WM_SETREDRAW, TRUE); }

        operator HWND() const { return m_hwnd; }

        void operator+=(std::string_view str) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            SendMsg(LB_ADDSTRING, 0, str16.c_str());
        }

        HWND m_hwnd;
    };  // end of dlgListBox

    class dlgListView
    {
    public:
        dlgListView() { m_hwnd = NULL; }
        dlgListView(HWND hwndParent, int id)
        {
            m_hwnd = ::GetDlgItem(hwndParent, id);
            ttASSERT_MSG(m_hwnd, "Invalid Listview id");
        }

        void Initialize(int id)
        {
            ttASSERT(m_hwnd);
            m_hwnd = GetDlgItem(GetParent(m_hwnd), id);
        };
        void Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); };
        void Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }

        int GetCurSel() { return static_cast<int>(::SendMessage(m_hwnd, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0))); }

        LRESULT SetCurSel(std::string_view item);

        template<typename T_INDEX>
        LRESULT SetCurSel(T_INDEX pos)
        {
            return SetSel((WPARAM) pos);
        }

        LRESULT SetSel(WPARAM index);

        template<typename T_INDEX>
        ttlib::cstr GetItemText(T_INDEX item)
        {
            return GetLVText((int) item, 0, 1024);
        }

        template<typename T_INDEX>
        ttlib::cstr GetItemText(T_INDEX item, T_INDEX subitem)
        {
            return GetLVText((int) item, (int) subitem, 1024);
        }

        template<typename T_INDEX, typename T_SIZE>
        ttlib::cstr GetItemText(T_INDEX item, T_INDEX subitem, T_SIZE maxTextLen)
        {
            return GetLVText((int) item, (int) subitem, (int) maxTextLen);
        }

        ttlib::cstr GetLVText(int item, int subitem = 0, int maxTextLen = 1024);

        bool GetItem(LVITEM* pItem) { return ::SendMessage(m_hwnd, LVM_GETITEM, 0, (LPARAM) pItem) ? true : false; }
        bool SetItem(LVITEM* pItem) { return ::SendMessage(m_hwnd, LVM_SETITEM, 0, (LPARAM) pItem) ? true : false; }

        LRESULT InsertItem(LVITEM* pitem) { return ::SendMessage(m_hwnd, LVM_INSERTITEM, 0, (LPARAM) pitem); }

        template<typename T_INDEX>
        BOOL DeleteItem(T_INDEX index)
        {
            return (BOOL)::SendMessage(m_hwnd, LVM_DELETEITEM, (WPARAM) index, 0);
        }
        void clear() const { ::SendMessage(m_hwnd, LVM_DELETEALLITEMS, 0, 0); }

        int add(std::string_view str, LPARAM lparam = -1);

        template<typename T_INDEX>
        bool addsubstring(std::string_view str, T_INDEX iItem, T_INDEX iSubItem)
        {
            return addsub(str, (int) iItem, (int) iSubItem);
        }

        bool addsub(std::string_view str, int iItem, int iSubItem);

        void InsertColumn(int iColumn, std::string_view utf8str, int width = -1);
        void InsertColumn(int iColumn, const std::wstring& str, int width = -1);

        void SetColumnWidth(int col, int width = LVSCW_AUTOSIZE_USEHEADER)
        {
            ::SendMessage(m_hwnd, LVM_SETCOLUMNWIDTH, (WPARAM) col, MAKELPARAM(width, 0));
        }

        HWND GetHWND() const { return m_hwnd; }

        operator HWND() const { return m_hwnd; }

        HWND m_hwnd;
    };  // end of dlgListView

}  // namespace ttlib
