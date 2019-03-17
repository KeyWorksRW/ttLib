/////////////////////////////////////////////////////////////////////////////
// Name:		ttcasemap.h
// Purpose:		Macros for mapping Windows messages to functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/*
	Place one or both of the the following macro blocks within a class that inherits from ttCDlg or ttCWin:

	BEGIN_TTCMD_MAP()
		// Insert a TTCASE_CMD or TTCASE_CTRL macro for every WM_COMMAND message you want to process
	END_TTMSG_MAP

	BEGIN_TTMSG_MAP()
		// Insert a TTMSG_WM_... macro for every message you want to process
	END_TTMSG_MAP()

	For each macro you process you will need to declare a matching function. For example:

		BEGIN_TTCMD_MAP()
			TTCASE_CMD(IDBTN_MYBTN, OnMyBtn)
		END_TTMSG_MAP()

		void OnMyBtn();

		BEGIN_TTMSG_MAP()
			TTMSG_WM_PAINT(OnPaint)
		END_TTMSG_MAP()

		void OnPaint();

	All macros return true to indicate that you handled the message and that the default window/dialog procedure should
	not be called. If the message requires a non-zero return, the called function will have a return type of LRESULT.
	Otherwise, a return type is not used.

	For messages that you would prefer to handle inline instead of calling a function, simply add a case statement. For
	example:

		BEGIN_TTMSG_MAP()
			TTMSG_WM_PAINT(OnPaint)

			case WM_DESTROY:
				PostQuitMessage(0);
				return true;	// to indicate we processed the message
		END_TTMSG_MAP()

	If you cannot find a macro for the message you want to process, use the TTMSG_ANY macro
*/

#pragma once

#ifndef __TTLIB_TTMSG_MAP_H__
#define __TTLIB_TTMSG_MAP_H__

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp) ((int) (short) LOWORD(lp))
	#define GET_Y_LPARAM(lp) ((int) (short) HIWORD(lp))
#endif

#define BEGIN_TTCMD_MAP() bool OnCmdCaseMap(size_t id, size_t NotifyCode, LRESULT& lResult) { NotifyCode; lResult; switch (id) {
#define BEGIN_TTMSG_MAP() bool OnMsgMap(size_t msg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) { wParam; lParam; lResult; switch (msg) {
#define END_TTMSG_MAP() default: return false; } }

////////////////////////////////////////////////////////////////
// The following macros are for use in a BEGIN_TTCMD_MAP block
////////////////////////////////////////////////////////////////

// void func();	// TTCASE_CMD calls func() without passing the NotifyCode
#define TTCASE_CMD(id, func) \
	case id: \
		func(); \
		return true;

// void func(size_t NotifyCode);	// use this to check the notification code
#define TTCASE_CTRL(id, func) \
	case id: \
		func(NotifyCode); \
		return true;

////////////////////////////////////////////////////////////////
// The following macros are for use in a BEGIN_TTMSG_MAP block
////////////////////////////////////////////////////////////////

// hdc not passed since it's assumed you will use BeginPaint()/EndPaint()
// void OnPaint();
#define TTMSG_WM_PAINT(func) \
	case WM_PAINT: \
		func(); \
		return true;

// LRESULT OnNotify(size_t id, NMHDR* pNmHdr);
#define TTMSG_WM_NOTIFY(func) \
	case WM_NOTIFY: \
		lResult = func((size_t) wParam, (NMHDR*) lParam); \
		return true;

// BOOL OnCreate(CREATESTRUCT* pcs);
#define TTMSG_WM_CREATE(func) \
	case WM_CREATE: \
		lResult = (LRESULT) func((CREATESTRUCT*) lParam); \
		return true;

// LRESULT OnCopyData(HWND hwndSender, COPYDATASTRUCT* pcds);
#define TTMSG_WM_COPYDATA(func) \
	case WM_COPYDATA: \
		lResult = func((HWND) wParam, (COPYDATASTRUCT*) lParam); \
		return true;

// void OnDestroy();
#define TTMSG_WM_DESTROY(func) \
	case WM_DESTROY: \
		func(); \
		return true;

