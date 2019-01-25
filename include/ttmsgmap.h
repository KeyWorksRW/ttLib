/////////////////////////////////////////////////////////////////////////////
// Name:		ttmsgmap.h
// Purpose:		Macros for mapping Windows messages to functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Most names of the macros below are the same as in atlcrack.h that's part of WTL (Windows Template Library). In most
// cases, the functions use the same parameters unless they used ATL or WTL specific tempaltes/classes. The body of the
// macro is defined very differently. That means you can use the same macro name, but you must use this header file and
// not the atlcrack.h header if you are implementing a BEGIN_TTMSG_MAP()/END_TTMSG_MAP() block.

#ifdef __ATLCRACK_H__
	#error You cannot inlcude atlcrack.h when including this header file
#endif

/*
	Place the following within a class that inherits from ttDlg or ttWin:

	BEGIN_TTMSG_MAP()
		// Insert as many MSG_() macros as you need to process
	END_TTMSG_MAP()

	If you need to return a non-zero result to Windows, set the m_result value before your function ends. All macros return true to
	indicate that you handled the message.

	For each macro you process you will need to declare a matching function. For example, to process a button click in a dialog:

	BEGIN_TTMSG_MAP()
		MSG_BUTTON_CLICK(IDBTN_MYBTN, OnMyBtn)
	END_TTMSG_MAP()

	void OnMyBtn();

*/

#pragma once

#ifndef __TTLIB_TTMSG_MAP_H__
#define __TTLIB_TTMSG_MAP_H__

#define BEGIN_TTMSG_MAP() bool OnMsgMap(UINT uMsg, WPARAM wParam, LPARAM lParam) { uMsg; wParam; lParam; m_result = 0;

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp) ((int) (short) LOWORD(lp))	 // note that this returns a signed value
#endif
#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lp) ((int) (short) HIWORD(lp))	 // note that this returns a signed value
#endif

