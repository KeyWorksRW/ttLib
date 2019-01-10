/////////////////////////////////////////////////////////////////////////////
// Name:		CTTDlg
// Purpose:		Class for displaying a modal dialog
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 2002-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_CTTDLG_H__
#define __TTLIB_CTTDLG_H__

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

// This is a modal-only dialog class that only requires Windows. It can be used whether your app is using
// ATL, WTL, wxWidgets, or is just a console app that needs a Modal() dialog box.

// If you use BEGIN_TTMSG_MAP/END_TTMSG_MAP then you can use all the regular handlers from ttmsgs.h just as
// if it was an ATL CDialogImpl derived class

// Classes are also provided for some dialog controls: CDlgComboBox, CDlgListBox, CDlgListView

#include <CommCtrl.h>
#include "cstr.h"	// CStr
#include "cwstr.h"	// CWStr

#ifdef BEGIN_MSG_MAP_EX
	#error You cannot use CTTDlg if you have #included <atlcrack.h> -- it conflicts with ttmsgs.h
#endif

#define BEGIN_TTMSG_MAP() BOOL OnMsgMap(UINT uMsg, WPARAM wParam, LPARAM lParam) { uMsg; wParam; lParam;
	#include "ttmsgs.h"
#define END_TTMSG_MAP() return FALSE; }

#ifndef __DLG_ID__
	#ifdef _DEBUG
		#define DLG_ID(id) CheckItemID(id, #id, __LINE__, __FILE__)
	#else
		#define DLG_ID(id) id
	#endif
#endif

class CTTDlg
{
public:
	CTTDlg(UINT idTemplate, HWND hwnd = NULL);

	INT_PTR DoModal();

	virtual BOOL OnMsgMap(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */) { return FALSE; }	  // Use of BEGIN_TTMSG_MAP will override this
	virtual void OnBegin() { }
	virtual void OnEnd() { }
	virtual void OnCancel() { }

	void DontCenterWindow(void) { m_fCenterWindow = false; }
	void FadeOnExit() { m_fFade = true; }
	void DontShadeBtns() { m_bShadeBtns = false; }
	void CancelEnd() { m_fCancelEnd = true; } // call within OnEnd() to cancel ending the dialog

	HWND GetDlgItem(ptrdiff_t id) const { return ::GetDlgItem(m_hWnd, (int) id); }
	int	 GetControlTextLength(ptrdiff_t id) const { return ::GetWindowTextLength(GetDlgItem(id)); }
	BOOL GetControlRect(ptrdiff_t id, RECT* prc) const { return ::GetWindowRect(GetDlgItem(id), prc); }

	void GetControlText(ptrdiff_t id, char* pszText, int cchMax = MAX_PATH) const { (void) ::GetWindowTextA(GetDlgItem(id), pszText, cchMax); }
	void GetControlText(ptrdiff_t id, CStr* pcsz) const { pcsz->GetWindowText(GetDlgItem(id)); }
	void SetControlText(ptrdiff_t id, const char* pszText) const { ASSERT(pszText); (void) ::SetWindowTextA(GetDlgItem(id), pszText); }

	void GetControlText(ptrdiff_t id, wchar_t* pwszText, int cchMax = MAX_PATH) const { (void) ::GetWindowTextW(GetDlgItem(id), pwszText, cchMax); }
	void GetControlText(ptrdiff_t id, CWStr* pcsz) const { pcsz->GetWindowText(GetDlgItem(id)); }
	void SetControlText(ptrdiff_t id, const wchar_t* pwszText) const { ASSERT(pwszText); (void) ::SetWindowTextW(GetDlgItem(id), pwszText); }

	ptrdiff_t GetControlInteger(ptrdiff_t id) const;
	void SetControlInteger(ptrdiff_t id, ptrdiff_t val) const;

	void EnableControl(int id, BOOL fEnable = TRUE)	 const { (void) ::EnableWindow(GetDlgItem(id), fEnable); }
	void DisableControl(int id) const { (void) ::EnableWindow(GetDlgItem(id), FALSE); }