// void OnMove(int xPos, int yPos);
#define TTMSG_WM_MOVE(func) \
	case WM_MOVE: \
		func(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnSize(size_t fSizeType, int xPos, int yPos);
#define TTMSG_WM_SIZE(func) \
	case WM_SIZE: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnActivate(size_t flag, BOOL bMinimized, HWND hwndPrevious);
#define TTMSG_WM_ACTIVATE(func) \
	case WM_ACTIVATE: \
		func((size_t) LOWORD(wParam), (BOOL) HIWORD(wParam), (HWND) lParam); \
		return true;

// void OnFocus(HWND hwndLoseFocus);
#define TTMSG_WM_SETFOCUS(func) \
	case WM_SETFOCUS: \
		func((HWND) wParam); \
		return true;

// void OnKillFocus(HWND hwndGainFocus);
#define TTMSG_WM_KILLFOCUS(func) \
	case WM_KILLFOCUS: \
		func((HWND) wParam); \
		return true;

// void OnEnable(BOOL bEnabled);
#define TTMSG_WM_ENABLE(func) \
	case WM_ENABLE: \
		func((BOOL) wParam); \
		return true;

// void OnClose();
#define TTMSG_WM_CLOSE(func) \
	case WM_CLOSE: \
		func(); \
		return true;

// LRESULT OnQueryEndSession(size_t flags);
#define TTMSG_WM_QUERYENDSESSION(func) \
	case WM_QUERYENDSESSION: \
		lResult = func((size_t) lParam); \
		return true;

// LRESULT OnQueryOpen();
#define TTMSG_WM_QUERYOPEN(func) \
	case WM_QUERYOPEN: \
		lResult = func(); \
		return true;

// LRESULT OnEraseBknd(HDC hdc);
#define TTMSG_WM_ERASEBKGND(func) \
	case WM_ERASEBKGND: \
		lResult = func((HDC) wParam); \
		return true;

// void OnSysColorChange();
#define TTMSG_WM_SYSCOLORCHANGE(func) \
	case WM_SYSCOLORCHANGE: \
		func(); \
		return true;

// void OnEndSession(BOOL bSessionEnding, size_t flags);
#define TTMSG_WM_ENDSESSION(func) \
	case WM_ENDSESSION: \
		func((BOOL) wParam, (size_t) lParam); \
		return true;

// void OnShowWindow(BOOL bShow, size_t fStatus);
#define TTMSG_WM_SHOWWINDOW(func) \
	case WM_SHOWWINDOW: \
		func((BOOL) wParam, (size_t) lParam); \
		return true;

// HBRUSH OnCtlColorEdit(HDC hdc, HWND hwndCtrl);
#define TTMSG_WM_CTLCOLOREDIT(func) \
	case WM_CTLCOLOREDIT: \
		lResult = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true;

// HBRUSH OnCtlColorLB(HDC hdc, HWND hwndCtrl);
#define TTMSG_WM_CTLCOLORLISTBOX(func) \
	case WM_CTLCOLORLISTBOX: \
		lResult = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true;

// HBRUSH OnCtlColorBtn(HDC hdc, HWND hwndCtrl);
#define TTMSG_WM_CTLCOLORBTN(func) \
	case WM_CTLCOLORBTN: \
		lResult = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true;

// HBRUSH OnCtlColorDlg(HDC hdc, HWND hwndCtrl);
#define TTMSG_WM_CTLCOLORDLG(func) \
	case WM_CTLCOLORDLG: \
		lResult = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true;

// HBRUSH OnCtlColorSB(HDC hdc, HWND hwndCtrl);
#define TTMSG_WM_CTLCOLORSCROLLBAR(func) \
	case WM_CTLCOLORSCROLLBAR: \
		lResult = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true;

// HBRUSH OnCtlColorStatic(HDC hdc, HWND hwndCtrl);
#define TTMSG_WM_CTLCOLORSTATIC(func) \
	case WM_CTLCOLORSTATIC: \
		lResult = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true;

// void OnSettingChange(size_t flag, const char* pszMetrics);
#define TTMSG_WM_SETTINGCHANGE(func) \
	case WM_SETTINGCHANGE: \
		func((size_t) wParam, (const char*) lParam); \
		return true;

// void OnDevModeChange(const char* pszDevice)
#define TTMSG_WM_DEVMODECHANGE(func) \
	case WM_DEVMODECHANGE: \
		func((const char*) lParam); \
		return true;

// void OnActivateApp(BOOL bActivated, size_t idThread);
#define TTMSG_WM_ACTIVATEAPP(func) \
	case WM_ACTIVATEAPP: \
		func((BOOL) wParam, (size_t) lParam); \
		return true;

// void OnFontChange();
#define TTMSG_WM_FONTCHANGE(func) \
	case WM_FONTCHANGE: \
		func(); \
		return true;

// void OnTimeChange();
#define TTMSG_WM_TIMECHANGE(func) \
	case WM_TIMECHANGE: \
		func(); \
		return true;

// void OnCancelMode();
#define TTMSG_WM_CANCELMODE(func) \
	case WM_CANCELMODE: \
		func(); \
		return true;

// BOOL OnSetCursor(HWND hwnd, size_t hitTest, size_t uMsg);
#define TTMSG_WM_SETCURSOR(func) \
	case WM_SETCURSOR: \
		lResult = (LRESULT) func((HWND) wParam, (size_t) LOWORD(lParam), (size_t) HIWORD(lParam)); \
		return true;

// LRESULT OnMouseActivate(HWND hwndTopParent, size_t hitTest, size_t uMsg);
#define TTMSG_WM_MOUSEACTIVATE(func) \
	case WM_MOUSEACTIVATE: \
		lResult = func((HWND) wParam, (size_t) LOWORD(lParam), (size_t) HIWORD(lParam)); \
		return true;

// void OnChildActivate();
#define TTMSG_WM_CHILDACTIVATE(func) \
	case WM_CHILDACTIVATE: \
		func(); \
		return true;

// void OnGetMinMaxInfo(MINMAXINFO* pmmi);
#define TTMSG_WM_GETMINMAXINFO(func) \
	case WM_GETMINMAXINFO: \
		func((MINMAXINFO*) lParam); \
		return true;

// void OnIconEraseBknd(HDC hdc);
#define TTMSG_WM_ICONERASEBKGND(func) \
	case WM_ICONERASEBKGND: \
		func((HDC) wParam); \
		return true;

// void OnSpoolerStatus(size_t flag, size_t cJobsLeft);
#define TTMSG_WM_SPOOLERSTATUS(func) \
	case WM_SPOOLERSTATUS: \
		func((size_t) wParam, (size_t) LOWORD(lParam)); \
		return true;

// BOOL OnDrawItem(size_t id, DRAWITEMSTRUCT* pdis)
#define TTMSG_WM_DRAWITEM(func) \
	case WM_DRAWITEM: \
		lResult = (LRESULT) func((size_t) wParam, (DRAWITEMSTRUCT*) lParam); \
		return true;

// BOOL OnMeasureItem(size_t id, MEASUREITEMSTRUCT* pmis);
#define TTMSG_WM_MEASUREITEM(func) \
	case WM_MEASUREITEM: \
		lResult = (LRESULT) func((size_t) wParam, (MEASUREITEMSTRUCT*) lParam); \
		return true;

// void OnDeleteItem(size_t id, DELETEITEMSTRUCT* pdis);
#define TTMSG_WM_DELETEITEM(func) \
	case WM_DELETEITEM: \
		lResult = TRUE;	\
		func((size_t) wParam, (DELETEITEMSTRUCT*) lParam); \
		return true;

// LRESULT OnCharToItem(size_t key, size_t CaretPos, HWND hwndLB);
#define TTMSG_WM_CHARTOITEM(func) \
	case WM_CHARTOITEM: \
		lResult = func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam), (HWND) lParam); \
		return true;