#define MSG_BUTTON_CLICK(id, func) \
	if (uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define MSG_EDIT_CHANGE(id, func) \
	if (uMsg == WM_COMMAND && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == CBN_EDITCHANGE) && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define MSG_SEL_CHANGE(id, func) \
	if (uMsg == WM_COMMAND && (HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == CBN_SELCHANGE) && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define MSG_LISTBOX_DBL_CLICK(id, func) \
	if (uMsg == WM_COMMAND && HIWORD(wParam) == LBN_DBLCLK && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define MSG_COMMAND_HANDLER(id, func) \
	if (uMsg == WM_COMMAND && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define MSG_NOTIFY_HANDLER(id, func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR) lParam)->idFrom == (UINT) id) { \
		m_result = func((LPNMHDR) lParam); \
		return true; \
	}

#define MSG_WMP_HANDLER(msg, func) \
	if (uMsg == msg) { \
		func(wParam, lParam); \
		return true; \
	}

#define MSG_GETDISPINFO(func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR) lParam)->code == LVN_GETDISPINFO) { \
		func((NMLVDISPINFO*) lParam); \
		return true; \
	}
#define MSG_WM_NCMOUSEMOVE(func) \
	if (uMsg == WM_NCMOUSEMOVE) { \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_MOVE(func) \
	if (uMsg == WM_MOVE) { \
		func(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_LBUTTONDBLCLK(func) \
	if (uMsg == WM_LBUTTONDBLCLK) { \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_SIZE(func) \
	if (uMsg == WM_SIZE) { \
		func((size_t) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))); \
		return true; \
	}

#define MSG_WM_NCHITTEST(func) \
	if (uMsg == WM_NCHITTEST) { \
		m_result = (LRESULT) func(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_NCLBUTTONDOWN(func) \
	if (uMsg == WM_NCLBUTTONDOWN) { \
		func((UINT) wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCLBUTTONUP(func) \
	if (uMsg == WM_NCLBUTTONUP) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCLBUTTONDBLCLK(func) \
	if (uMsg == WM_NCLBUTTONDBLCLK) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCRBUTTONDOWN(func) \
	if (uMsg == WM_NCRBUTTONDOWN) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCRBUTTONUP(func) \
	if (uMsg == WM_NCRBUTTONUP) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCRBUTTONDBLCLK(func) \
	if (uMsg == WM_NCRBUTTONDBLCLK) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCMBUTTONDOWN(func) \
	if (uMsg == WM_NCMBUTTONDOWN) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCMBUTTONUP(func) \
	if (uMsg == WM_NCMBUTTONUP) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCMBUTTONDBLCLK(func) \
	if (uMsg == WM_NCMBUTTONDBLCLK) { \
		func((UINT)wParam, (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_MOUSEMOVE(func) \
	if (uMsg == WM_MOUSEMOVE) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_MOUSEWHEEL(func) \
	if (uMsg == WM_MOUSEWHEEL) { \
		m_result = (LRESULT)func((UINT)LOWORD(wParam), (short)HIWORD(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
		}

#define MSG_WM_LBUTTONDOWN(func) \
	if (uMsg == WM_LBUTTONDOWN) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_MOUSEHOVER(func) \
	if (uMsg == WM_MOUSEHOVER) { \
		func(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_LBUTTONUP(func) \
	if (uMsg == WM_LBUTTONUP) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_RBUTTONDOWN(func) \
	if (uMsg == WM_RBUTTONDOWN) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_RBUTTONUP(func) \
	if (uMsg == WM_RBUTTONUP) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_RBUTTONDBLCLK(func) \
	if (uMsg == WM_RBUTTONDBLCLK) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_MBUTTONDOWN(func) \
	if (uMsg == WM_MBUTTONDOWN) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_MBUTTONUP(func) \
	if (uMsg == WM_MBUTTONUP) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_MBUTTONDBLCLK(func) \
	if (uMsg == WM_MBUTTONDBLCLK) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_CONTEXTMENU(func) \
	if (uMsg == WM_CONTEXTMENU) { \
		func((HWND)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_DISPLAYCHANGE(func) \
	if (uMsg == WM_DISPLAYCHANGE) { \
		func((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

// func(hittest, whichbutton, pts)
#define MSG_WM_NCXBUTTONDOWN(func) \
	if (uMsg == WM_NCXBUTTONDOWN) { \
		func(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam), (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCXBUTTONUP(func) \
	if (uMsg == WM_NCXBUTTONUP) { \
		func(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam), (POINTS) lParam); \
		return true; \
	}

#define MSG_WM_NCXBUTTONDBLCLK(func) \
	if (uMsg == WM_NCXBUTTONDBLCLK) { \
		func(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam), (POINTS) lParam); \
		return true; \
	}

// func(vkey, whichbutton, pts)
#define MSG_WM_XBUTTONDOWN(func) \
	if (uMsg == WM_XBUTTONDOWN) { \
		func(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_XBUTTONUP(func) \
	if (uMsg == WM_XBUTTONUP) { \
		func(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_XBUTTONDBLCLK(func) \
	if (uMsg == WM_XBUTTONDBLCLK) { \
		func(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_CREATE(func) \
	if (uMsg == WM_CREATE) { \
		m_result = (LRESULT) func((LPCREATESTRUCT) lParam); \
		return true; \
	}

#define MSG_WM_INITDIALOG(func) \
	if (uMsg == WM_INITDIALOG) { \
		m_result = (LRESULT) func((HWND) wParam, lParam); \
		return true; \
	}

#define MSG_WM_COPYDATA(func) \
	if (uMsg == WM_COPYDATA) \
	{ \
		m_result = (LRESULT) func((HWND) wParam, (PCOPYDATASTRUCT) lParam); \
		return true; \
	}

#define MSG_WM_DESTROY(func) \
	if (uMsg == WM_DESTROY)
	{ \
		func(); \
		return true; \
	}

#define MSG_WM_ACTIVATE(func) \
	if (uMsg == WM_ACTIVATE) { \
		func((UINT) LOWORD(wParam), (BOOL) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_WM_SETFOCUS(func) \
	if (uMsg == WM_SETFOCUS) { \
		func((HWND)wParam); \
		return true; \
	}

#define MSG_WM_KILLFOCUS(func) \
	if (uMsg == WM_KILLFOCUS) { \
		func((HWND)wParam); \
		return true; \
	}

#define MSG_WM_ENABLE(func) \
	if (uMsg == WM_ENABLE) { \
		func(wParam); \
		return true; \
	}

#define MSG_WM_PAINT(func) \
	if (uMsg == WM_PAINT) { \
		func(); \
		return true; \
	}

#define MSG_WM_CLOSE(func) \
	if (uMsg == WM_CLOSE) { \
		func(); \
		return true; \
	}

#define MSG_WM_QUERYENDSESSION(func) \
	if (uMsg == WM_QUERYENDSESSION) { \
		m_result = (LRESULT) func(); \
		return true; \
	}

#define MSG_WM_QUERYOPEN(func) \
	if (uMsg == WM_QUERYOPEN) { \
		m_result = (LRESULT) func(); \
		return true; \
	}

#define MSG_WM_ERASEBKGND(func) \
	if (uMsg == WM_ERASEBKGND) { \
		m_result = (LRESULT) func((HDC) wParam); \
		return true; \
	}

#define MSG_WM_SYSCOLORCHANGE(func) \
	if (uMsg == WM_SYSCOLORCHANGE) { \
		func(); \
		return true; \
	}

#define MSG_WM_ENDSESSION(func) \
	if (uMsg == WM_ENDSESSION) { \
		func(); \
		return true; \
	}

#define MSG_WM_SHOWWINDOW(func) \
	if (uMsg == WM_SHOWWINDOW) { \
		func((BOOL) wParam, (int) lParam); \
		return true; \
	}

#define MSG_WM_CTLCOLOREDIT(func) \
	if (uMsg == WM_CTLCOLOREDIT) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_CTLCOLORLISTBOX(func) \
	if (uMsg == WM_CTLCOLORLISTBOX) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_CTLCOLORBTN(func) \
	if (uMsg == WM_CTLCOLORBTN) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_CTLCOLORDLG(func) \
	if (uMsg == WM_CTLCOLORDLG) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_CTLCOLORSCROLLBAR(func) \
	if (uMsg == WM_CTLCOLORSCROLLBAR) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_CTLCOLORSTATIC(func) \
	if (uMsg == WM_CTLCOLORSTATIC) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_SETTINGCHANGE(func) \
	if (uMsg == WM_SETTINGCHANGE) { \
		func((UINT) wParam); \
		return true; \
	}

#define MSG_WM_DEVMODECHANGE(func) \
	if (uMsg == WM_DEVMODECHANGE) { \
		func((const char*) lParam); \
		return true; \
	}

#define MSG_WM_ACTIVATEAPP(func) \
	if (uMsg == WM_ACTIVATEAPP) { \
		func((BOOL) wParam); \
		return true; \
	}

#define MSG_WM_FONTCHANGE(func) \
	if (uMsg == WM_FONTCHANGE) { \
		func(); \
		return true; \
	}

#define MSG_WM_TIMECHANGE(func) \
	if (uMsg == WM_TIMECHANGE) { \
		func(); \
		return true; \
	}

#define MSG_WM_CANCELMODE(func) \
	if (uMsg == WM_CANCELMODE) { \
		func(); \
		return true; \
	}

#define MSG_WM_SETCURSOR(func) \
	if (uMsg == WM_SETCURSOR) { \
		m_result = (LRESULT) func((HWND) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
		return true; \
	}

#define MSG_WM_MOUSEACTIVATE(func) \
	if (uMsg == WM_MOUSEACTIVATE) { \
		m_result = (LRESULT) func((HWND) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
		return true; \
	}

#define MSG_WM_CHILDACTIVATE(func) \
	if (uMsg == WM_CHILDACTIVATE) { \
		func(); \
		return true; \
	}

#define MSG_WM_GETMINMAXINFO(func) \
	if (uMsg == WM_GETMINMAXINFO) { \
		func((LPMINMAXINFO)lParam); \
		return true; \
	}

#define MSG_WM_ICONERASEBKGND(func) \
	if (uMsg == WM_ICONERASEBKGND) { \
		func((HDC) wParam); \
		return true; \
	}

#define MSG_WM_SPOOLERSTATUS(func) \
	if (uMsg == WM_SPOOLERSTATUS) { \
		func((UINT) wParam, (UINT) LOWORD(lParam)); \
		return true; \
	}

#define MSG_WM_DRAWITEM(func) \
	if (uMsg == WM_DRAWITEM) { \
		func((LPDRAWITEMSTRUCT) lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_WM_MEASUREITEM(func) \
	if (uMsg == WM_MEASUREITEM) { \
		func((UINT) wParam, (LPMEASUREITEMSTRUCT) lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_WM_DELETEITEM(func) \
	if (uMsg == WM_DELETEITEM) { \
		func((UINT) wParam, (LPDELETEITEMSTRUCT) lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_WM_CHARTOITEM(func) \
	if (uMsg == WM_CHARTOITEM) { \
		m_result = (LRESULT) func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_WM_VKEYTOITEM(func) \
	if (uMsg == WM_VKEYTOITEM) { \
		m_result = (LRESULT) func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_WM_QUERYDRAGICON(func) \
	if (uMsg == WM_QUERYDRAGICON) { \
		m_result = (LRESULT) func(); \
		return true; \
	}

#define MSG_WM_COMPAREITEM(func) \
	if (uMsg == WM_COMPAREITEM) { \
		m_result = (LRESULT) func((UINT) wParam, (LPCOMPAREITEMSTRUCT) lParam); \
		return true; \
	}
#define MSG_WM_COMPACTING(func) \
	if (uMsg == WM_COMPACTING) { \
		func((UINT)wParam); \
		return true; \
	}

#define MSG_WM_NCCREATE(func) \
	if (uMsg == WM_NCCREATE) { \
		m_result = (LRESULT) func((LPCREATESTRUCT) lParam); \
		return true; \
	}

#define MSG_WM_NCDESTROY(func) \
	if (uMsg == WM_NCDESTROY) { \
		func(); \
		return true; \
	}

#define MSG_WM_NCCALCSIZE(func) \
	if (uMsg == WM_NCCALCSIZE) { \
		m_result = func((BOOL) wParam, lParam); \
		return true; \
	}

#define MSG_WM_NCPAINT(func) \
	if (uMsg == WM_NCPAINT) { \
		func((HRGN)wParam); \
		return true; \
	}

#define MSG_WM_NCACTIVATE(func) \
	if (uMsg == WM_NCACTIVATE) { \
		m_result = (LRESULT) func((BOOL) wParam); \
		return true; \
	}

#define MSG_WM_GETDLGCODE(func) \
	if (uMsg == WM_GETDLGCODE) { \
		m_result = (LRESULT) func((LPMSG) lParam); \
		return true; \
	}

#define MSG_WM_KEYDOWN(func) \
	if (uMsg == WM_KEYDOWN) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_KEYUP(func) \
	if (uMsg == WM_KEYUP) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_CHAR(func) \
	if (uMsg == WM_CHAR) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_DEADCHAR(func) \
	if (uMsg == WM_DEADCHAR) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_SYSKEYDOWN(func) \
	if (uMsg == WM_SYSKEYDOWN) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_SYSKEYUP(func) \
	if (uMsg == WM_SYSKEYUP) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_SYSCHAR(func) \
	if (uMsg == WM_SYSCHAR) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_SYSDEADCHAR(func) \
	if (uMsg == WM_SYSDEADCHAR) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		return true; \
	}

#define MSG_WM_SYSCOMMAND(func) \
	if (uMsg == WM_SYSCOMMAND) { \
		func((UINT)wParam)); \
		return true; \
	}

#define MSG_WM_TCARD(func) \
	if (uMsg == WM_TCARD) { \
		func((UINT) wParam, (DWORD_PTR) lParam); \
		return true; \
	}

#define MSG_WM_TIMER(func) \
	if (uMsg == WM_TIMER) { \
		func((UINT)wParam); \
		return true; \
	}

#define MSG_WM_HSCROLL(func) \
	if (uMsg == WM_HSCROLL) { \
		func((int) LOWORD(wParam), (short) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_WM_VSCROLL(func) \
	if (uMsg == WM_VSCROLL) { \
		func((int) LOWORD(wParam), (short) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_WM_INITMENU(func) \
	if (uMsg == WM_INITMENU) { \
		func((HMENU)wParam); \
		return true; \
	}

#define MSG_WM_INITMENUPOPUP(func) \
	if (uMsg == WM_INITMENUPOPUP) { \
		func((HMENU) wParam, (UINT) LOWORD(lParam), (BOOL) HIWORD(lParam)); \
		return true; \
	}

#define MSG_WM_MENUSELECT(func) \
	if (uMsg == WM_MENUSELECT) { \
		func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), (HMENU) lParam); \
		return true; \
	}

#define MSG_WM_MENUCHAR(func) \
	if (uMsg == WM_MENUCHAR) { \
		m_result = func((char) LOWORD(wParam), (UINT) HIWORD(wParam), (HMENU) lParam); \
		return true; \
	}

#define MSG_WM_NOTIFY(func) \
	if (uMsg == WM_NOTIFY) { \
		m_result = func((int) wParam, (LPNMHDR) lParam); \
		return true; \
	}

#define MSG_WM_ENTERIDLE(func) \
	if (uMsg == WM_ENTERIDLE) { \
		func(); \
		return true; \
	}

#define MSG_WM_PARENTNOTIFY(func) \
	if (uMsg == WM_PARENTNOTIFY) { \
		func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), lParam); \
		return true; \
	}

#define MSG_WM_MDIACTIVATE(func) \
	if (uMsg == WM_MDIACTIVATE) { \
		func((HWND) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_RENDERFORMAT(func) \
	if (uMsg == WM_RENDERFORMAT) { \
		func((UINT)wParam); \
		return true; \
	}

#define MSG_WM_RENDERALLFORMATS(func) \
	if (uMsg == WM_RENDERALLFORMATS) { \
		func(); \
		return true; \
	}

#define MSG_WM_DESTROYCLIPBOARD(func) \
	if (uMsg == WM_DESTROYCLIPBOARD) { \
		func(); \
		return true; \
	}

#define MSG_WM_DRAWCLIPBOARD(func) \
	if (uMsg == WM_DRAWCLIPBOARD) { \
		func(); \
		return true; \
	}

#define MSG_WM_PAINTCLIPBOARD(func) \
	if (uMsg == WM_PAINTCLIPBOARD) { \
		func((HWND) wParam, (const LPPAINTSTRUCT) ::GlobalLock((HGLOBAL) lParam)); \
		::GlobalUnlock((HGLOBAL) lParam); \
		return true; \
	}

#define MSG_WM_VSCROLLCLIPBOARD(func) \
	if (uMsg == WM_VSCROLLCLIPBOARD) { \
		func((HWND) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
		return true; \
	}

#define MSG_WM_SIZECLIPBOARD(func) \
	if (uMsg == WM_SIZECLIPBOARD) { \
		func((HWND) wParam, (const LPRECT) ::GlobalLock((HGLOBAL) lParam)); \
		::GlobalUnlock((HGLOBAL)lParam); \
		return true; \
	}

#define MSG_WM_ASKCBFORMATNAME(func) \
	if (uMsg == WM_ASKCBFORMATNAME) { \
		func((DWORD_PTR) wParam, (char*) lParam); \
		return true; \
	}

#define MSG_WM_CHANGECBCHAIN(func) \
	if (uMsg == WM_CHANGECBCHAIN) { \
		func((HWND) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_HSCROLLCLIPBOARD(func) \
	if (uMsg == WM_HSCROLLCLIPBOARD) { \
		func((HWND) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
		return true; \
	}

#define MSG_WM_QUERYNEWPALETTE(func) \
	if (uMsg == WM_QUERYNEWPALETTE) { \
		m_result = (LRESULT) func(); \
		return true; \
	}

#define MSG_WM_PALETTECHANGED(func) \
	if (uMsg == WM_PALETTECHANGED) { \
		func((HWND)wParam); \
		return true; \
	}

#define MSG_WM_PALETTEISCHANGING(func) \
	if (uMsg == WM_PALETTEISCHANGING) { \
		func((HWND) wParam); \
		return true; \
	}

#define MSG_WM_DROPFILES(func) \
	if (uMsg == WM_DROPFILES) { \
		func((HDROP) wParam); \
		return true; \
	}

#define MSG_WM_WINDOWPOSCHANGING(func) \
	if (uMsg == WM_WINDOWPOSCHANGING) { \
		func((LPWINDOWPOS)lParam); \
		return true; \
	}

#define MSG_WM_WINDOWPOSCHANGED(func) \
	if (uMsg == WM_WINDOWPOSCHANGED) { \
		func((LPWINDOWPOS)lParam); \
		return true; \
	}

#define MSG_WM_EXITMENULOOP(func) \
	if (uMsg == WM_EXITMENULOOP) { \
		func((BOOL)wParam); \
		return true; \
	}

#define MSG_WM_ENTERMENULOOP(func) \
	if (uMsg == WM_ENTERMENULOOP) { \
		func((BOOL) wParam); \
		return true; \
	}

#define MSG_WM_STYLECHANGED(func) \
	if (uMsg == WM_STYLECHANGED) { \
		func((UINT) wParam, (LPSTYLESTRUCT) lParam); \
		return true; \
	}

#define MSG_WM_STYLECHANGING(func) \
	if (uMsg == WM_STYLECHANGING) { \
		func((UINT) wParam, (LPSTYLESTRUCT) lParam); \
		return true; \
	}

#define MSG_WM_SIZING(func) \
	if (uMsg == WM_SIZING) { \
		func((UINT) wParam, (LPRECT) lParam); \
		return true; \
	}

#define MSG_WM_MOVING(func) \
	if (uMsg == WM_MOVING) { \
		func((UINT) wParam, (LPRECT) lParam); \
		return true; \
	}

#define MSG_WM_CAPTURECHANGED(func) \
	if (uMsg == WM_CAPTURECHANGED) { \
		func((HWND) lParam); \
		return true; \
	}

#define MSG_WM_DEVICECHANGE(func) \
	if (uMsg == WM_DEVICECHANGE) { \
		m_result = (LRESULT) func((UINT) wParam, (DWORD_PTR) lParam); \
		return true; \
	}

#define MSG_WM_COMMAND(func) \
	if (uMsg == WM_COMMAND) { \
		func((UINT) HIWORD(wParam), (int) LOWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_WM_ENTERSIZEMOVE(func) \
	if (uMsg == WM_ENTERSIZEMOVE) { \
		func(); \
		return true; \
	}

#define MSG_WM_EXITSIZEMOVE(func) \
	if (uMsg == WM_EXITSIZEMOVE) { \
		func(); \
		return true; \
	}

#define MSG_WM_GETFONT(func) \
	if (uMsg == WM_GETFONT) { \
		m_result = (LRESULT) func(); \
		return true; \
	}

#define MSG_WM_GETHOTKEY(func) \
	if (uMsg == WM_GETHOTKEY) { \
		m_result = func(); \
		return true; \
	}

#define MSG_WM_GETICON(func) \
	if (uMsg == WM_GETICON) { \
		m_result = (LRESULT) func((UINT) wParam); \
		return true; \
	}

#define MSG_WM_GETTEXT(func) \
	if (uMsg == WM_GETTEXT) { \
		m_result = (LRESULT) func((int) wParam, (char*) lParam); \
		return true; \
	}

#define MSG_WM_GETTEXTLENGTH(func) \
	if (uMsg == WM_GETTEXTLENGTH) { \
		m_result = (LRESULT) func(); \
		return true; \
	}

#define MSG_WM_HELP(func) \
	if (uMsg == WM_HELP) { \
		func((LPHELPINFO) lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_WM_HOTKEY(func) \
	if (uMsg == WM_HOTKEY) { \
		func((int) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
		return true; \
	}

#define MSG_WM_INPUTLANGCHANGE(func) \
	if (uMsg == WM_INPUTLANGCHANGE) { \
		func((DWORD_PTR) wParam, (HKL) lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_WM_INPUTLANGCHANGEREQUEST(func) \
	if (uMsg == WM_INPUTLANGCHANGEREQUEST) { \
		func((BOOL) wParam, (HKL) lParam); \
		return true; \
	}

#define MSG_WM_NEXTDLGCTL(func) \
	if (uMsg == WM_NEXTDLGCTL) { \
		func((BOOL) LOWORD(lParam), wParam); \
		return true; \
	}

#define MSG_WM_NEXTMENU(func) \
	if (uMsg == WM_NEXTMENU) { \
		func((int) wParam, (LPMDINEXTMENU) lParam); \
		return true; \
	}

#define MSG_WM_NOTIFYFORMAT(func) \
	if (uMsg == WM_NOTIFYFORMAT) { \
		m_result = (LRESULT) func((HWND) wParam, (int) lParam); \
		return true; \
	}

#define MSG_WM_POWERBROADCAST(func) \
	if (uMsg == WM_POWERBROADCAST) { \
		m_result = (LRESULT) func((DWORD_PTR) wParam, (DWORD_PTR) lParam); \
		return true; \
	}

#define MSG_WM_PRINT(func) \
	if (uMsg == WM_PRINT) { \
		func((HDC) wParam, (UINT) lParam); \
		return true; \
	}

#define MSG_WM_PRINTCLIENT(func) \
	if (uMsg == WM_PRINTCLIENT) { \
		func((HDC) wParam, (UINT) lParam); \
		return true; \
	}

#define MSG_WM_RASDIALEVENT(func) \
	if (uMsg == WM_RASDIALEVENT) { \
		func((RASCONNSTATE) wParam, (DWORD_PTR) lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_WM_SETFONT(func) \
	if (uMsg == WM_SETFONT) { \
		func((HFONT) wParam, (BOOL) LOWORD(lParam)); \
		return true; \
	}

#define MSG_WM_SETHOTKEY(func) \
	if (uMsg == WM_SETHOTKEY) { \
		m_result = (LRESULT) func((int) LOBYTE(LOWORD(wParam)), (UINT) HIBYTE(LOWORD(wParam))); \
		return true; \
	}

#define MSG_WM_SETICON(func) \
	if (uMsg == WM_SETICON) { \
		m_result = (LRESULT) func((UINT) wParam, (HICON) lParam); \
		return true; \
	}

#define MSG_WM_SETREDRAW(func) \
	if (uMsg == WM_SETREDRAW) { \
		func((BOOL) wParam); \
		return true; \
	}

#define MSG_WM_SETTEXT(func) \
	if (uMsg == WM_SETTEXT) \
	{ \
		m_result = (LRESULT) func((const char*) lParam); \
		return true; \
	}
#define MSG_WM_USERCHANGED(func) \
	if (uMsg == WM_USERCHANGED) { \
		func(); \
		return true; \
	}

#ifndef WM_MOUSEHOVER
	#define WM_MOUSEHOVER 0x02A1
	#define WM_MOUSELEAVE 0x02A3
#endif

#define MSG_WM_MOUSEHOVER(func) \
	if (uMsg == WM_MOUSEHOVER) { \
		func(wParm, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_MOUSELEAVE(func) \
	if (uMsg == WM_MOUSELEAVE) { \
		func(); \
		return true; \
	}

#define MSG_WM_MENURBUTTONUP(func) \
	if (uMsg == WM_MENURBUTTONUP) { \
		func(wParam, (HMENU) lParam); \
		return true; \
	}

#define MSG_WM_MENUDRAG(func) \
	if (uMsg == WM_MENUDRAG) { \
		m_result = func(wParam, (HMENU) lParam); \
		return true; \
	}

#define MSG_WM_MENUGETOBJECT(func) \
	if (uMsg == WM_MENUGETOBJECT) { \
		m_result = func((PMENUGETOBJECTINFO) lParam); \
		return true; \
	}

#define MSG_WM_UNINITMENUPOPUP(func) \
	if (uMsg == WM_UNINITMENUPOPUP) { \
		func((UINT) HIWORD(lParam), (HMENU) wParam); \
		return true; \
	}

#define MSG_WM_MENUCOMMAND(func) \
	if (uMsg == WM_MENUCOMMAND) { \
		func(wParam, (HMENU)lParam); \
		return true; \
	}

#define MSG_WM_APPCOMMAND(func) \
	if (uMsg == WM_APPCOMMAND) { \
		func(GET_APPCOMMAND_LPARAM(lParam)); \
		return true; \
	}

#define MSG_WM_CHANGEUISTATE(func) \
	if (uMsg == WM_CHANGEUISTATE) { \
		func(LOWORD(wParam), HIWORD(wParam)); \
		return true; \
	}

#define MSG_WM_UPDATEUISTATE(func) \
	if (uMsg == WM_UPDATEUISTATE) { \
		func(LOWORD(wParam), HIWORD(wParam)); \
		return true; \
	}

#define MSG_WM_QUERYUISTATE(func) \
	if (uMsg == WM_QUERYUISTATE) { \
		m_result = func(); \
		return true; \
	}

#define MSG_WM_INPUT(func) \
	if (uMsg == WM_INPUT) \
	{ \
		func(GET_RAWINPUT_CODE_WPARAM(wParam), (HRAWINPUT)lParam); \
		m_result = 0; \
		return true; \
	}

#define MSG_WM_UNICHAR(func) \
	if (uMsg == WM_UNICHAR) { \
		func((char) wParam, (UINT) lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16)); \
		m_result = (wParam == UNICODE_NOCHAR) ? TRUE : FALSE; \
		return true; \
	}

#define MSG_WM_WTSSESSION_CHANGE(func) \
	if (uMsg == WM_WTSSESSION_CHANGE) { \
		func(wParam, (PWTSSESSION_NOTIFICATION) lParam); \
		return true; \
	}

#define MSG_WM_THEMECHANGED(func) \
	if (uMsg == WM_THEMECHANGED) { \
		func(); \
		return true; \
	}

#define MSG_DM_GETDEFID(func) \
	if (uMsg == DM_GETDEFID) { \
		m_result = func(); \
		return true; \
	}

#define MSG_DM_SETDEFID(func) \
	if (uMsg == DM_SETDEFID) { \
		func((UINT)wParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_DM_REPOSITION(func) \
	if (uMsg == DM_REPOSITION) { \
		func(); \
		return true; \
	}

#define MSG_OCM_COMMAND(func) \
	if (uMsg == OCM_COMMAND) { \
		func((UINT) HIWORD(wParam), (int) LOWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_NOTIFY(func) \
	if (uMsg == OCM_NOTIFY) { \
		m_result = func((int) wParam, (LPNMHDR) lParam); \
		return true; \
	}

#define MSG_OCM_PARENTNOTIFY(func) \
	if (uMsg == OCM_PARENTNOTIFY) { \
		func((UINT)LOWORD(wParam), (UINT)HIWORD(wParam), lParam); \
		return true; \
	}

#define MSG_OCM_DRAWITEM(func) \
	if (uMsg == OCM_DRAWITEM) { \
		func((LPDRAWITEMSTRUCT) lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_OCM_MEASUREITEM(func) \
	if (uMsg == OCM_MEASUREITEM) { \
		func((LPMEASUREITEMSTRUCT)lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_OCM_COMPAREITEM(func) \
	if (uMsg == OCM_COMPAREITEM) { \
		m_result = (LRESULT)func((LPCOMPAREITEMSTRUCT)lParam); \
		return true; \
	}

#define MSG_OCM_DELETEITEM(func) \
	if (uMsg == OCM_DELETEITEM) { \
		func((LPDELETEITEMSTRUCT)lParam); \
		m_result = TRUE; \
		return true; \
	}

#define MSG_OCM_VKEYTOITEM(func) \
	if (uMsg == OCM_VKEYTOITEM) { \
		m_result = (LRESULT) func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_CHARTOITEM(func) \
	if (uMsg == OCM_CHARTOITEM) { \
		m_result = (LRESULT) func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_HSCROLL(func) \
	if (uMsg == OCM_HSCROLL) { \
		func((int) LOWORD(wParam), (int) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_VSCROLL(func) \
	if (uMsg == OCM_VSCROLL) { \
		func((int) LOWORD(wParam), (int) HIWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_CTLCOLOREDIT(func) \
	if (uMsg == OCM_CTLCOLOREDIT) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_CTLCOLORLISTBOX(func) \
	if (uMsg == OCM_CTLCOLORLISTBOX) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_CTLCOLORBTN(func) \
	if (uMsg == OCM_CTLCOLORBTN) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_CTLCOLORDLG(func) \
	if (uMsg == OCM_CTLCOLORDLG) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_CTLCOLORSCROLLBAR(func) \
	if (uMsg == OCM_CTLCOLORSCROLLBAR) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_OCM_CTLCOLORSTATIC(func) \
	if (uMsg == OCM_CTLCOLORSTATIC) { \
		m_result = (LRESULT) func((HDC) wParam, (HWND) lParam); \
		return true; \
	}

#define MSG_WM_CLEAR(func) \
	if (uMsg == WM_CLEAR) { \
		func(); \
		return true; \
	}

#define MSG_WM_COPY(func) \
	if (uMsg == WM_COPY) { \
		func(); \
		return true; \
	}

#define MSG_WM_CUT(func) \
	if (uMsg == WM_CUT) { \
		func(); \
		return true; \
	}

#define MSG_WM_PASTE(func) \
	if (uMsg == WM_PASTE) { \
		func(); \
		return true; \
	}

#define MSG_WM_UNDO(func) \
	if (uMsg == WM_UNDO) { \
		func(); \
		return true; \
	}

#define MESSAGE_HANDLER_EX(msg, func) \
	if (uMsg == msg) { \
		m_result = func(uMsg, wParam, lParam); \
		return true; \
	}

#define MESSAGE_RANGE_HANDLER_EX(msgFirst, msgLast, func) \
	if (uMsg >= msgFirst && uMsg <= msgLast) { \
		m_result = func(uMsg, wParam, lParam); \
		return true; \
	}

#define COMMAND_HANDLER_EX(id, code, func) \
	if (uMsg == WM_COMMAND && code == HIWORD(wParam) && id == LOWORD(wParam)) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define COMMAND_ID_HANDLER_EX(id, func) \
	if (uMsg == WM_COMMAND && id == LOWORD(wParam)) \
	{ \
		func((UINT) HIWORD(wParam), (int) LOWORD(wParam), (HWND) lParam); \
		return true; \
	}

#define COMMAND_CODE_HANDLER_EX(code, func) \
	if (uMsg == WM_COMMAND && code == HIWORD(wParam)) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define NOTIFY_HANDLER_EX(id, cd, func) \
	if (uMsg == WM_NOTIFY && cd == ((LPNMHDR)lParam)->code && id == ((LPNMHDR)lParam)->idFrom) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define NOTIFY_ID_HANDLER_EX(id, func) \
	if (uMsg == WM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom) { \
		m_result = func((LPNMHDR) lParam); \
		return true; \
	}

#define NOTIFY_CODE_HANDLER_EX(cd, func) \
	if (uMsg == WM_NOTIFY && cd == ((LPNMHDR)lParam)->code) { \
		m_result = func((LPNMHDR) lParam); \
		return true; \
	}

#define COMMAND_RANGE_HANDLER_EX(idFirst, idLast, func) \
	if (uMsg == WM_COMMAND && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define COMMAND_RANGE_CODE_HANDLER_EX(idFirst, idLast, code, func) \
	if (uMsg == WM_COMMAND && code == HIWORD(wParam) && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define NOTIFY_RANGE_HANDLER_EX(idFirst, idLast, func) \
	if (uMsg == WM_NOTIFY && ((LPNMHDR) lParam)->idFrom >= idFirst && ((LPNMHDR) lParam)->idFrom <= idLast) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define NOTIFY_RANGE_CODE_HANDLER_EX(idFirst, idLast, cd, func) \
	if (uMsg == WM_NOTIFY && cd == ((LPNMHDR) lParam)->code && ((LPNMHDR) lParam)->idFrom >= idFirst && ((LPNMHDR) lParam)->idFrom <= idLast) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define REFLECTED_COMMAND_HANDLER_EX(id, code, func) \
	if (uMsg == OCM_COMMAND && code == HIWORD(wParam) && id == LOWORD(wParam)) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define REFLECTED_COMMAND_ID_HANDLER_EX(id, func) \
	if (uMsg == OCM_COMMAND && id == LOWORD(wParam)) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define REFLECTED_COMMAND_CODE_HANDLER_EX(code, func) \
	if (uMsg == OCM_COMMAND && code == HIWORD(wParam)) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define REFLECTED_NOTIFY_HANDLER_EX(id, cd, func) \
	if (uMsg == OCM_NOTIFY && cd == ((LPNMHDR)lParam)->code && id == ((LPNMHDR)lParam)->idFrom) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define REFLECTED_NOTIFY_ID_HANDLER_EX(id, func) \
	if (uMsg == OCM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define REFLECTED_NOTIFY_CODE_HANDLER_EX(cd, func) \
	if (uMsg == OCM_NOTIFY && cd == ((LPNMHDR)lParam)->code) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define REFLECTED_COMMAND_RANGE_HANDLER_EX(idFirst, idLast, func) \
	if (uMsg == OCM_COMMAND && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define REFLECTED_COMMAND_RANGE_CODE_HANDLER_EX(idFirst, idLast, code, func) \
	if (uMsg == OCM_COMMAND && code == HIWORD(wParam) && LOWORD(wParam) >= idFirst	&& LOWORD(wParam) <= idLast) { \
		func((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam); \
		return true; \
	}

#define REFLECTED_NOTIFY_RANGE_HANDLER_EX(idFirst, idLast, func) \
	if (uMsg == OCM_NOTIFY && ((LPNMHDR) lParam)->idFrom >= idFirst && ((LPNMHDR) lParam)->idFrom <= idLast) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define REFLECTED_NOTIFY_RANGE_CODE_HANDLER_EX(idFirst, idLast, cd, func) \
	if (uMsg == OCM_NOTIFY && cd == ((LPNMHDR) lParam)->code && ((LPNMHDR) lParam)->idFrom >= idFirst && ((LPNMHDR) lParam)->idFrom <= idLast) { \
		m_result = func((LPNMHDR)lParam); \
		return true; \
	}

#define END_TTMSG_MAP() return false; }

#endif	// __TTLIB_TTMSG_MAP_H__
