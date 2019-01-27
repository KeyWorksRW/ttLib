/////////////////////////////////////////////////////////////////////////////
// Name:		ttWin
// Purpose:		Class for working with windows
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttwin.h"	// ttWin

// This is the Window procedure used by all windows that ttWin created or subclassed.

LRESULT WINAPI ttpriv::ttWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE) {
		CREATESTRUCT* pcs = (CREATESTRUCT*) lParam;
		void* pThis = (void*) pcs->lpCreateParams;
		if (pThis)
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) pThis);
	}

	ttWin* pThis = (ttWin*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!pThis)
		return DefWindowProc(hwnd, msg, wParam, lParam);

	if (pThis->OnMsgMap(msg, wParam, lParam))
		return pThis->m_result;

	if (pThis->m_SubClassProc)
		return CallWindowProc(pThis->m_SubClassProc, hwnd, msg, wParam, lParam);

	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

ttWin::ttWin()
{
	m_hwnd = NULL;
	m_hwndParent = NULL;
	m_SubClassProc = nullptr;

	// We create this here so the caller can modify it before calling CreateWnd

	m_pwc = new WNDCLASSEX;
	memset(m_pwc, 0, sizeof(WNDCLASSEXA));
	m_pwc->cbSize = sizeof(WNDCLASSEXA);
	m_pwc->lpfnWndProc = ttpriv::ttWinProc;
	m_pwc->style = CS_HREDRAW | CS_VREDRAW;
	m_pwc->hbrBackground = (HBRUSH) (LONG_PTR) (COLOR_WINDOW + 1);
}

ttWin::~ttWin()
{
	if (m_pwc)	// this will have already been deleted if CreateWnd or Attach was called
		delete m_pwc;
	if (m_pszClassName)
		::free((void*) m_pszClassName);
}

bool ttWin::SetClassName(const char* pszClassName)
{
	ttASSERT_NONEMPTY(pszClassName);

	if (!pszClassName || !*pszClassName || tt::strlen(pszClassName) > 256)	// Windows limits class names to 256 characters
		return false;

	if (m_pszClassName)
		::free((void*) m_pszClassName);
	m_pszClassName = _strdup(pszClassName);
	if (m_pwc)
		m_pwc->lpszClassName = m_pszClassName;
	return true;
}

bool ttWin::CreateWnd(const char* pszTitle, DWORD dwExStyle, DWORD dwStyle, HWND hwndParent, RECT* prcPosition, HMENU hmenu)
{
	if (m_pwc) {	// means the class hasn't been registered yet
		if (!m_pszClassName) {
			ttStr cszClass(32);
			cszClass.strcpy("ttWin");
			tt::hextoa(GetTickCount(), (char*) cszClass + cszClass.strlen(), true);
			m_pszClassName = _strdup(cszClass);
			m_pwc->lpszClassName = m_pszClassName;
		}
		m_hinst = m_pwc->hInstance = GetModuleHandle(NULL);
		if (::RegisterClassExA(m_pwc) == 0)
			return false;

		delete m_pwc;
		m_pwc = nullptr;
	}

	m_hwndParent = hwndParent;

	if (prcPosition != NULL)
		m_hwnd = ::CreateWindowExA(dwExStyle, m_pszClassName, pszTitle, dwStyle,
			prcPosition->left, prcPosition->top, tt::RC_WIDTH(prcPosition), tt::RC_HEIGHT(prcPosition),
			hwndParent, hmenu, m_hinst, (void*) this);
	else
		m_hwnd = ::CreateWindowExA(dwExStyle, m_pszClassName, pszTitle, dwStyle,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hwndParent, hmenu, m_hinst, (void*) this);

	return tt::IsValidWindow(m_hwnd);
}

bool ttWin::SubClass(HWND hwnd)
{
	ttASSERT_MSG(!m_SubClassProc, "You can only subclass a window once!");

	if (m_SubClassProc)
		return false;

	if (hwnd != NULL)
		AttachWnd(hwnd);

	SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR) this);

	m_SubClassProc = (WNDPROC) SetWindowLongPtrA(hwnd, GWLP_WNDPROC, (LONG_PTR) ttpriv::ttWinProc);

	return (m_SubClassProc != nullptr);
}

bool ttWin::AttachWnd(HWND hwnd)
{
	if (hwnd == NULL || !IsWindow(hwnd))
		return false;
	m_hwnd = hwnd;

	// Deleting m_pwc and getting the window's class name makes it possible to call CreateWnd and create another window
	// of the same type that we attached to.

	ttStr cszClassName(256);
	if (::GetClassNameA(hwnd, cszClassName, cszClassName.sizeBuffer()) != 0) {
		if (m_pwc) {
			delete m_pwc;
			m_pwc = nullptr;
		}
		SetClassName(cszClassName);	// we store the name separately from m_pwc
	}
	m_hwndParent = GetParent(hwnd);

	return true;
}