// LRESULT OnVKeyToItem(size_t key, size_t CaretPos, HWND hwndLB);
#define TTMSG_WM_VKEYTOITEM(func) \
	case WM_VKEYTOITEM: \
		lResult = func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam), (HWND) lParam); \
		return true;

// HANDLE OnQueryDragIcon();
#define TTMSG_WM_QUERYDRAGICON(func) \
	case WM_QUERYDRAGICON: \
		lResult = (LRESULT) func(); \
		return true;

// LRESULT OnCompareItem(size_t idCtrl, COMPAREITEMSTRUCT* pcis);
#define TTMSG_WM_COMPAREITEM(func) \
	case WM_COMPAREITEM: \
		lResult = func((size_t) wParam, (COMPAREITEMSTRUCT*) lParam); \
		return true;

// BOOL OnNcCreate(CREATESTRUCT* pcs);
#define TTMSG_WM_NCCREATE(func) \
	case WM_NCCREATE: \
		lResult = (LRESULT) func((CREATESTRUCT*) lParam); \
		return true;

// void OnNcDestroy();
#define TTMSG_WM_NCDESTROY(func) \
	case WM_NCDESTROY: \
		func(); \
		return true;

// LRESULT OnNcCalcSize(BOOL bValid, NCCALCSIZE_PARAMS* pncsp);
#define TTMSG_WM_NCCALCSIZE(func) \
	case WM_NCCALCSIZE: \
		lResult = func((BOOL) wParam, (NCCALCSIZE_PARAMS*) lParam); \
		return true;

