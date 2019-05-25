/////////////////////////////////////////////////////////////////////////////
// Name:      ttCDlg
// Purpose:   Class for displaying a modal dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTCDLG_H__
#define __TTLIB_TTCDLG_H__

#ifndef _WINDOWS_
    #error This code will only work on Windows
#endif

// This dialog class has no base requirements other than compiling for Windows. It can be used whether your app is using
// ATL, WTL, wxWidgets, or is just a console app that needs a dialog box.

// Classes are also provided for some dialog controls: ttCComboBox, ttCListBox, ttCListView

#include <CommCtrl.h>

#include "ttdebug.h"    // ttASSERT macros
#include "ttstr.h"      // ttCStr
#include "ttwstr.h"     // ttCWStr
#include "ttmultibtn.h" // ttCMultiBtn

#ifndef BEGIN_TTMSG_MAP
    #include "ttcasemap.h"  // Macros for mapping Windows messages to functions
#endif

#ifndef __DLG_ID__
    #ifdef _DEBUG
        #define DLG_ID(id) tt::CheckItemID(*this, id, #id, __FILE__, __func__, __LINE__)
    #else
        #define DLG_ID(id) id
    #endif
#endif

namespace ttpriv {
    INT_PTR WINAPI DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
}

class ttCDlg
{
public:
    ttCDlg(UINT idTemplate);

    INT_PTR DoModal(HWND hwndParent);
    HWND    DoModeless(HWND hwndParent);    // Must use returned handle in IsDialogMessage for keys to work in dialog

    virtual void OnBegin() { }  // called when dialog is initialized, override if you need to do something during dialog initialization
    virtual void OnOK() { }     // called when IDOK button is pressed--call CancelEnd() before return to prevent closing the dialog
    virtual void OnCancel() { } // called when IDCANCEL button is pressed--call CancelEnd() before return to prevent closing the dialog

    void CenterWindow(bool bCenterOnDesktop = false);   // Call this in OnBegin to center dialog in owner window or desktop
    void EnableShadeBtns();                             // call this in OnBegin to convert all buttons in the dialog to 3D shaded buttons
    void SetBtnIcon(int idBtn, int idIcon, UINT nIconAlign = BS_LEFT);  // call this in OnBegin to add an Icon to a 3D shaded button

    void CancelEnd() { m_bCancelEnd = true; } // call within OnEnd() to cancel ending the dialog
    BOOL CloseDialog(size_t result = IDOK) { return (m_bModeless ? DestroyWindow(*this) : ::EndDialog(*this, (int) result)); }

    HWND GetDlgItem(int id) const { return ::GetDlgItem(m_hwnd, (int) id); }
    int  GetControlTextLength(int id) const { return ::GetWindowTextLength(GetDlgItem(id)); }
    BOOL GetControlRect(int id, RECT* prc) const { return ::GetWindowRect(GetDlgItem(id), prc); }

    void GetControlText(int id, char* pszText, int cchMax = MAX_PATH) const { (void) ::GetWindowTextA(GetDlgItem(id), pszText, cchMax); }
    void GetControlText(int id, ttCStr* pcsz) const { pcsz->GetWindowText(GetDlgItem(id)); }
    void SetControlText(int id, const char* pszText) const { ttASSERT(pszText); (void) ::SetWindowTextA(GetDlgItem(id), pszText); }

    void GetControlText(int id, wchar_t* pwszText, int cchMax = MAX_PATH) const { (void) ::GetWindowTextW(GetDlgItem(id), pwszText, cchMax); }
    void GetControlText(int id, ttCWStr* pcsz) const { pcsz->GetWindowText(GetDlgItem(id)); }
    void SetControlText(int id, const wchar_t* pwszText) const { ttASSERT(pwszText); (void) ::SetWindowTextW(GetDlgItem(id), pwszText); }

    void SetTitle(const char* pszTitle) { ::SetWindowTextA(*this, pszTitle); }

    ptrdiff_t GetControlInteger(int id) const;
    void SetControlInteger(int id, ptrdiff_t val) const;

