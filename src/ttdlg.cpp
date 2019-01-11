/////////////////////////////////////////////////////////////////////////////
// Name:		ttDlg
// Purpose:		Class for creating a Modal dialog box
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif
#include <VersionHelpers.h>

#include "../include/ttdlg.h"

#define WMP_CENTER_WINDOW WM_USER + 0x7000

namespace ttpriv {
	BOOL (WINAPI* pfnAnimateWindow)(HWND, DWORD, DWORD);
	HMONITOR MonitorFromWindow(HWND hwnd, DWORD dwFlags);
	HMONITOR MonitorFromPoint(POINT pt, DWORD dwFlags);
	BOOL	 GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi);

	HMONITOR (WINAPI* pfnMonitorFromWindow)(HWND hwnd, DWORD dwFlags);
	HMONITOR (WINAPI* pfnMonitorFromPoint)(POINT pt, DWORD dwFlags);
	BOOL	 (WINAPI* pfnGetMonitorInfo)(HMONITOR hMonitor, LPMONITORINFO lpmi);
}

ttDlg::ttDlg(UINT idTemplate, HWND hwnd)
{
	m_hWnd = NULL;
	m_hwndParent = hwnd;
	m_fCenterWindow = true;
	m_fCancelEnd = false;
	m_fFade = false;
	m_idTemplate = idTemplate;
	m_bShadeBtns = true;
	m_bInitializing = true;
};

INT_PTR ttDlg::DoModal()
{
	INT_PTR result = ::DialogBoxParam(tt::hinstResources, MAKEINTRESOURCE(m_idTemplate), m_hwndParent, (DLGPROC) ttpriv::DlgProc, (LPARAM) this);
#ifdef _DEBUG
	if (result == -1) {
		HRSRC hrsrc = FindResource(tt::hinstResources, MAKEINTRESOURCE(m_idTemplate), RT_DIALOG);
		ttASSERT_MSG(hrsrc, "Missing dialog template");
		if (!hrsrc)
			return result;
	}
#endif
	ttASSERT_MSG(result != -1, "Failed to create dialog box");
	return result;
}

INT_PTR WINAPI ttpriv::DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INITDIALOG) {	// this is the only time that pThis will be NULL
#ifdef _WIN64
		SetWindowLongPtr(hdlg, GWLP_USERDATA, (LONG_PTR) lParam);
#else
		SetWindowLongPtr(hdlg, GWL_USERDATA, lParam);
#endif
		ttDlg* pThis = (ttDlg*) lParam;
		pThis->m_hWnd = hdlg;
		if (!tt::IsValidWindow(pThis->m_hwndParent))
			pThis->m_hwndParent = GetActiveWindow();

		if (pThis->m_fCenterWindow) {
			PostMessage(hdlg, WMP_CENTER_WINDOW, 0, 0);
			ShowWindow(hdlg, SW_HIDE);	// Don't show window until we are centered
		}

		if (pThis->OnMsgMap(msg, wParam, lParam))
			return pThis->lResult;

		pThis->OnBegin();
		pThis->m_bInitializing = false;
		return TRUE;
	}

#ifdef _WIN64
	ttDlg* pThis = (ttDlg*) GetWindowLongPtr(hdlg, GWLP_USERDATA);
#else
	ttDlg* pThis = (ttDlg*) GetWindowLongPtr(hdlg, GWL_USERDATA);