	void ShowControl(int id) const { ::ShowWindow(GetDlgItem(id), SW_SHOW); }
	void HideControl(int id) const { ::ShowWindow(GetDlgItem(id), SW_HIDE); }

	bool GetCheck(int id) const { return (SendMessage(id, BM_GETCHECK) == BST_CHECKED); }
	bool isChecked(int id) const { return GetCheck(id); }
	void SetCheck(int id, BOOL bCheck = TRUE) const { (void) SendMessage(id, BM_SETCHECK, bCheck); }
	void UnCheck(int id) const { (void) SendMessage(id, BM_SETCHECK, FALSE); }

	HICON SetIcon(HICON hIcon, BOOL bBigIcon = TRUE) { ASSERT(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, WM_SETICON, bBigIcon, (LPARAM)hIcon); }

	LRESULT SendMessage(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessage(GetDlgItem(id), msg, wParam, lParam); }
	LRESULT SendMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessage(*this, msg, wParam, lParam); }
	LRESULT PostMessage(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::PostMessage(GetDlgItem(id), msg, wParam, lParam); }
	LRESULT PostMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::PostMessage(*this, msg, wParam, lParam); }

	void SetFocus(int idControl) const { ::SetFocus(GetDlgItem(idControl)); }

	void EndDialog(int nResult = IDCANCEL) const { ::EndDialog(m_hWnd, nResult); }
	void FadeWindow();

	void KDDX_Text(int id, CStr& csz) {
		if (m_bInitializing)
			SetControlText(id, csz);
		else
			GetControlText(id, &csz);
	}
	void KDDX_Text(int id, CWStr& csz) {	// wide character version
		if (m_bInitializing)
			SetControlText(id, csz);
		else
			GetControlText(id, &csz);
	}
	void KDDX_Check(int id, bool& bFlag) {
		if (m_bInitializing)
			SetCheck(id, bFlag);
		else
			bFlag = GetCheck(id);
	}
	void KDDX_Int(int id, ptrdiff_t* pVal) {
		if (m_bInitializing)
			SetControlInteger(id, *pVal);
		else
			*pVal = GetControlInteger(id);
	}

	operator HWND() const { return m_hWnd; }

#ifdef _DEBUG
	DWORD CheckItemID(int id, const char* pszID, int line, const char* file) const;
#endif

protected:
	friend INT_PTR WINAPI CTTDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

	// Class members

	bool m_bInitializing;
	bool m_bShadeBtns;
	bool m_fCenterWindow;
	bool m_fCancelEnd;
	bool m_fFade;

	int m_idTemplate;

	HWND m_hWnd;
	HWND m_hwndParent;

	LRESULT lResult;	// used by BEGIN_TTMSG_MAP
};

