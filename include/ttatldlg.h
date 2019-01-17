/////////////////////////////////////////////////////////////////////////////
// Name:		cdlgatl.h
// Purpose:		Adds CTTDlg-like functionality to ATL
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Include this inside of an ATL CWindow or CDialogImpl header file to get some of the same functionality
// that CTTDlg.h has.

	void InitializeSpinControl(UINT idSpin, UINT idBuddy, int minVal, int maxVal) {
			SendCtrlMessage(idSpin, UDM_SETBUDDY, (WPARAM) (HWND) GetDlgItem(idBuddy));
			SendCtrlMessage(idSpin, UDM_SETRANGE, 0, MAKELPARAM(maxVal, minVal)); }

	void GetControlText(int id, char* pszText, int cchMax = MAX_PATH) const { (void) ::GetWindowText(GetDlgItem(id), pszText, cchMax); }
	void GetControlText(int id, CStr* pcsz) { pcsz->GetWindowText(GetDlgItem(id)); }
	int	 GetControlTextLength(int id) { return ::GetWindowTextLength(GetDlgItem(id)); }
	BOOL GetControlRect(int id, RECT* prc) { return ::GetWindowRect(GetDlgItem(id), prc); }

	void SetControlText(int id, const char* pszText) const { ASSERT(pszText); (void) ::SetWindowText(GetDlgItem(id), pszText); }
	void SetControlText(int id, int idResource) const { (void) ::SetWindowText(GetDlgItem(id), GetResString(idResource)); }

	void EnableControl(int id, BOOL fEnable = TRUE)	 const { (void) ::EnableWindow(GetDlgItem(id), fEnable); }
	void DisableControl(int id) const { (void) ::EnableWindow(GetDlgItem(id), FALSE); }

	void ShowControl(int id) const { ::ShowWindow(GetDlgItem(id), SW_SHOW); }
	void HideControl(int id) const { ::ShowWindow(GetDlgItem(id), SW_HIDE); }

	bool isChecked(int id) const { return (SendCtrlMessage(id, BM_GETCHECK) != BST_UNCHECKED); }
	void Check(int id)	 const {(void) SendCtrlMessage(id, BM_SETCHECK, TRUE); }
	void UnCheck(int id) const {(void) SendCtrlMessage(id, BM_SETCHECK, FALSE); }

	bool GetCheck(int id) const { return isChecked(id); }
	void SetCheck(int id, BOOL fCheck = TRUE) const { (void) SendCtrlMessage(id, BM_SETCHECK, fCheck); }

	LRESULT	 SendCtrlMessage(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessage(GetDlgItem(id), msg, wParam, lParam); }
	LRESULT	 PostCtrlMessage(int id, UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::PostMessage(GetDlgItem(id), msg, wParam, lParam); }

#ifdef _DEBUG
	int CheckItemID(int id, const char* pszID, int line, const char* file) {
		if (::GetDlgItem(*this, id) == NULL) {
			CStr cszMsg;
			cszMsg.printf("Invalid dialog control id: %s (%u)", pszID, id);
			AssertionMsg(cszMsg, file, __func__, line);
		}
		return id;
	}
#endif
