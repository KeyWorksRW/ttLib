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

// This dialog class has no base requirements other than compiling for Windows. It can be used whether your app is
// using ATL, WTL, wxWidgets, or is just a console app that needs a dialog box.

// Classes are also provided for some dialog controls: dlgCombo, dlgListBox, dlgListView

#include <CommCtrl.h>

#include "ttdebug.h"        // ttASSERT macros
#include "ttlibspace.h"     // Contains the ttlib namespace functions/declarations common to all ttLib libraries
#include "ttmultibtn.h"     // ttCMultiBtn

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
}

namespace ttlib
{
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
        BOOL CloseDialog(INT_PTR result = IDOK)
        {
            return (m_isModeless ? DestroyWindow(*this) : ::EndDialog(*this, result));
        }

        HWND gethwnd(int id) const { return ::GetDlgItem(m_hwnd, id); }

        // The following get/set functions automatically convert to/from UTF16/UTF8 allowing calls to the
        // UNICODE versions of the Windows API while still using. 8-bit UTF8 strings

        bool GetControlText(int id, std::string& str) { return ttlib::GetWndText(gethwnd(id), str); }
        std::string GetControlText(int id) { return ttlib::GetWndText(gethwnd(id)); }
        int GetControlTextLength(int id) const { return ::GetWindowTextLengthW(gethwnd(id)); }

        void SetControlText(int id, std::string_view utf8str) { ttlib::SetWndText(gethwnd(id), utf8str); }

        void SetDlgTitle(std::string_view utf8str) { ttlib::SetWndText(*this, utf8str); }

        BOOL GetControlRect(int id, RECT* prc) const { return ::GetWindowRect(gethwnd(id), prc); }

        int GetControlInteger(int id) const;
        void SetControlInteger(int id, int val) const;

        void EnableControl(int id, BOOL isEnable = TRUE) const { (void) ::EnableWindow(gethwnd(id), isEnable); }
        void DisableControl(int id) const { (void) ::EnableWindow(gethwnd(id), FALSE); }

        void ShowControl(int id) const { ::ShowWindow(gethwnd(id), SW_SHOW); }
        void HideControl(int id) const { ::ShowWindow(gethwnd(id), SW_HIDE); }

        bool GetCheck(int id) const { return (SendItemMsg(id, BM_GETCHECK) == BST_CHECKED); }
        bool IsChecked(int id) const { return GetCheck(id); }
        void SetCheck(int id, BOOL bCheck = TRUE) const { (void) SendItemMsg(id, BM_SETCHECK, bCheck); }
        void UnCheck(int id) const { (void) SendItemMsg(id, BM_SETCHECK, FALSE); }

        HICON SetIcon(HICON hIcon, BOOL bBigIcon = TRUE)
        {
            ttASSERT(::IsWindow(m_hwnd));
            return (HICON)::SendMessageA(m_hwnd, WM_SETICON, bBigIcon, (LPARAM) hIcon);
        }

        LRESULT SendItemMsg(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            return ::SendMessageA(gethwnd(id), msg, wParam, lParam);
        }
        LRESULT PostItemMsg(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            return ::PostMessageA(gethwnd(id), msg, wParam, lParam);
        }

