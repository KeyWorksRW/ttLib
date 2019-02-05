/////////////////////////////////////////////////////////////////////////////
// Name:		ttmsgmap.h
// Purpose:		Macros for mapping Windows messages to functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/*
	Place the following within a class that inherits from ttDlg or ttWin:

	BEGIN_TTMSG_MAP()
		// Insert a TTMSG macro for every message you want to process
	END_TTMSG_MAP()

	If the message handler needs to return a non-zero result to Windows, set the m_result value before your function ends. All
	macros return true to indicate that you handled the message and that the default window/dialog procedure should not be called.

	TTMSG(msg, func) can be used to process any message. However, any macros placed below TTMSG() will not be called if they are
	checking for the same msg as TTMSG().

	TTMSG_COMMAND(id, func) can be used to process any WM_COMMAND message. However, any of the other macros that handle WM_COMMAND
	will not be called unless placed BEFORE you call this macro.

	For each macro you process you will need to declare a matching function. For example:

	BEGIN_TTMSG_MAP()
		TTMSG_BUTTON_CLICK(IDBTN_MYBTN, OnMyBtn)
		TTMSG(WM_INITMENU, OnInitMenu)
	END_TTMSG_MAP()

	void OnMyBtn();
	void OnInitMenu(WPARAM wParam, LPARAM lParam);
*/

#pragma once

#ifndef __TTLIB_TTMSG_MAP_H__
#define __TTLIB_TTMSG_MAP_H__

#define BEGIN_TTMSG_MAP() bool OnMsgMap(UINT uMsg, WPARAM wParam, LPARAM lParam) { uMsg; wParam; lParam; m_result = 0;
#define END_TTMSG_MAP() return false; }

#define TTMSG_BUTTON_CLICK(id, func) \
	if (uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define TTMSG_EDIT_CHANGE(id, func) \
	if (uMsg == WM_COMMAND && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == CBN_EDITCHANGE) && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define TTMSG_SEL_CHANGE(id, func) \
	if (uMsg == WM_COMMAND && (HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == CBN_SELCHANGE) && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define TTMSG_LISTBOX_DBL_CLICK(id, func) \
	if (uMsg == WM_COMMAND && HIWORD(wParam) == LBN_DBLCLK && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

// Use this for any WM_COMMAND messages not handled above
#define TTMSG_COMMAND(id, func) \
	if (uMsg == WM_COMMAND && LOWORD(wParam) == id) { \
		func(); \
		return true; \
	}

#define TTMSG_NOTIFY(id, func) \
	if (uMsg == WM_NOTIFY && ((NMHDR*) lParam)->idFrom == (UINT) id) { \
		func((NMHDR*) lParam); \
		return true; \
	}

#define TTMSG_PAINT(func) \
	if (uMsg == WM_PAINT) { \
		func(); \
		return true; \
	}

#define TTMSG_CLOSE(func) \
	if (uMsg == WM_CLOSE) { \
		func(); \
		return true; \
	}

#define TTMSG_CREATE(func) \
	if (uMsg == WM_CREATE) { \
		func((CREATESTRUCT*) lParam); \
		return true; \
	}

#define TTMSG_DESTROY(func) \
	if (uMsg == WM_DESTROY) { \
		func(); \
		return true; \
	}

// Use this for any WM_ messages not handled above
#define TTMSG(msg, func) \
	if (uMsg == msg) { \
		func(wParam, lParam); \
		return true; \
	}

/*

	If you have a large number of menu commands to handle, you can use the following to convert them into a switch statement instead of multiple if statements which may result in
	better compiler optimization.

	BEGIN_TTMSG_MAP()
		BEGIN_TTCMD_SWITCH()
			TTCMD(IDM_MYMENU, OnMyMenu)
			TTCMD(IDM_ABOUT, OnAbout)
		END_TTCMD_SWITCH()
	END_TTMSG_MAP()

	void OnMyMenu();
	void OnAbout();

*/

#define BEGIN_TTCMD_SWITCH() if (uMsg == WM_COMMAND) { switch (LOWORD(wParam)) {
#define END_TTCMD_SWITCH() default: return false; } }

#define TTCMD(id, func) case id: func(); return true;

#endif	// __TTLIB_TTMSG_MAP_H__