    void EnableControl(int id, BOOL fEnable = TRUE)  const { (void) ::EnableWindow(GetDlgItem(id), fEnable); }
    void DisableControl(int id) const { (void) ::EnableWindow(GetDlgItem(id), FALSE); }

    void ShowControl(int id) const { ::ShowWindow(GetDlgItem(id), SW_SHOW); }
    void HideControl(int id) const { ::ShowWindow(GetDlgItem(id), SW_HIDE); }

    bool GetCheck(int id) const { return (SendItemMsg(id, BM_GETCHECK) == BST_CHECKED); }
    bool IsChecked(int id) const { return GetCheck(id); }
    void SetCheck(int id, BOOL bCheck = TRUE) const { (void) SendItemMsg(id, BM_SETCHECK, bCheck); }
    void UnCheck(int id) const { (void) SendItemMsg(id, BM_SETCHECK, FALSE); }

    HICON SetIcon(HICON hIcon, BOOL bBigIcon = TRUE) { ttASSERT(::IsWindow(m_hwnd)); return (HICON)::SendMessage(m_hwnd, WM_SETICON, bBigIcon, (LPARAM)hIcon); }

    LRESULT SendItemMsg(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessage(GetDlgItem(id), msg, wParam, lParam); }
    LRESULT PostItemMsg(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::PostMessage(GetDlgItem(id), msg, wParam, lParam); }

    LRESULT SendMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessage(*this, msg, wParam, lParam); }
    LRESULT PostMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::PostMessage(*this, msg, wParam, lParam); }

    void SetFocus(int idControl) const { ::SetFocus(GetDlgItem(idControl)); }

    void EndDialog(int nResult = IDCANCEL) const { ::EndDialog(m_hwnd, nResult); }
    void FadeWindow();

    void ttDDX_Text(int id, ttCStr& csz) {  (m_bInitializing ? SetControlText(id, csz) : GetControlText(id, &csz)); }
    void ttDDX_Text(int id, ttCWStr& csz) { (m_bInitializing ? SetControlText(id, csz) : GetControlText(id, &csz)); }
    void ttDDX_Check(int id, bool& bFlag) {   (m_bInitializing ? SetCheck(id, bFlag) : (void) (bFlag = GetCheck(id))); }
    void ttDDX_Int(int id, ptrdiff_t* pVal) { (m_bInitializing ? SetControlInteger(id, *pVal) : (void) (*pVal = GetControlInteger(id))); }

    HWND GetParent() { return m_hwndParent; }
    operator HWND() const { return m_hwnd; }

protected:
    // BEGIN_TTCMD_MAP in ttcasemap.h will override this
    virtual bool OnCmdCaseMap(int /* id */, int /* NotifyCode */, LRESULT& /* lResult */) { return false; }

    // BEGIN_TTMSG_MAP in ttcasemap.h will override this
    virtual bool OnMsgMap(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, LRESULT& lResult) { lResult = 0; return false; }

    friend INT_PTR WINAPI ttpriv::DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

    // Class members

    HWND m_hwnd;            // m_hwnd vs m_hWnd -- SDK/include, ATL and WTL use both variants. We're sticking with all lowercase.
    HWND m_hwndParent;

    ttCMultiBtn* m_pShadedBtns;

    int m_idTemplate;

    bool m_bCancelEnd;
    bool m_bInitializing;
    bool m_bModeless;
}; // end of ttCDlg