#endif
	if (!pThis)
		return FALSE;

	if (pThis->OnMsgMap(msg, wParam, lParam))
		return pThis->lResult;

	switch (msg) {
		case WMP_CENTER_WINDOW:
			{
				RECT rc;
				GetWindowRect(hdlg, &rc);

				int cx = tt::RC_WIDTH(rc);
				int cy = tt::RC_HEIGHT(rc);

				if (pThis->m_hwndParent) {
					GetWindowRect(pThis->m_hwndParent, &rc);
				}
				else {
					SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
				}

				int left = rc.left + (tt::RC_WIDTH(rc) - cx) / 2;
				int top	 = rc.top + (tt::RC_HEIGHT(rc) - cy) / 2;

				// Make certain the dialog doesn't spawn between two monitors

				RECT rcDesktop;
				HMONITOR hmon = ttpriv::pfnMonitorFromWindow(hdlg, MONITOR_DEFAULTTOPRIMARY);
				if (hmon) {
					MONITORINFO mi;
					mi.cbSize = sizeof(mi);
					if (ttpriv::pfnGetMonitorInfo(hmon, &mi)) {
						CopyRect(&rcDesktop, &mi.rcWork);
					}
					else {
						SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
					}
				}
				else {
					SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
				}

				if (left < rcDesktop.left)
					left = rcDesktop.left;
				if (left + cx > rcDesktop.right)
					left -= (left + cx - rcDesktop.right);

				if (top < rcDesktop.top)
					top = rcDesktop.top;
				if (top + cy > rcDesktop.bottom)
					top -= (top + cy - rcDesktop.bottom);

				::MoveWindow(hdlg, left, top, cx, cy, TRUE);
				ShowWindow(hdlg, SW_SHOW);	// We're centered, so now show the window
			}
			break;

		case WM_COMMAND:
			{
				switch (LOWORD(wParam)) {
					case IDOK:
						pThis->m_bInitializing = false;
						pThis->OnEnd();
						if (pThis->m_fCancelEnd)
							pThis->m_fCancelEnd = false;
						else {
							if (pThis->m_fFade) {
								pThis->FadeWindow();
							}
							EndDialog(hdlg, IDOK);
						}
						break;

					case IDCANCEL:
						pThis->OnCancel();
						if (pThis->m_fCancelEnd)
							pThis->m_fCancelEnd = false;
						else {
							// Note that if the dialog is cancelled, we don't fade, we just exit immediately
							EndDialog(hdlg, IDCANCEL);
						}
						break;

					default:
						break;
				}
			}
			break;
	}

	return FALSE;
}

ptrdiff_t ttDlg::GetControlInteger(ptrdiff_t id) const
{
	char szBuf[20];
	GetControlText(id, szBuf, sizeof(szBuf) - 1);
	return tt::atoi(szBuf);
}

void ttDlg::SetControlInteger(ptrdiff_t id, ptrdiff_t val) const
{
	char szBuf[20];
	tt::itoa(val, szBuf, sizeof(szBuf));
	SetControlText(id, szBuf);
}

void ttDlg::FadeWindow()
{

#if (WINVER < 0x0500)

// AnimateWindow() Commands
#define AW_HOR_POSITIVE				0x00000001
#define AW_HOR_NEGATIVE				0x00000002
#define AW_VER_POSITIVE				0x00000004
#define AW_VER_NEGATIVE				0x00000008
#define AW_CENTER					0x00000010
#define AW_HIDE						0x00010000
#define AW_ACTIVATE					0x00020000
#define AW_SLIDE					0x00040000
#define AW_BLEND					0x00080000

#endif

	if (IsWindowsXPOrGreater()) {
		if (!ttpriv::pfnAnimateWindow) {
			HMODULE hUser32 = GetModuleHandle(TEXT("USER32"));
			if (hUser32 &&
					(*(FARPROC*)&ttpriv::pfnAnimateWindow = GetProcAddress(hUser32, "AnimateWindow")) != NULL)
				ttpriv::pfnAnimateWindow(m_hWnd, 200, AW_HIDE | AW_BLEND);
		}
		else
			ttpriv::pfnAnimateWindow(m_hWnd, 200, AW_HIDE | AW_BLEND);

		// Theoretically, we may not have called AnimateWindow -- but only if an
		// Operating System later then version 5 doesn't support it (or for some
		// fluke, we couldn't get the proc address).

		Sleep(200);
	}
}

LRESULT ttListView::AddString(const char* psz, LPARAM lParam)
{
	ttASSERT(psz);
	if (!psz)
		return -1;
	LVITEMA lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (char*) psz;
	if (lParam != -1) {
		lvi.mask |= LVIF_PARAM;
		lvi.lParam = lParam;
	}
	return (LRESULT) ::SendMessage(m_hwnd, LVM_INSERTITEMA, 0, (LPARAM) &lvi);
}

LRESULT ttListView::AddString(const wchar_t* pwsz, LPARAM lParam)
{
	ttASSERT(pwsz);
	if (!pwsz)
		return -1;
	LVITEMW lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*) pwsz;
	if (lParam != -1) {
		lvi.mask |= LVIF_PARAM;
		lvi.lParam = lParam;
	}
	return (LRESULT) ::SendMessage(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) &lvi);
}