class CDlgComboBox
{
public:
	CDlgComboBox() { m_hwnd = NULL; }
	CDlgComboBox(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
	CDlgComboBox(HWND hwndParent, int id) {
		m_hwnd = ::GetDlgItem(hwndParent, id);
		ASSERT_COMMENT(m_hwnd, "Invalid Combo-box id");
	}
	void Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
	void Initialize(int id) { ASSERT(m_hwnd); m_hwnd = ::GetDlgItem(GetParent(m_hwnd), id); }
	void Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); }

	LRESULT SendMessageA(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { ASSERT(m_hwnd); return ::SendMessageA(m_hwnd, msg, wParam, lParam); }
	LRESULT SendMessageW(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { ASSERT(m_hwnd); return ::SendMessageW(m_hwnd, msg, wParam, lParam); }

	void Enable(BOOL fEnable = TRUE) const { EnableWindow(m_hwnd, fEnable); }

	int		GetTextLength() const { ASSERT(m_hwnd); return GetWindowTextLength(m_hwnd); }
	LRESULT	GetLBTextLength(LRESULT iSel) const { ASSERT(m_hwnd); return SendMessage(CB_GETLBTEXTLEN, iSel, 0); }

	inline int		GetText(char* psz, int cchMax = MAX_PATH) const { ASSERT(m_hwnd); return ::GetWindowTextA(m_hwnd, psz, cchMax); }
	inline LRESULT	GetLBText(char* psz, LRESULT iSel) const { return SendMessage(CB_GETLBTEXT, iSel, (LPARAM) psz); }
	inline void		SetText(const char* psz) const { ASSERT(m_hwnd); ASSERT(psz); (void) ::SetWindowTextA(m_hwnd, psz); }

	inline int		GetText(wchar_t* pwsz, int cchMax = MAX_PATH) const { ASSERT(m_hwnd); return ::GetWindowTextW(m_hwnd, pwsz, cchMax); }
	inline LRESULT	GetLBText(wchar_t* pwsz, LRESULT iSel) const { return SendMessageW(CB_GETLBTEXT, iSel, (LPARAM) pwsz); }
	inline void		SetText(const wchar_t* pwsz) const { ASSERT(m_hwnd); ASSERT(pwsz); (void) ::SetWindowTextW(m_hwnd, pwsz); }

	LRESULT GetCount() const { return SendMessage(CB_GETCOUNT); }
	void	ResetContent() const { SendMessage(CB_RESETCONTENT); }
	void	Reset() const { SendMessage(CB_RESETCONTENT); }

	LRESULT Add(const char* psz) const {
		ASSERT(psz);
		if (!psz)
			return CB_ERR;
		return SendMessage(CB_ADDSTRING, 0, (LPARAM) psz);
	}
	LRESULT Add(const wchar_t* pwsz) const {	// wide char version
		ASSERT(pwsz);
		if (!pwsz)
			return CB_ERR;
		return SendMessageW(CB_ADDSTRING, 0, (LPARAM) pwsz);
	}
	LRESULT Add(int val) const { char szNum[30]; Itoa(val, szNum, sizeof(szNum)); return SendMessageA(CB_ADDSTRING, 0, (LPARAM) szNum); }
	LRESULT AddString(const char* psz) const {
		ASSERT(psz);
		if (!psz)
			return CB_ERR;
		return SendMessage(CB_ADDSTRING, 0, (LPARAM) psz);
	}
	LRESULT AddString(const wchar_t* pwsz) const {	// wide char version
		ASSERT(pwsz);
		if (!pwsz)
			return CB_ERR;
		return SendMessageW(CB_ADDSTRING, 0, (LPARAM) pwsz);
	}
	LRESULT InsertString(int index, const char* psz) const {
		ASSERT(psz);
		if (!psz)
			return CB_ERR;
		return SendMessage(CB_INSERTSTRING, index, (LPARAM) psz);
	}
	LRESULT InsertString(int index, const wchar_t* pwsz) const {	// wide char version
		ASSERT(pwsz);
		if (!pwsz)
			return CB_ERR;
		return SendMessageW(CB_INSERTSTRING, index, (LPARAM) pwsz);
	}
	LRESULT DeleteString(WPARAM index) const { return SendMessage(CB_DELETESTRING, index); }

	LRESULT GetItemData(WPARAM index) const {	return SendMessage(CB_GETITEMDATA, index); }
	LRESULT SetItemData(WPARAM index, LPARAM data) const {	return SendMessage(CB_SETITEMDATA, index, data); }

	LRESULT GetCurSel() const { return SendMessage(CB_GETCURSEL); }
	LRESULT SetCurSel(WPARAM index = 0) const { return SendMessage(CB_SETCURSEL, index); }
	LRESULT GetEditSel(DWORD* pStart, DWORD* pEnd) const { return SendMessage(CB_GETEDITSEL, (WPARAM) pStart, (LPARAM) pEnd); }
	void	SetEditSel(int iStart, int iEnd) const { (void) SendMessage(CB_SETEDITSEL, 0, MAKELPARAM(iStart, iEnd)); }
	void	SelectEditContol(void) const { SendMessage(CB_SETEDITSEL, 0, MAKELPARAM(0, -1)); }

	LRESULT FindString(const char* pszString, int iStart = -1) const {
		ASSERT(pszString);
		if (!pszString)
			return CB_ERR;
		return SendMessage(CB_FINDSTRING, (WPARAM) iStart, (LPARAM) pszString);
	}
	LRESULT FindString(const wchar_t* pwszString, int iStart = -1) const {
		ASSERT(pwszString);
		if (!pwszString)
			return CB_ERR;
		return SendMessageW(CB_FINDSTRING, (WPARAM) iStart, (LPARAM) pwszString);
	}
	LRESULT SelectString(const char* pszString, int iStart = -1) const {
		ASSERT(pszString);
		if (!pszString)
			return CB_ERR;
		return SendMessage(CB_SELECTSTRING, (WPARAM) iStart, (LPARAM) pszString);
	}
	LRESULT SelectString(const wchar_t* pwszString, int iStart = -1) const {
		ASSERT(pwszString);
		if (!pwszString)
			return CB_ERR;
		return SendMessageW(CB_SELECTSTRING, (WPARAM) iStart, (LPARAM) pwszString);
	}

	void SetFont(HFONT hfont) { SendMessage(WM_SETFONT, (WPARAM) hfont); }

	void Invalidate(BOOL bErase = TRUE) { ASSERT(m_hwnd); InvalidateRect(m_hwnd, nullptr, bErase); }

	void operator+=(const char* psz) const { ASSERT(psz); SendMessage(CB_ADDSTRING, 0, (LPARAM) psz); }
	void operator+=(const wchar_t* pwsz) const { ASSERT(pwsz); SendMessageW(CB_ADDSTRING, 0, (LPARAM) pwsz); }

	HWND m_hwnd;

	operator HWND() const { return m_hwnd; }
};

class CDlgListBox
{
public:
	CDlgListBox() { m_hwnd = NULL; }
	CDlgListBox(HWND hwndCtrl) { m_hwnd = hwndCtrl; }
	CDlgListBox(HWND hwndParent, int id) { m_hwnd = GetDlgItem(hwndParent, id); ASSERT_COMMENT(m_hwnd, "Invalid Listbox id"); }

	void Initialize(int id) { ASSERT(m_hwnd); m_hwnd = GetDlgItem(GetParent(m_hwnd), id); }
	void Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); }
	void Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }

	LRESULT	SendMessageA(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessageA(m_hwnd, msg, wParam, lParam); }
	LRESULT SendMessageW(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { ASSERT(m_hwnd); return ::SendMessageW(m_hwnd, msg, wParam, lParam); }

	void	 Enable(BOOL fEnable = TRUE) const { EnableWindow(m_hwnd, fEnable); }

	LRESULT	 GetTextLength(int index = -1) const { return SendMessage(LB_GETTEXTLEN, (index == -1) ? GetCurSel() : index); }

	LRESULT	 GetText(char* psz, int index = -1) const { return SendMessageA(LB_GETTEXT, (index == -1) ? GetCurSel() : index, (LPARAM) psz); }
	LRESULT	 GetText(wchar_t* pwsz, int index = -1) const { return SendMessageW(LB_GETTEXT, (index == -1) ? GetCurSel() : index, (LPARAM) pwsz); }	// wide char version
	void	 GetText(CStr* pcsz, int index = -1) const { pcsz->GetListBoxText(*this, (index == -1) ? GetCurSel() : index); }
	void	 GetText(CWStr* pcsz, int index = -1) const { pcsz->GetListBoxText(*this, (index == -1) ? GetCurSel() : index); }						// wide char version

	LRESULT	 GetCount() const { return SendMessage(LB_GETCOUNT); }
	LRESULT	 GetSelCount() const {
			ASSERT((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)));
			return SendMessage(LB_GETSELCOUNT); }
	void ResetContent() const { SendMessage(LB_RESETCONTENT); }
	void Reset() const { SendMessage(LB_RESETCONTENT); }
	void SetCount(int cItems) {
#ifdef _DEBUG
		LRESULT result =
#endif
		SendMessage(LB_SETCOUNT, (WPARAM) cItems);
		ASSERT_COMMENT(result != LB_ERR, "SetCount failed. Does listbox have LBS_NODATA style?");
	}

	void SetFont(HFONT hfont, bool fRedraw = false) { SendMessage(WM_SETFONT, (WPARAM) hfont, (LPARAM) (fRedraw ? TRUE : FALSE)); }

	LRESULT	 Add(const char* psz) const {
		ASSERT(psz);
		if (!psz)
			return LB_ERR;
		return SendMessage(LB_ADDSTRING, 0, (LPARAM) psz);
	}
	LRESULT	 Add(const wchar_t* pwsz) const {	// wide char version
		ASSERT(pwsz);
		if (!pwsz)
			return LB_ERR;
		return SendMessage(LB_ADDSTRING, 0, (LPARAM) pwsz);
	}
	LRESULT	 Add(const char* psz, LPARAM data) const {
		ASSERT(psz);
		if (!psz)
			return LB_ERR;
		LRESULT index = SendMessage(LB_ADDSTRING, 0, (LPARAM) psz);
		if (index != LB_ERR)
			SendMessage(LB_SETITEMDATA, index, data);
		return index;
	}
	LRESULT	 Add(const wchar_t* pwsz, LPARAM data) const {
		ASSERT(pwsz);
		if (!pwsz)
			return LB_ERR;
		LRESULT index = SendMessageW(LB_ADDSTRING, 0, (LPARAM) pwsz);
		if (index != LB_ERR)
			SendMessageW(LB_SETITEMDATA, index, data);
		return index;
	}
	LRESULT	 AddString(const char* psz) const { return SendMessage(LB_ADDSTRING, 0, (LPARAM) psz); }
	LRESULT	 AddString(const wchar_t* pwsz) const { return SendMessageW(LB_ADDSTRING, 0, (LPARAM) pwsz); }
	LRESULT	 InsertString(int index, const char* psz) const { return SendMessage(LB_INSERTSTRING, index, (LPARAM) psz); }
	LRESULT	 InsertString(int index, const wchar_t* pwsz) const { return SendMessageW(LB_INSERTSTRING, index, (LPARAM) pwsz); }

	LRESULT	 DeleteString(ptrdiff_t index) const { return SendMessage(LB_DELETESTRING, index); }

	// void	 RemoveListItem();	// removes currently selected item

	LRESULT	 GetItemData(WPARAM index) const {	 return SendMessage(LB_GETITEMDATA, index); }
	LRESULT	 SetItemData(WPARAM index, int data) const {  return SendMessage(LB_SETITEMDATA, index, data); }

	LRESULT GetItemRect(RECT* prc, WPARAM index = (WPARAM) -1)	const { return SendMessage(LB_GETITEMRECT, ((index == (WPARAM) -1) ? GetCurSel() : index), (LPARAM) prc); }

	LRESULT	 GetCurSel() const { // works on single selection listbox only
		ASSERT_COMMENT(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "GetCurSel() only works on single selection listbox");
		return SendMessage(LB_GETCURSEL); }
	LRESULT	 SetCurSel(WPARAM index = 0) const { // works on single selection listbox only
		ASSERT_COMMENT(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SetCurSel() only works on single selection listbox");
		return SendMessage(LB_SETCURSEL, index); }
	LRESULT	 GetTopIndex(void) const { return SendMessage(LB_GETTOPINDEX); }
	void	 SetTopIndex(WPARAM index) const { (void) SendMessage(LB_SETTOPINDEX, index); }

	// For multi-select list boxes
	LRESULT	 GetSel(WPARAM index) const {
		ASSERT_COMMENT((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "GetSel() only works on multiple-select list box");
		return SendMessage(LB_GETSEL, index); }
	void SetSel(int index, BOOL fSelect = TRUE) const {
		ASSERT_COMMENT((GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SetSel() only works on multiple-select list box");
		(void) SendMessage(LB_SETSEL, fSelect, MAKELPARAM(index, 0)); }

	LRESULT	 FindString(const char* pszString, int iStart = -1) const { return SendMessage(LB_FINDSTRING, iStart, (LPARAM) pszString); }
	LRESULT	 FindString(const wchar_t* pwszString, int iStart = -1) const { return SendMessageW(LB_FINDSTRING, iStart, (LPARAM) pwszString); }
	LRESULT	 SelectString(const char* pszString, int iStart = -1) const { // works on single selection listbox only }
		ASSERT_COMMENT(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SelectString only works on single-selection listbox");
		return SendMessage(LB_SELECTSTRING, iStart, (LPARAM) pszString); }
	LRESULT	 SelectString(const wchar_t* pwszString, int iStart = -1) const { // works on single selection listbox only }
		ASSERT_COMMENT(!(GetWindowLong(m_hwnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)), "SelectString only works on single-selection listbox");
		return SendMessageW(LB_SELECTSTRING, iStart, (LPARAM) pwszString); }

	void Invalidate(BOOL bErase = TRUE) { InvalidateRect(m_hwnd, NULL, bErase); }
	void DisableRedraw(void) { SendMessage(WM_SETREDRAW, FALSE); }
	void EnableRedraw(void)	 { SendMessage(WM_SETREDRAW, TRUE); }

	HWND m_hwnd;

	operator HWND() const { return m_hwnd; }
	void operator+=(const char* psz) const { SendMessage(LB_ADDSTRING, 0, (LPARAM) psz); }
};

class CDlgListView
{
public:
	CDlgListView() { m_hwnd = NULL; }
	CDlgListView(HWND hwndParent, int id) {
			m_hwnd = ::GetDlgItem(hwndParent, id);
			ASSERT_COMMENT(m_hwnd, "Invalid Listview id");
		}
	void Initialize(int id) { ASSERT(m_hwnd); m_hwnd = GetDlgItem(GetParent(m_hwnd), id); };
	void Initialize(HWND hdlg, int id) { m_hwnd = ::GetDlgItem(hdlg, id); };
	void Attach(HWND hwndCtrl) { m_hwnd = hwndCtrl; }

	LRESULT	 GetCurSel() { return SendMessage(m_hwnd, LVM_GETNEXTITEM, (WPARAM) -1, MAKELPARAM(LVNI_SELECTED, 0)); }

	bool GetItem(LV_ITEM* pItem) { return SendMessage(m_hwnd, LVM_GETITEM, 0, (LPARAM) pItem) ? true : false; }
	bool SetItem(LV_ITEMA* pItem) { return SendMessage(m_hwnd, LVM_SETITEMA, 0, (LPARAM) pItem) ? true : false; }
	bool SetItem(LV_ITEMW* pItem) { return SendMessage(m_hwnd, LVM_SETITEMW, 0, (LPARAM) pItem) ? true : false; }

	LRESULT InsertItem(LV_ITEMA* pitem) { return (LRESULT)	::SendMessage(m_hwnd, LVM_INSERTITEMA, 0, (LPARAM) pitem); }
	LRESULT InsertItemW(LV_ITEMW* pitem) { return (LRESULT) ::SendMessage(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) pitem); }
	BOOL DeleteItem(int index) { return (BOOL) ListView_DeleteItem(m_hwnd, index); }
	void Reset() const { ListView_DeleteAllItems(m_hwnd); }
	LRESULT AddString(const char* psz, LPARAM lParam = -1);
	LRESULT AddString(const wchar_t* pwsz, LPARAM lParam = -1);

	BOOL AddSubString(int iItem, int iSubItem, const char* psz);
	BOOL AddSubString(int iItem, int iSubItem, const wchar_t* pwsz);

	void InsertColumn(int iColumn, const char* pszText, int width = -1);
	void InsertColumn(int iColumn, const wchar_t* pwszText, int width = -1);

	void SetColumnWidth(int col, int width = LVSCW_AUTOSIZE_USEHEADER) { ListView_SetColumnWidth(m_hwnd, col, width); }

	HWND GetHWND() const { return m_hwnd; }

	HWND m_hwnd;
	LVCOLUMN m_lvColumn;

	operator HWND() const { return m_hwnd; }
};

// These are needed for centering a window (WMP_CENTER_WINDOW)

HMONITOR KeyMonitorFromWindow(HWND hwnd, DWORD dwFlags);
HMONITOR KeyMonitorFromPoint(POINT pt, DWORD dwFlags);
BOOL	 KeyGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi);

#endif	// __TTLIB_CTTDLG_H__