class ttCComboBox
{
public:
    ttCComboBox() { m_hwnd = NULL; }
    ttCComboBox(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
    ttCComboBox(HWND hwndParent, int id) { m_hwnd = ::GetDlgItem(hwndParent, id); ttASSERT_MSG(m_hwnd, "Invalid Combo-box id"); }

    void Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
    void Initialize(int id) { ttASSERT(m_hwnd); m_hwnd = ::GetDlgItem(GetParent(m_hwnd), id); }
    void Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); }

    LRESULT SendMessageA(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { ttASSERT(m_hwnd); return ::SendMessageA(m_hwnd, msg, wParam, lParam); }
    LRESULT SendMessageW(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { ttASSERT(m_hwnd); return ::SendMessageW(m_hwnd, msg, wParam, lParam); }

    void Enable(BOOL fEnable = TRUE) const { EnableWindow(m_hwnd, fEnable); }

    int     GetTextLength() const { ttASSERT(m_hwnd); return GetWindowTextLength(m_hwnd); }
    LRESULT GetLBTextLength(LRESULT iSel) const { ttASSERT(m_hwnd); return SendMessage(CB_GETLBTEXTLEN, iSel, 0); }

    inline int      GetText(char* psz, int cchMax = MAX_PATH) const { ttASSERT(m_hwnd); return ::GetWindowTextA(m_hwnd, psz, cchMax); }
    inline LRESULT  GetLBText(char* psz, LRESULT iSel) const { return SendMessage(CB_GETLBTEXT, iSel, (LPARAM) psz); }
    inline void     SetText(const char* psz) const { ttASSERT(m_hwnd); ttASSERT(psz); (void) ::SetWindowTextA(m_hwnd, psz); }

    inline int      GetText(wchar_t* pwsz, int cchMax = MAX_PATH) const { ttASSERT(m_hwnd); return ::GetWindowTextW(m_hwnd, pwsz, cchMax); }
    inline LRESULT  GetLBText(wchar_t* pwsz, LRESULT iSel) const { return SendMessageW(CB_GETLBTEXT, iSel, (LPARAM) pwsz); }
    inline void     SetText(const wchar_t* pwsz) const { ttASSERT(m_hwnd); ttASSERT(pwsz); (void) ::SetWindowTextW(m_hwnd, pwsz); }

    LRESULT GetCount() const { return SendMessage(CB_GETCOUNT); }
    void    ResetContent() const { SendMessage(CB_RESETCONTENT); }
    void    Reset() const { SendMessage(CB_RESETCONTENT); }

    LRESULT Add(const char* psz) const { ttASSERT(psz); return SendMessage(CB_ADDSTRING, 0, (LPARAM) psz); }
    LRESULT Add(const wchar_t* pwsz) const { ttASSERT(pwsz); return SendMessageW(CB_ADDSTRING, 0, (LPARAM) pwsz); }
    LRESULT Add(int val) const { char szNum[30]; ttItoa(val, szNum, sizeof(szNum)); return SendMessageA(CB_ADDSTRING, 0, (LPARAM) szNum); }
    LRESULT AddString(const char* psz) const { ttASSERT(psz); return SendMessage(CB_ADDSTRING, 0, (LPARAM) psz); }
    LRESULT AddString(const wchar_t* pwsz) const { ttASSERT(pwsz); return SendMessageW(CB_ADDSTRING, 0, (LPARAM) pwsz); }
    LRESULT InsertString(int index, const char* psz) const { ttASSERT(psz); return SendMessage(CB_INSERTSTRING, index, (LPARAM) psz); }
    LRESULT InsertString(int index, const wchar_t* pwsz) const { ttASSERT(pwsz); return SendMessageW(CB_INSERTSTRING, index, (LPARAM) pwsz); }
    LRESULT DeleteString(WPARAM index) const { return SendMessage(CB_DELETESTRING, index); }

    LRESULT GetItemData(WPARAM index) const {   return SendMessage(CB_GETITEMDATA, index); }
    LRESULT SetItemData(WPARAM index, LPARAM data) const {  return SendMessage(CB_SETITEMDATA, index, data); }

    LRESULT GetCurSel() const { return SendMessage(CB_GETCURSEL); }
    LRESULT SetCurSel(WPARAM index = 0) const { return SendMessage(CB_SETCURSEL, index); }
    LRESULT GetEditSel(DWORD* pStart, DWORD* pEnd) const { return SendMessage(CB_GETEDITSEL, (WPARAM) pStart, (LPARAM) pEnd); }
    void    SetEditSel(int iStart, int iEnd) const { (void) SendMessage(CB_SETEDITSEL, 0, MAKELPARAM(iStart, iEnd)); }
    void    SelectEditContol(void) const { SendMessage(CB_SETEDITSEL, 0, MAKELPARAM(0, -1)); }

    LRESULT FindString(const char* pszString, int iStart = -1) const { ttASSERT(pszString); return SendMessage(CB_FINDSTRINGEXACT, (WPARAM) iStart, (LPARAM) pszString); }
    LRESULT FindString(const wchar_t* pwszString, int iStart = -1) const { ttASSERT(pwszString); return SendMessageW(CB_FINDSTRINGEXACT, (WPARAM) iStart, (LPARAM) pwszString); }
    LRESULT FindPrefix(const char* pszPrefix, int iStart = -1) const { ttASSERT(pszPrefix); return SendMessage(CB_FINDSTRING, (WPARAM) iStart, (LPARAM) pszPrefix); }
    LRESULT FindPrefix(const wchar_t* pwszPrefix, int iStart = -1) const { ttASSERT(pwszPrefix); return SendMessageW(CB_FINDSTRING, (WPARAM) iStart, (LPARAM) pwszPrefix); }

    LRESULT SelectString(const char* pszString, int iStart = -1) const { return SendMessage(CB_SELECTSTRING, (WPARAM) iStart, (LPARAM) pszString); }
    LRESULT SelectString(const wchar_t* pwszString, int iStart = -1) const { return SendMessageW(CB_SELECTSTRING, (WPARAM) iStart, (LPARAM) pwszString); }

    void SetFont(HFONT hfont) { SendMessage(WM_SETFONT, (WPARAM) hfont); }

    void InValidateAlloc(BOOL bErase = TRUE) { ttASSERT(m_hwnd); InvalidateRect(m_hwnd, nullptr, bErase); }

    void operator += (const char* psz) const { ttASSERT(psz); SendMessage(CB_ADDSTRING, 0, (LPARAM) psz); }
    void operator += (const wchar_t* pwsz) const { ttASSERT(pwsz); SendMessageW(CB_ADDSTRING, 0, (LPARAM) pwsz); }

    operator HWND() const { return m_hwnd; }

    HWND m_hwnd;
}; // end of ttCComboBox

class ttCListBox
{
public:
    ttCListBox() { m_hwnd = NULL; }
    ttCListBox(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
    ttCListBox(HWND hwndParent, int id) { m_hwnd = GetDlgItem(hwndParent, id); ttASSERT_MSG(m_hwnd, "Invalid Listbox id"); }

    void    Initialize(int id) { ttASSERT(m_hwnd); m_hwnd = GetDlgItem(GetParent(m_hwnd), id); }
    void    Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); }
    void    Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }

    LRESULT SendMessageA(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessageA(m_hwnd, msg, wParam, lParam); }
    LRESULT SendMessageW(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { ttASSERT(m_hwnd); return ::SendMessageW(m_hwnd, msg, wParam, lParam); }

    void    Enable(BOOL fEnable = TRUE) const { EnableWindow(m_hwnd, fEnable); }

    LRESULT GetTextLength(int index = -1) const { return SendMessage(LB_GETTEXTLEN, (index == -1) ? GetCurSel() : index); }

    LRESULT GetText(char* psz, int index = -1) const { return SendMessageA(LB_GETTEXT, (index == -1) ? GetCurSel() : index, (LPARAM) psz); }
    LRESULT GetText(wchar_t* pwsz, int index = -1) const { return SendMessageW(LB_GETTEXT, (index == -1) ? GetCurSel() : index, (LPARAM) pwsz); }   // wide char version
    void    GetText(ttCStr* pcsz, int index = -1) const { pcsz->GetListBoxText(*this, (index == -1) ? GetCurSel() : index); }
    void    GetText(ttCWStr* pcsz, int index = -1) const { pcsz->GetListBoxText(*this, (index == -1) ? GetCurSel() : index); }                      // wide char version

    LRESULT GetCount() const { return SendMessage(LB_GETCOUNT); }
    LRESULT GetSelCount() const { ttASSERT((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL))); return SendMessage(LB_GETSELCOUNT); }
    void    ResetContent() const { SendMessage(LB_RESETCONTENT); }
    void    Reset() const { SendMessage(LB_RESETCONTENT); }
    void    SetCount(int cItems) {
#ifdef _DEBUG
                    LRESULT result =
#endif
                    SendMessage(LB_SETCOUNT, (WPARAM) cItems);
                    ttASSERT_MSG(result != LB_ERR, "SetCount failed. Does listbox have LBS_NODATA style?"); }

    void    SetFont(HFONT hfont, bool fRedraw = false) { SendMessage(WM_SETFONT, (WPARAM) hfont, (LPARAM) (fRedraw ? TRUE : FALSE)); }

    LRESULT  Add(const char* psz) const { ttASSERT(psz); return SendMessage(LB_ADDSTRING, 0, (LPARAM) psz); }
    LRESULT  Add(const wchar_t* pwsz) const { ttASSERT(pwsz); return SendMessage(LB_ADDSTRING, 0, (LPARAM) pwsz); }
    LRESULT  Add(const char* psz, LPARAM data) const {
                    ttASSERT(psz);
                    LRESULT index = SendMessage(LB_ADDSTRING, 0, (LPARAM) psz);
                    if (index != LB_ERR)
                        SendMessage(LB_SETITEMDATA, index, data);
                    return index; }
    LRESULT  Add(const wchar_t* pwsz, LPARAM data) const {
                    ttASSERT(pwsz);
                    LRESULT index = SendMessageW(LB_ADDSTRING, 0, (LPARAM) pwsz);
                    if (index != LB_ERR)
                        SendMessageW(LB_SETITEMDATA, index, data);
                    return index; }
    LRESULT  AddString(const char* psz) const { return SendMessage(LB_ADDSTRING, 0, (LPARAM) psz); }
    LRESULT  AddString(const wchar_t* pwsz) const { return SendMessageW(LB_ADDSTRING, 0, (LPARAM) pwsz); }
    LRESULT  InsertString(int index, const char* psz) const { return SendMessage(LB_INSERTSTRING, index, (LPARAM) psz); }
    LRESULT  InsertString(int index, const wchar_t* pwsz) const { return SendMessageW(LB_INSERTSTRING, index, (LPARAM) pwsz); }

    LRESULT  DeleteString(ptrdiff_t index) const { return SendMessage(LB_DELETESTRING, index); }

    // void  RemoveListItem();  // removes currently selected item

    LRESULT  GetItemData(WPARAM index) const {   return SendMessage(LB_GETITEMDATA, index); }
    LRESULT  SetItemData(WPARAM index, int data) const {  return SendMessage(LB_SETITEMDATA, index, data); }

    LRESULT  GetItemRect(RECT* prc, WPARAM index = (WPARAM) -1) const { return SendMessage(LB_GETITEMRECT, ((index == (WPARAM) -1) ? GetCurSel() : index), (LPARAM) prc); }


    LRESULT GetCurSel() const { // works on single selection listbox only
                    ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "GetCurSel() only works on single selection listbox");
                    return SendMessage(LB_GETCURSEL); }
    LRESULT SetCurSel(WPARAM index = 0) const { // works on single selection listbox only
                    ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SetCurSel() only works on single selection listbox");
                    return SendMessage(LB_SETCURSEL, index); }
    LRESULT GetTopIndex(void) const { return SendMessage(LB_GETTOPINDEX); }
    void    SetTopIndex(WPARAM index) const { (void) SendMessage(LB_SETTOPINDEX, index); }

    // For multi-select list boxes
    LRESULT GetSel(WPARAM index) const {
                    ttASSERT_MSG((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "GetSel() only works on multiple-select list box");
                    return SendMessage(LB_GETSEL, index); }
    void    SetSel(int index, BOOL fSelect = TRUE) const {
                    ttASSERT_MSG((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SetSel() only works on multiple-select list box");
                    (void) SendMessage(LB_SETSEL, fSelect, MAKELPARAM(index, 0)); }

    LRESULT FindString(const char* pszString, int iStart = -1) const { return SendMessage(LB_FINDSTRINGEXACT, iStart, (LPARAM) pszString); }
    LRESULT FindString(const wchar_t* pwszString, int iStart = -1) const { return SendMessageW(LB_FINDSTRINGEXACT, iStart, (LPARAM) pwszString); }
    LRESULT FindPrefix(const char* pszPrefix, int iStart = -1) const { return SendMessage(LB_FINDSTRING, iStart, (LPARAM) pszPrefix); }
    LRESULT FindPrefix(const wchar_t* pwszPrefix, int iStart = -1) const { return SendMessageW(LB_FINDSTRING, iStart, (LPARAM) pwszPrefix); }
    LRESULT SelectString(const char* pszString, int iStart = -1) const { // works on single selection listbox only
                    ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SelectString only works on single-selection listbox");
                    return SendMessage(LB_SELECTSTRING, iStart, (LPARAM) pszString); }
    LRESULT SelectString(const wchar_t* pwszString, int iStart = -1) const { // works on single selection listbox only
                    ttASSERT_MSG(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SelectString only works on single-selection listbox");
                    return SendMessageW(LB_SELECTSTRING, iStart, (LPARAM) pwszString); }

    void    InValidateAlloc(BOOL bErase = TRUE) { InvalidateRect(m_hwnd, NULL, bErase); }
    void    DisableRedraw(void) { SendMessage(WM_SETREDRAW, FALSE); }
    void    EnableRedraw(void)   { SendMessage(WM_SETREDRAW, TRUE); }

    operator HWND() const { return m_hwnd; }
    void operator += (const char* psz) const { SendMessage(LB_ADDSTRING, 0, (LPARAM) psz); }

    HWND m_hwnd;
};  // end of ttCListBox

class ttCListView
{
public:
    ttCListView() { m_hwnd = NULL; }
    ttCListView(HWND hwndParent, int id) { m_hwnd = ::GetDlgItem(hwndParent, id); ttASSERT_MSG(m_hwnd, "Invalid Listview id"); }

    void    Initialize(int id) { ttASSERT(m_hwnd); m_hwnd = GetDlgItem(GetParent(m_hwnd), id); };
    void    Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); };
    void    Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }

    LRESULT GetCurSel() { return SendMessage(m_hwnd, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0)); }
    LRESULT SetCurSel(int pos);
    LRESULT SetCurSel(const char* pszItem);

    bool    GetItem(LVITEM* pItem) { return SendMessage(m_hwnd, LVM_GETITEM, 0, (LPARAM) pItem) ? true : false; }
    bool    SetItem(LVITEMA* pItem) { return SendMessage(m_hwnd, LVM_SETITEMA, 0, (LPARAM) pItem) ? true : false; }
    bool    SetItem(LVITEMW* pItem) { return SendMessage(m_hwnd, LVM_SETITEMW, 0, (LPARAM) pItem) ? true : false; }

    LRESULT InsertItem(LVITEMA* pitem) { return (LRESULT)   ::SendMessage(m_hwnd, LVM_INSERTITEMA, 0, (LPARAM) pitem); }
    LRESULT InsertItemW(LVITEMW* pitem) { return (LRESULT) ::SendMessage(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) pitem); }
    BOOL    DeleteItem(int index) { return (BOOL) ListView_DeleteItem(m_hwnd, index); }
    void    Reset() const { ListView_DeleteAllItems(m_hwnd); }

    LRESULT AddString(const char* psz, LPARAM lParam = -1);
    LRESULT AddString(const wchar_t* pwsz, LPARAM lParam = -1);

    BOOL    AddSubString(int iItem, int iSubItem, const char* psz);
    BOOL    AddSubString(int iItem, int iSubItem, const wchar_t* pwsz);

    void    InsertColumn(int iColumn, const char* pszText, int width = -1);
    void    InsertColumn(int iColumn, const wchar_t* pwszText, int width = -1);

    void    SetColumnWidth(int col, int width = LVSCW_AUTOSIZE_USEHEADER) { ListView_SetColumnWidth(m_hwnd, col, width); }

    HWND    GetHWND() const { return m_hwnd; }

    operator HWND() const { return m_hwnd; }

    HWND m_hwnd;
};  // end of ttCListView

#endif  // __TTLIB_TTCDLG_H__