BOOL ttListView::AddSubString(int iItem, int iSubItem, const char* psz)
{
	ttASSERT(psz);
	if (!psz)
		return -1;
	LV_ITEMA lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (char*) psz;
	lvi.iItem = iItem;
	lvi.iSubItem = iSubItem;
	return (BOOL) ::SendMessage(m_hwnd, LVM_SETITEMA, 0, (LPARAM) &lvi);
}

BOOL ttListView::AddSubString(int iItem, int iSubItem, const wchar_t* pwsz)
{
	ttASSERT(pwsz);
	if (!pwsz)
		return -1;
	LV_ITEMW lvi;
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (wchar_t*) pwsz;
	lvi.iItem = iItem;
	lvi.iSubItem = iSubItem;
	return (BOOL) ::SendMessage(m_hwnd, LVM_SETITEMW, 0, (LPARAM) &lvi);
}

void ttListView::InsertColumn(int iColumn, const char* pszText, int width)
{
	ttASSERT(pszText);
	if (!pszText)
		return;
	LV_COLUMNA lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | (width == -1 ? 0 : LVCF_WIDTH);
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = width;
	lvc.pszText = (char*) pszText;

	::SendMessage(m_hwnd, LVM_INSERTCOLUMNA, (WPARAM) iColumn, (LPARAM) &lvc);
}

void ttListView::InsertColumn(int iColumn, const wchar_t* pszText, int width)
{
	ttASSERT(pszText);
	if (!pszText)
		return;
	LV_COLUMNW lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | (width == -1 ? 0 : LVCF_WIDTH);
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = width;
	lvc.pszText = (wchar_t*) pszText;

	::SendMessage(m_hwnd, LVM_INSERTCOLUMNW, (WPARAM) iColumn, (LPARAM) &lvc);
}

#ifdef _DEBUG

DWORD ttDlg::CheckItemID(int id, const char* pszID, int line, const char* file) const {
	if (::GetDlgItem(*this, id) == NULL) {
		ttString cszMsg;
		cszMsg.printf("Invalid dialog control id: %s (%u)", pszID, id);
		tt::AssertionMsg(cszMsg, file, __func__, line);
	}
	return id;
}

#endif	// _DEBUG

///////////////////// Monitor Code /////////////////////////////////////////

#ifndef	xPRIMARY_MONITOR
#define xPRIMARY_MONITOR ((HMONITOR)0x12340042)
#endif

#ifndef	MONITORINFOF_PRIMARY
#define MONITORINFOF_PRIMARY		0x00000001
#endif

namespace ttpriv {
	bool InitMonitorStubs();
}

bool ttpriv::InitMonitorStubs()
{
	static bool fTried = false;
	if (fTried)
		return (ttpriv::pfnGetMonitorInfo != NULL);

	fTried = true;
	HMODULE hUser32 = GetModuleHandle(TEXT("USER32"));

	if (hUser32 &&
			(*(FARPROC*)&ttpriv::pfnMonitorFromWindow	  = GetProcAddress(hUser32,"MonitorFromWindow")) != NULL &&
			(*(FARPROC*)&ttpriv::pfnMonitorFromPoint	  = GetProcAddress(hUser32,"MonitorFromPoint")) != NULL &&
			(*(FARPROC*)&ttpriv::pfnGetMonitorInfo	  = GetProcAddress(hUser32,"GetMonitorInfoA")) != NULL)
		return true;

	return false;
}

HMONITOR ttpriv::MonitorFromWindow(HWND hwnd, DWORD dwFlags)
{
	if (ttpriv::InitMonitorStubs())
		return ttpriv::pfnMonitorFromWindow(hwnd, dwFlags);
	else
		return xPRIMARY_MONITOR;
}

HMONITOR ttpriv::MonitorFromPoint(POINT pt, DWORD dwFlags)
{
	if (ttpriv::InitMonitorStubs())
		return ttpriv::pfnMonitorFromPoint(pt, dwFlags);
	else
		return xPRIMARY_MONITOR;
}

BOOL ttpriv::GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
	if (ttpriv::InitMonitorStubs())
		return ttpriv::pfnGetMonitorInfo(hMonitor, lpmi);
	else {
		SystemParametersInfo(SPI_GETWORKAREA, 0, &lpmi->rcWork, 0);
		lpmi->rcMonitor.left = 0;
		lpmi->rcMonitor.top	 = 0;
		lpmi->rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
		lpmi->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
		lpmi->dwFlags = MONITORINFOF_PRIMARY;
		return TRUE;
	}
}