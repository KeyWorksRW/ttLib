/////////////////////////////////////////////////////////////////////////////
// Name:		ttWin
// Purpose:		Class for working with windows
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Do NOT use this class to create a MDI windows!

#pragma once

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include "ttmsgmap.h"	// #define macros for BEGIN_TTMSG_MAP()/END_TTMSG_MAP() block

namespace ttpriv {
	LRESULT WINAPI ttWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

class ttWin		// Non-MDI window class
{
public:
	ttWin();
	~ttWin();

public:
	// Call these methods before calling CreateWnd

	void SetClassBkgrnd(HBRUSH hbkgrnd) { if (m_pwc) m_pwc->hbrBackground = hbkgrnd; }	// constructor will have set this to COLOR_WINDOW + 1
	void SetClassCursor(HCURSOR hcur) { if (m_pwc) m_pwc->hCursor = hcur; }
	void SetClassIcon(size_t idIcon) { if (m_pwc) m_pwc->hIcon = LoadIcon(m_pwc->hInstance, (LPCTSTR) idIcon); }
	void SetClassMenu(size_t idMenuResource) { if (m_pwc) m_pwc->lpszMenuName = (LPCTSTR) idMenuResource; }
	bool SetClassName(const char* pszClassName);	// returns false if strlen(pszClassName) > 255
	void SetClassSmallIcon(size_t idIcon) { if (m_pwc) m_pwc->hIconSm = LoadIcon(m_pwc->hInstance, (LPCTSTR) idIcon); }
	void SetClassStyle(DWORD style) { if (m_pwc) m_pwc->style = style; }	// constructor will have set this to CS_HREDRAW | CS_VREDRAW

	bool CreateWnd(const char* pszTitle, DWORD dwExStyle, DWORD dwStyle, HWND hwndParent = NULL, RECT* prc = NULL, HMENU hmenu = NULL);

	bool AttachWnd(HWND hwnd);			// attaches to a window not created by ttWin, updates m_pszClassName, m_hwnd and m_hwndParent
	bool SubClass(HWND hwnd = NULL);	// if NULL, subclass our own window

	// Class functions

	LRESULT SendMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::SendMessage(*this, msg, wParam, lParam); }
	LRESULT PostMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const { return ::PostMessage(*this, msg, wParam, lParam); }

	void ShowWindow(int nCmdShow = SW_SHOW) { ::ShowWindow(*this, nCmdShow); }

	// For other Windows functions requiring an HWND parameter, simply pass in *this as the HWND

	////////// Operators //////////

	operator HWND() const { return m_hwnd; }
	operator WNDCLASSEX*() { return m_pwc; }

protected:
	virtual bool OnMsgMap(UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */) { return false; }	  // Use of BEGIN_TTMSG_MAP will override this

protected:
	friend LRESULT WINAPI ttpriv::ttWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Class members
	const char*	m_pszClassName;		// class name of the window we created or attached to

	HWND		m_hwnd;				// m_hwnd vs m_hWnd -- SDK/include, ATL and WTL use both variants. We're sticking with all lowercase.
	HWND		m_hwndParent;
	HINSTANCE	m_hinst;			// instance used to create the class, can be used to load resources from the app

	WNDCLASSEXA* m_pwc; 			// created in constructor, deleted by CreateWnd and AttachWnd
	WNDPROC		 m_SubClassProc;	// previous window procedure before it was subclassed
	LRESULT		 m_result;
};