// LRESULT OnNcHitTest(int xPos, int yPos);
#define TTMSG_WM_NCHITTEST(func) \
	case WM_NCHITTEST: \
		lResult = (LRESULT) func(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcPaint(HRGN hrgn);
#define TTMSG_WM_NCPAINT(func) \
	case WM_NCPAINT: \
		func((HRGN) wParam); \
		return true;

// BOOL OnNcActivate(BOOL bActive);
#define TTMSG_WM_NCACTIVATE(func) \
	case WM_NCACTIVATE: \
		lResult = (LRESULT) func((BOOL) wParam); \
		return true;

// LRESULT OnGetDlgCode(MSG* pmsg);
#define TTMSG_WM_GETDLGCODE(func) \
	case WM_GETDLGCODE: \
		lResult = func((MSG*) lParam); \
		return true;

// void OnNcMouseMove(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMOUSEMOVE(func) \
	case WM_NCMOUSEMOVE: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcLBtnDwn(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCLBUTTONDOWN(func) \
	case WM_NCLBUTTONDOWN: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcLBtnUp(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCLBUTTONUP(func) \
	case WM_NCLBUTTONUP: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcLBtnDblClk(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCLBUTTONDBLCLK(func) \
	case WM_NCLBUTTONDBLCLK: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcRBtnDwn(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCRBUTTONDOWN(func) \
	case WM_NCRBUTTONDOWN: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcRBtnUp(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCRBUTTONUP(func) \
	case WM_NCRBUTTONUP: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcRBtnDblClk(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCRBUTTONDBLCLK(func) \
	case WM_NCRBUTTONDBLCLK: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcMBtnDwn(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMBUTTONDOWN(func) \
	case WM_NCMBUTTONDOWN: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcMBtnUp(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMBUTTONUP(func) \
	case WM_NCMBUTTONUP: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnNcMBtnDblClk(size_t hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMBUTTONDBLCLK(func) \
	case WM_NCMBUTTONDBLCLK: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnKeyDown(size_t virtKey, size_t nRepCnt, size_t nFlags);
#define TTMSG_WM_KEYDOWN(func) \
	case WM_KEYDOWN: \
		func((size_t) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnKeyUp(size_t virtKey, size_t cReps, size_t flags);
#define TTMSG_WM_KEYUP(func) \
	case WM_KEYUP: \
		func((size_t) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnChar(char ch, size_t cReps, size_t flags);
#define TTMSG_WM_CHAR(func) \
	case WM_CHAR: \
		func((char) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnDeadChar(char ch, size_t cReps, size_t flags);
#define TTMSG_WM_DEADCHAR(func) \
	case WM_DEADCHAR: \
		func((char) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnSysKeyDown(size_t virtKey, size_t cReps, size_t flags);
#define TTMSG_WM_SYSKEYDOWN(func) \
	case WM_SYSKEYDOWN: \
		func((size_t) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnSysKeyUp(size_t virtKey, size_t cReps, size_t flags);
#define TTMSG_WM_SYSKEYUP(func) \
	case WM_SYSKEYUP: \
		func((size_t) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnSysChar(char ch, size_t cReps, size_t flags);
#define TTMSG_WM_SYSCHAR(func) \
	case WM_SYSCHAR: \
		func((char) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnSysDeadChar(char ch, UINT cReps, UINT flags);
#define TTMSG_WM_SYSDEADCHAR(func) \
	case WM_SYSDEADCHAR: \
		func((char) wParam, (size_t) lParam & 0xFFFF, (size_t) ((lParam & 0xFFFF0000) >> 16)); \
		return true;

// void OnSysCommand(size_t cmdTYpe, int x, int y);
#define TTMSG_WM_SYSCOMMAND(func) \
	case WM_SYSCOMMAND: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnTCard(size_t idAction, long data);
#define TTMSG_WM_TCARD(func) \
	case WM_TCARD: \
		func((size_t) wParam, (long) lParam); \
		return true;

// void OnTimer(size_t id);
#define TTMSG_WM_TIMER(func) \
	case WM_TIMER: \
		func((size_t) wParam); \
		return true;

// void OnHScroll(size_t SBCode, size_t pos, HWND hwndSB);
#define TTMSG_WM_HSCROLL(func) \
	case WM_HSCROLL: \
		func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam), (HWND) lParam); \
		return true;

// void OnVScroll(size_t SBCode, size_t pos, HWND hwndSB);
#define TTMSG_WM_VSCROLL(func) \
	case WM_VSCROLL: \
		func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam), (HWND) lParam); \
		return true;

// void OnInitMenu(HMENU hMenu);
#define TTMSG_WM_INITMENU(func) \
	case WM_INITMENU: \
		func((HMENU) wParam); \
		return true;

// void OnInitMenuPopup(HMENU hSubMenu, size_t pos, BOOL bSysMenu);
#define TTMSG_WM_INITMENUPOPUP(func) \
	case WM_INITMENUPOPUP: \
		func((HMENU) wParam, (size_t) LOWORD(lParam), (BOOL) HIWORD(lParam)); \
		return true;

// void OnMenuSelect(size_t id, size_t flags, HMENU hMenu);
#define TTMSG_WM_MENUSELECT(func) \
	case WM_MENUSELECT: \
		func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam), (HMENU) lParam); \
		return true;

// LRESULT OnMenuChar(char ch, size_t flag, HMENU hMenu);
#define TTMSG_WM_MENUCHAR(func) \
	case WM_MENUCHAR: \
		lResult = func((char) LOWORD(wParam), (size_t) HIWORD(wParam), (HMENU) lParam); \
		return true;

// LRESULT OnNotify(size_t idCtrl, NMHDR* pnmh);
#define TTMSG_WM_NOTIFY(func) \
	case WM_NOTIFY: \
		lResult = func((size_t) wParam, (NMHDR*) lParam); \
		return true;

// void OnEnterIdle(size_t flag, HWND hwnd);
#define TTMSG_WM_ENTERIDLE(func) \
	case WM_ENTERIDLE: \
		func((size_t) wParam, (HWND) lParam); \
		return true;

// void OnMouseMove(size_t flags, int xPos, int yPos);
#define TTMSG_WM_MOUSEMOVE(func) \
	case WM_MOUSEMOVE: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnMouseWheel(size_t flags, ptrdiff_t delta, int xPos, int yPos);
#define TTMSG_WM_MOUSEWHEEL(func) \
	case WM_MOUSEWHEEL: \
		func((size_t) LOWORD(wParam), (ptrdiff_t) HIWORD(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnLBtnDwn(size_t flags, int xPos, int yPos);
#define TTMSG_WM_LBUTTONDOWN(func) \
	case WM_LBUTTONDOWN: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnLBtnUp(size_t flags, int xPos, int yPos);
#define TTMSG_WM_LBUTTONUP(func) \
	case WM_LBUTTONUP: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnLBtnDblClk(size_t flags, int xPos, int yPos);
#define TTMSG_WM_LBUTTONDBLCLK(func) \
	case WM_LBUTTONDBLCLK: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnRBtnDwn(size_t flags, int xPos, int yPos);
#define TTMSG_WM_RBUTTONDOWN(func) \
	case WM_RBUTTONDOWN: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnRBtnUp(size_t flags, int xPos, int yPos);
#define TTMSG_WM_RBUTTONUP(func) \
	case WM_RBUTTONUP: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnRBtnDblClk(size_t flags, int xPos, int yPos);
#define TTMSG_WM_RBUTTONDBLCLK(func) \
	case WM_RBUTTONDBLCLK: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnMBtnDwn(size_t flags, int xPos, int yPos);
#define TTMSG_WM_MBUTTONDOWN(func) \
	case WM_MBUTTONDOWN: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnMBtnUp(size_t flags, int xPos, int yPos);
#define TTMSG_WM_MBUTTONUP(func) \
	case WM_MBUTTONUP: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnMBtnDblClk(size_t flags, int xPos, int yPos);
#define TTMSG_WM_MBUTTONDBLCLK(func) \
	case WM_MBUTTONDBLCLK: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnEnable(BOOL bEnable);
#define TTMSG_WM_ENABLE(func) \
	case WM_ENABLE: \
		func((BOOL) wParam); \
		return true;

// void OnParentNotify(size_t event, size_t idChild, LPARAM lParam);
#define TTMSG_WM_PARENTNOTIFY(func) \
	case WM_PARENTNOTIFY: \
		func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam), lParam); \
		return true;

// void OnMDIActivate(HWND hwndActivate, HWND hwndDeactivate);
#define TTMSG_WM_MDIACTIVATE(func) \
	case WM_MDIACTIVATE: \
		func((HWND) wParam, (HWND) lParam); \
		return true;

// void OnRenderFormat(size_t nFormat);
#define TTMSG_WM_RENDERFORMAT(func) \
	case WM_RENDERFORMAT: \
		func((size_t) wParam); \
		return true;

// void OnRenderAllFormats();
#define TTMSG_WM_RENDERALLFORMATS(func) \
	case WM_RENDERALLFORMATS: \
		func(); \
		return true;

// void OnContextMenu(HWND hwnd, int xPos, int yPos);
#define TTMSG_WM_CONTEXTMENU(func) \
	case WM_CONTEXTMENU: \
		func((HWND) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnDestroyClipboard();
#define TTMSG_WM_DESTROYCLIPBOARD(func) \
	case WM_DESTROYCLIPBOARD: \
		func(); \
		return true;

// void OnDrawClipboard();
#define TTMSG_WM_DRAWCLIPBOARD(func) \
	case WM_DRAWCLIPBOARD: \
		func(); \
		return true;

// void OnPaintClipboard(HWND hwndViewer, const PAINTSTRUCT* pps);
#define TTMSG_WM_PAINTCLIPBOARD(func) \
	case WM_PAINTCLIPBOARD: \
		func((HWND) wParam, (const PAINTSTRUCT*) GlobalLock((HGLOBAL) lParam)); \
		GlobalUnlock((HGLOBAL) lParam); \
		return true;

// void OnSizeClipboard(HWND hwndViewer, const RECT* prc);
#define TTMSG_WM_SIZECLIPBOARD(func) \
	case WM_SIZECLIPBOARD: \
		func((HWND) wParam, (const RECT*) GlobalLock((HGLOBAL) lParam)); \
		GlobalUnlock((HGLOBAL) lParam); \
		return true;

// void OnVScrollClipboard(HWND hwndViewer, size_t event, size_t pos);
#define TTMSG_WM_VSCROLLCLIPBOARD(func) \
	case WM_VSCROLLCLIPBOARD: \
		func((HWND) wParam, (size_t) LOWORD(lParam), (size_t) HIWORD(lParam)); \
		return true;

// void OnAskCbFormatName(size_t cch, char* pszName);
#define TTMSG_WM_ASKCBFORMATNAME(func) \
	case WM_ASKCBFORMATNAME: \
		func((size_t) wParam, (char*) lParam); \
		return true;

// void OnChangeCbChain(HWND hwndRemove, HWND hwndAfter);
#define TTMSG_WM_CHANGECBCHAIN(func) \
	case WM_CHANGECBCHAIN: \
		func((HWND) wParam, (HWND) lParam); \
		return true;

// void OnHScrollClipboard(HWND hwndViewer, size_t event, size_t pos);
#define TTMSG_WM_HSCROLLCLIPBOARD(func) \
	case WM_HSCROLLCLIPBOARD: \
		func((HWND) wParam, (size_t) LOWORD(lParam), (size_t) HIWORD(lParam)); \
		return true;

// BOOL OnQueryNewPalette();
#define TTMSG_WM_QUERYNEWPALETTE(func) \
	case WM_QUERYNEWPALETTE: \
		lResult = (LRESULT) func(); \
		return true;

// void OnPaletteChanged(HWND hwndChanged);
#define TTMSG_WM_PALETTECHANGED(func) \
	case WM_PALETTECHANGED: \
		func((HWND) wParam); \
		return true;

// void OnPaletteIsChanging(HWND hwndChanging);
#define TTMSG_WM_PALETTEISCHANGING(func) \
	case WM_PALETTEISCHANGING: \
		func((HWND) wParam); \
		return true;

// void OnDropFiles(HDROP hDrop);
#define TTMSG_WM_DROPFILES(func) \
	case WM_DROPFILES: \
		func((HDROP) wParam); \
		return true;

// void OnWindowPosChanging(PWINDOWPOS* pWndPos);
#define TTMSG_WM_WINDOWPOSCHANGING(func) \
	case WM_WINDOWPOSCHANGING: \
		func((PWINDOWPOS*) lParam); \
		return true;

// void OnHelp(HELPINFO* phi);
#define TTMSG_WM_HELP(func) \
	case WM_HELP: \
		func((HELPINFO*) lParam); \
		return true;

// void OnHotKey(size_t idKey, size_t uModifiers, size_t uVirtKey);
#define TTMSG_WM_HOTKEY(func) \
	case WM_HOTKEY: \
		func((size_t) wParam, (size_t) LOWORD(lParam), (size_t) HIWORD(lParam)); \
		return true;

// void OnInputLangChange(size_t charset, HKL hKbdLayout);
#define TTMSG_WM_INPUTLANGCHANGE(func) \
	case WM_INPUTLANGCHANGE: \
		func((size_t) wParam, (HKL) lParam); \
		return true;

// void OnInputLangChangeRequest(size_t flag, HKL hKbdLayout);
#define TTMSG_WM_INPUTLANGCHANGEREQUEST(func) \
	case WM_INPUTLANGCHANGEREQUEST: \
		func((size_t) wParam, (HKL) lParam); \
		return true;

// void OnNextDlgCtl(size_t idFlag, BOOL bFlagType);
#define TTMSG_WM_NEXTDLGCTL(func) \
	case WM_NEXTDLGCTL: \
		func((size_t) wParam, (BOOL) LOWORD(lParam)); \
		return true;

// void OnNextMenu(size_t VirtKey, MDINEXTMENU* pMdiNextMenu);
#define TTMSG_WM_NEXTMENU(func) \
	case WM_NEXTMENU: \
		func((size_t) wParam, (MDINEXTMENU*) lParam); \
		return true;

// LRESULT OnNotifyFormat(HWND hwndFrom, size_t cmd);
#define TTMSG_WM_NOTIFYFORMAT(func) \
	case WM_NOTIFYFORMAT: \
		lResult = func((HWND) wParam, (size_t) lParam); \
		return true;

// LRESULT OnPowerBroadcast(size_t event, POWERBROADCAST_SETTING* pPBDSetting);
#define TTMSG_WM_POWERBROADCAST(func) \
	case WM_POWERBROADCAST: \
		lResult = func((DWORD) wParam, (POWERBROADCAST_SETTING*) lParam); \
		return true;

// void OnPrint(HDC hdc, size_t drwFlags);
#define TTMSG_WM_PRINT(func) \
	case WM_PRINT: \
		func((HDC) wParam, (size_t) lParam); \
		return true;

// void OnPrintClient(HDC hdc, size_t drwFlags);
#define TTMSG_WM_PRINTCLIENT(func) \
	case WM_PRINTCLIENT: \
		func((HDC) wParam, (size_t) lParam); \
		return true;

// BOOL OnRasDialEvent(RASCONNSTATE rasconnstate, DWORD dwError);
#define TTMSG_WM_RASDIALEVENT(func) \
	case WM_RASDIALEVENT: \
		lResult = (LRESULT) func((RASCONNSTATE) wParam, (DWORD) lParam); \
		return true;

// void OnSetFont(HANDLE hFont, BOOL bRedraw);
#define TTMSG_WM_SETFONT(func) \
	case WM_SETFONT: \
		func((HFONT) wParam, (BOOL) LOWORD(lParam)); \
		return true;

// LRESULT OnSetHotKey(size_t VirtKey, size_t uFlags);
#define TTMSG_WM_SETHOTKEY(func) \
	case WM_SETHOTKEY: \
		lResult = func((size_t) LOBYTE(LOWORD(wParam)), (size_t) HIBYTE(LOWORD(wParam))); \
		return true;

// HICON OnSetIcon(size_t uType, HICON hIcon);
#define TTMSG_WM_SETICON(func) \
	case WM_SETICON: \
		lResult = (LRESULT) func((size_t) wParam, (HICON) lParam); \
		return true;

// void OnSetRedraw(BOOL bRedraw);
#define TTMSG_WM_SETREDRAW(func) \
	case WM_SETREDRAW: \
		func((BOOL) wParam); \
		return true;

// LRESULT OnSetText(const char* pszText);
#define TTMSG_WM_SETTEXT(func) \
	case WM_SETTEXT: \
		lResult = (LRESULT) func((const char*) lParam); \
		return true;

// void OnUserChanged();
#define TTMSG_WM_USERCHANGED(func) \
	case WM_USERCHANGED: \
		func(); \
		return true;

// void OnMouseHover(size_t flags, int xPos, int yPos);
#define TTMSG_WM_MOUSEHOVER(func) \
	case WM_MOUSEHOVER: \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnMouseLeave();
#define TTMSG_WM_MOUSELEAVE(func) \
	case WM_MOUSELEAVE: \
		func(); \
		return true;

// void OnMenuRButtonUp(size_t index, HANDLE hMenu);
#define TTMSG_WM_MENURBUTTONUP(func) \
	case WM_MENURBUTTONUP: \
		func((size_t) wParam, (HMENU) lParam); \
		return true;

// LRESULT OnMenuDrag(size_t pos, HANDLE hMenu);
#define TTMSG_WM_MENUDRAG(func) \
	case WM_MENUDRAG: \
		lResult = func(wParam, (HMENU) lParam); \
		return true;

// LRESULT OnMenuGetObject(MENUGETOBJECTINFO* pinfo);
#define TTMSG_WM_MENUGETOBJECT(func) \
	case WM_MENUGETOBJECT: \
		lResult = func((MENUGETOBJECTINFO*) lParam); \
		return true;

// void OnUnInitMenuPopup(HANDLE hMenu, size_t flag);
#define TTMSG_WM_UNINITMENUPOPUP(func) \
	case WM_UNINITMENUPOPUP: \
		func((HMENU) wParam, (size_t) HIWORD(lParam)); \
		return true;

// void OnMenuCommand(size_t index, HANDLE hMenu);
#define TTMSG_WM_MENUCOMMAND(func) \
	case WM_MENUCOMMAND: \
		func((size_t) wParam, (HMENU) lParam); \
		return true;

// BOOL OnAppCommand(HWND hwnd, size_t cmd, size_t device, size_t keyFlags);
#define TTMSG_WM_APPCOMMAND(func) \
	case WM_APPCOMMAND: \
		lResult = (LRESULT) func((HWND) wParam, (size_t) GET_APPCOMMAND_LPARAM(lParam), (size_t) GET_DEVICE_LPARAM(lParam), (size_t) GET_KEYSTATE_LPARAM(lParam)); \
		return true;

// BOOL OnNCXButtonDown(size_t fwButton, size_t HitTest, int xPos, int yPos);
#define TTMSG_WM_NCXBUTTONDOWN(func) \
	case WM_NCXBUTTONDOWN: \
		lResult = (LRESULT) func((size_t) GET_XBUTTON_WPARAM(wParam), (size_t) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// BOOL OnNCXButtonUp(size_t fwButton, size_t HitTest, int xPos, int yPos);
#define TTMSG_WM_NCXBUTTONUP(func) \
	case WM_NCXBUTTONUP: \
		lResult = (LRESULT) func((size_t) GET_XBUTTON_WPARAM(wParam), (size_t) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// BOOL OnNCXButtonDblClk(size_t fwButton, size_t HitTest, int xPos, int yPos);
#define TTMSG_WM_NCXBUTTONDBLCLK(func) \
	case WM_NCXBUTTONDBLCLK: \
		lResult = (LRESULT) func((size_t) GET_XBUTTON_WPARAM(wParam), (size_t) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// BOOL OnXButtonDown(size_t fwButton, size_t HitTest, int xPos, int yPos);
#define TTMSG_WM_XBUTTONDOWN(func) \
	case WM_XBUTTONDOWN: \
		lResult = (LRESULT) func((size_t) GET_XBUTTON_WPARAM(wParam), (size_t) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// BOOL OnXButtonUp(size_t fwButton, size_t HitTest, int xPos, int yPos);
#define TTMSG_WM_XBUTTONUP(func) \
	case WM_XBUTTONUP: \
		lResult = (LRESULT) func((size_t) GET_XBUTTON_WPARAM(wParam), (size_t) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// BOOL OnXButtonDblClk(size_t fwButton, size_t HitTest, int xPos, int yPos);
#define TTMSG_WM_XBUTTONDBLCLK(func) \
	case WM_XBUTTONDBLCLK: \
		lResult = (LRESULT) func((size_t) GET_XBUTTON_WPARAM(wParam), (size_t) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnChangeUIState(size_t action, size_t state);
#define TTMSG_WM_CHANGEUISTATE(func) \
	case WM_CHANGEUISTATE: \
		func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam)); \
		return true;

// void OnUpdateUIState(size_t action, size_t state);
#define TTMSG_WM_UPDATEUISTATE(func) \
	case WM_UPDATEUISTATE: \
		func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam)); \
		return true;

// LRESULT OnQueryUIState()
#define TTMSG_WM_QUERYUISTATE(func) \
	case WM_QUERYUISTATE: \
		lResult = func(); \
		return true;

// void OnInput(size_t RawInputCode, HRAWINPUT hRawInput)
#define TTMSG_WM_INPUT(func) \
	case WM_INPUT: \
		func((size_t) GET_RAWINPUT_CODE_WPARAM(wParam), (HRAWINPUT) lParam); \
		return true;

// void OnThemeChanged()
#define TTMSG_WM_THEMECHANGED(func) \
	case WM_THEMECHANGED: \
		func(); \
		return true;

// BOOL OnMouseHWheel(size_t flags, ptrdiff_t delta, int xPos, int yPos);
#define TTMSG_WM_MOUSEHWHEEL(func) \
	case WM_MOUSEHWHEEL: \
		lResult = (LRESULT) func((size_t) LOWORD(wParam), (ptrdiff_t) HIWORD(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true;

// void OnGesture(ULONGLONG ullArguments, HGESTUREINFO hGestureInfo);
#define TTMSG_WM_GESTURE(func) \
	case WM_GESTURE: \
		func((ULONGLONG) wParam, (HGESTUREINFO) lParam); \
		return true;

// Note that return is set to false because DefWindwProc must be called even if this message is handled
// void OnGestureNotify(GESTURENOTIFYSTRUCT* pGestureNotifyStruct);
#define TTMSG_WM_GESTURENOTIFY(func) \
	case WM_GESTURENOTIFY: \
		func((GESTURENOTIFYSTRUCT*) lParam); \
		return false;

// void OnDpiChanged(size_t nDpiX, size_t nDpiY, RECT* prc);
#define TTMSG_WM_DPICHANGED(func) \
	case WM_DPICHANGED: \
		func((size_t) LOWORD(wParam), (size_t) HIWORD(wParam), (RECT*) lParam); \
		return true;

// LRESULT func(WPARAM wParam, LPARAM lParam);
#define TTMSG_ANY(msg, func) \
	case msg: \
		lResult = func(wParam, lParam); \
		return true; \

#endif	// not __TTLIB_TTMSG_MAP_H__