        LRESULT SendMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            return ::SendMessageA(*this, msg, wParam, lParam);
        }
        LRESULT PostMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            return ::PostMessageA(*this, msg, wParam, lParam);
        }

        void SetFocus(int idControl) const { ::SetFocus(gethwnd(idControl)); }

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

        ttCMultiBtn* m_pShadedBtns { nullptr };

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

        LRESULT SendMessageA(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            ttASSERT(m_hwnd);
            return ::SendMessageA(m_hwnd, msg, wParam, lParam);
        }
        LRESULT SendMessageW(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            ttASSERT(m_hwnd);
            return ::SendMessageW(m_hwnd, msg, wParam, lParam);
        }

        void Enable(BOOL isEnable = TRUE) const { EnableWindow(m_hwnd, isEnable); }

        bool GetText(std::string& str) { return ttlib::GetWndText(*this, str); }
        bool GetLBText(LRESULT index, std::string& str) { return ttlib::GetComboLBText(*this, index, str); }
        void SetText(std::string_view str) { ttlib::SetWndText(*this, str); }

        LRESULT append(std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(CB_ADDSTRING, 0, (LPARAM) str16.c_str());
        }
        LRESULT insert(int index, std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(CB_INSERTSTRING, (WPARAM) index, (LPARAM) str16.c_str());
        }

        LRESULT find(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(CB_FINDSTRINGEXACT, (WPARAM) iStart, (LPARAM) str16.c_str());
        }

        LRESULT findprefix(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(CB_FINDSTRING, (WPARAM) iStart, (LPARAM) str16.c_str());
        }

        LRESULT select(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(CB_SELECTSTRING, (WPARAM) iStart, (LPARAM) str16.c_str());
        }

        LRESULT size() const { return SendMessage(CB_GETCOUNT); }
        LRESULT clear() const { return SendMessage(CB_RESETCONTENT); }

        LRESULT GetCount() const { return SendMessage(CB_GETCOUNT); }
        void ResetContent() const { SendMessage(CB_RESETCONTENT); }
        void Reset() const { SendMessage(CB_RESETCONTENT); }

        LRESULT DeleteString(WPARAM index) const { return SendMessage(CB_DELETESTRING, index); }

        LRESULT GetItemData(WPARAM index) const { return SendMessage(CB_GETITEMDATA, index); }
        LRESULT SetItemData(WPARAM index, LPARAM data) const { return SendMessage(CB_SETITEMDATA, index, data); }

        LRESULT GetCurSel() const { return SendMessage(CB_GETCURSEL); }
        LRESULT SetCurSel(WPARAM index = 0) const { return SendMessage(CB_SETCURSEL, index); }
        LRESULT GetEditSel(DWORD* pStart, DWORD* pEnd) const
        {
            return SendMessage(CB_GETEDITSEL, (WPARAM) pStart, (LPARAM) pEnd);
        }
        void SetEditSel(int iStart, int iEnd) const
        {
            (void) SendMessage(CB_SETEDITSEL, 0, MAKELPARAM(iStart, iEnd));
        }
        void SelectEditContol(void) const { SendMessage(CB_SETEDITSEL, 0, MAKELPARAM(0, -1)); }

        void SetFont(HFONT hfont) { SendMessageA(WM_SETFONT, (WPARAM) hfont); }

        void InValidateAlloc(BOOL isErase = TRUE)
        {
            ttASSERT(m_hwnd);
            InvalidateRect(m_hwnd, nullptr, isErase);
        }

        void operator+=(const char* psz) const
        {
            ttASSERT(psz);
            if (psz)
            {
                std::string_view vstr(psz);
                std::wstring str16;
                ttlib::utf8to16(vstr, str16);
                (void) ::SendMessageW(m_hwnd, CB_ADDSTRING, 0, (LPARAM) str16.c_str());
            }
        }
        void operator+=(const wchar_t* pwsz) const
        {
            ttASSERT(pwsz);
            if (pwsz)
                SendMessageW(CB_ADDSTRING, 0, (LPARAM) pwsz);
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

        bool GetText(std::string& str) { return ttlib::GetWndText(*this, str); }
        bool GetLBText(LRESULT index, std::string& str) { return ttlib::GetListboxText(*this, index, str); }
        void SetText(std::string_view str) { ttlib::SetWndText(*this, str); }

        LRESULT append(std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(LB_ADDSTRING, 0, (LPARAM) str16.c_str());
        }
        LRESULT insert(int index, std::string_view str)
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(LB_INSERTSTRING, (WPARAM) index, (LPARAM) str16.c_str());
        }

        LRESULT find(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(LB_FINDSTRINGEXACT, (WPARAM) iStart, (LPARAM) str16.c_str());
        }

        LRESULT findprefix(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(LB_FINDSTRING, (WPARAM) iStart, (LPARAM) str16.c_str());
        }

        LRESULT select(std::string_view str, int iStart = -1) const
        {
            std::wstring str16;
            ttlib::utf8to16(str, str16);
            return SendMessageW(LB_SELECTSTRING, (WPARAM) iStart, (LPARAM) str16.c_str());
        }

        LRESULT size() const { return SendMessage(LB_GETCOUNT); }
        LRESULT clear() const { return SendMessage(LB_RESETCONTENT); }

        LRESULT SendMessageA(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            return ::SendMessageA(m_hwnd, msg, wParam, lParam);
        }
        LRESULT SendMessageW(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const
        {
            ttASSERT(m_hwnd);
            return ::SendMessageW(m_hwnd, msg, wParam, lParam);
        }

        void Enable(BOOL isEnable = TRUE) const { EnableWindow(m_hwnd, isEnable); }

        LRESULT GetCount() const { return SendMessage(LB_GETCOUNT); }
        LRESULT GetSelCount() const
        {
            ttASSERT((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)));
            return SendMessage(LB_GETSELCOUNT);
        }
        void ResetContent() const { SendMessage(LB_RESETCONTENT); }
        void Reset() const { SendMessage(LB_RESETCONTENT); }
        void SetCount(int cItems)
        {
#if !defined(NDEBUG)  // Starts debug section.
            LRESULT result =
#endif
                SendMessage(LB_SETCOUNT, (WPARAM) cItems);
            ttASSERT_MSG(result != LB_ERR, "SetCount failed. Does listbox have LBS_NODATA style?");
        }

        void SetFont(HFONT hfont, bool fRedraw = false)
        {
            SendMessageA(WM_SETFONT, (WPARAM) hfont, (LPARAM)(fRedraw ? TRUE : FALSE));
        }

        LRESULT DeleteString(int index) const { return SendMessage(LB_DELETESTRING, index); }

        // void  RemoveListItem();  // removes currently selected item

        LRESULT GetItemData(WPARAM index) const { return SendMessage(LB_GETITEMDATA, index); }
        LRESULT SetItemData(WPARAM index, int data) const { return SendMessage(LB_SETITEMDATA, index, data); }

        LRESULT GetItemRect(RECT* prc, WPARAM index = (WPARAM) -1) const
        {
            return SendMessage(LB_GETITEMRECT, ((index == (WPARAM) -1) ? GetCurSel() : index), (LPARAM) prc);
        }

        // Works on single selection listbox only
        LRESULT GetCurSel() const
        {
            ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "GetCurSel() only works on single selection listbox");
            return SendMessage(LB_GETCURSEL);
        }

        // Works on single selection listbox only
        LRESULT SetCurSel(WPARAM index = 0) const
        {
            ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "SetCurSel() only works on single selection listbox");
            return SendMessage(LB_SETCURSEL, index);
        }
        LRESULT GetTopIndex(void) const { return SendMessage(LB_GETTOPINDEX); }
        void SetTopIndex(WPARAM index) const { (void) SendMessage(LB_SETTOPINDEX, index); }

        // For multi-select list boxes
        LRESULT GetSel(WPARAM index) const
        {
            ttASSERT_MSG((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "GetSel() only works on multiple-select list box");
            return SendMessage(LB_GETSEL, index);
        }
        void SetSel(int index, BOOL isSelect = TRUE) const
        {
            ttASSERT_MSG((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)),
                         "SetSel() only works on multiple-select list box");
            (void) SendMessage(LB_SETSEL, isSelect, MAKELPARAM(index, 0));
        }

        void InValidateAlloc(BOOL isErase = TRUE) { InvalidateRect(m_hwnd, NULL, isErase); }
        void DisableRedraw(void) { SendMessage(WM_SETREDRAW, FALSE); }
        void EnableRedraw(void) { SendMessage(WM_SETREDRAW, TRUE); }

        operator HWND() const { return m_hwnd; }
        void operator+=(const char* psz) const
        {
            ttASSERT(psz);
            if (!psz)
                return;
            std::string_view vstr(psz);
            std::wstring str16;
            ttlib::utf8to16(vstr, str16);
            (void) ::SendMessageW(m_hwnd, LB_ADDSTRING, 0, (LPARAM) str16.c_str());
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

        LRESULT GetCurSel()
        {
            return SendMessage(m_hwnd, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0));
        }
        LRESULT SetCurSel(int pos);
        LRESULT SetCurSel(const char* pszItem);

        bool GetItem(LVITEMA* pItem)
        {
            return SendMessageA(m_hwnd, LVM_GETITEM, 0, (LPARAM) pItem) ? true : false;
        }
        bool SetItem(LVITEMA* pItem)
        {
            return SendMessageA(m_hwnd, LVM_SETITEMA, 0, (LPARAM) pItem) ? true : false;
        }
        bool SetItem(LVITEMW* pItem)
        {
            return SendMessageW(m_hwnd, LVM_SETITEMW, 0, (LPARAM) pItem) ? true : false;
        }

        LRESULT InsertItem(LVITEMA* pitem)
        {
            return ::SendMessageA(m_hwnd, LVM_INSERTITEMA, 0, (LPARAM) pitem);
        }
        LRESULT InsertItemW(LVITEMW* pitem)
        {
            return ::SendMessageW(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) pitem);
        }
        BOOL DeleteItem(int index) { return (BOOL) ListView_DeleteItem(m_hwnd, index); }
        void Reset() const { ListView_DeleteAllItems(m_hwnd); }

        int add(std::string_view str, LPARAM lparam = -1);
        BOOL addsubstring(std::string_view str, int iItem, int iSubItem);

        void InsertColumn(int iColumn, std::string_view utf8str, int width = -1);
        void InsertColumn(int iColumn, const std::wstring& str, int width = -1);

        void SetColumnWidth(int col, int width = LVSCW_AUTOSIZE_USEHEADER)
        {
            ListView_SetColumnWidth(m_hwnd, col, width);
        }

        HWND GetHWND() const { return m_hwnd; }

        operator HWND() const { return m_hwnd; }

        HWND m_hwnd;
    };  // end of dlgListView

}  // namespace ttlib
