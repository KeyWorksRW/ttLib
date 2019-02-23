/////////////////////////////////////////////////////////////////////////////
// Name:		ttCDlg
// Purpose:		Class for creating a Modal dialog box
// Author:		Ralph Walden
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

static HMONITOR ttKeyMonitorFromWindow(HWND hwnd, DWORD dwFlags);
static BOOL 	ttKeyMonitorFromPoint(HMONITOR hMonitor, LPMONITORINFO lpmi);

// Currently unused
// static HMONITOR ttKeyMonitorFromPoint(POINT pt, DWORD dwFlags);

ttCDlg::ttCDlg(UINT idTemplate, HWND hwnd)
{
	m_hwnd = NULL;
	m_hwndParent = hwnd;
	m_bCenterWindow = true;
	m_bCancelEnd = false;
	m_fFade = false;
	m_idTemplate = idTemplate;
	m_bShadeBtns = false;	// default to non-shaded buttons
	m_bInitializing = true;
};

INT_PTR ttCDlg::DoModal()
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
		ttCDlg* pThis = (ttCDlg*) lParam;
		pThis->m_hwnd = hdlg;
		if (!tt::IsValidWindow(pThis->m_hwndParent))
			pThis->m_hwndParent = GetActiveWindow();

		if (pThis->m_bCenterWindow) {
			PostMessage(hdlg, WMP_CENTER_WINDOW, 0, 0);
			ShowWindow(hdlg, SW_HIDE);	// Don't show window until we are centered
		}

		if (pThis->m_bShadeBtns)
			pThis->m_ShadedBtns.Initialize(hdlg);

		LRESULT lResult = 0;
		if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
			return lResult;

		pThis->m_bInitializing = true;	// needed to make sure ttDDX_ macros work correctly
		pThis->OnBegin();
		pThis->m_bInitializing = false;
		return TRUE;
	}

#ifdef _WIN64
	ttCDlg* pThis = (ttCDlg*) GetWindowLongPtr(hdlg, GWLP_USERDATA);
#else
	ttCDlg* pThis = (ttCDlg*) GetWindowLongPtr(hdlg, GWL_USERDATA);
#endif
	if (!pThis)
		return FALSE;

	LRESULT lResult = 0;
	if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
		return lResult;

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
				HMONITOR hmon = ttKeyMonitorFromWindow(hdlg, MONITOR_DEFAULTTOPRIMARY);
				if (hmon) {
					MONITORINFO mi;
					mi.cbSize = sizeof(mi);
					if (ttKeyMonitorFromPoint(hmon, &mi)) {
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
						pThis->OnOK();
						if (pThis->m_bCancelEnd)
							pThis->m_bCancelEnd = false;
						else {
							if (pThis->m_fFade) {
								pThis->FadeWindow();
							}
							EndDialog(hdlg, IDOK);
						}
						break;

					case IDCANCEL:
						pThis->OnCancel();
						if (pThis->m_bCancelEnd)
							pThis->m_bCancelEnd = false;
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

ptrdiff_t ttCDlg::GetControlInteger(ptrdiff_t id) const
{
	char szBuf[20];
	GetControlText(id, szBuf, sizeof(szBuf) - 1);
	return tt::atoi(szBuf);
}

void ttCDlg::SetControlInteger(ptrdiff_t id, ptrdiff_t val) const
{
	char szBuf[20];
	tt::itoa(val, szBuf, sizeof(szBuf));
	SetControlText(id, szBuf);
}

static BOOL (WINAPI* tt_pfnAnimateWindow)(HWND, DWORD, DWORD);

void ttCDlg::FadeWindow()
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
		if (!tt_pfnAnimateWindow) {
			HMODULE hUser32 = GetModuleHandle(TEXT("USER32"));
			if (hUser32 &&
					(*(FARPROC*)&tt_pfnAnimateWindow = GetProcAddress(hUser32, "AnimateWindow")) != NULL)
				tt_pfnAnimateWindow(m_hwnd, 200, AW_HIDE | AW_BLEND);
		}
		else
			tt_pfnAnimateWindow(m_hwnd, 200, AW_HIDE | AW_BLEND);

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

///////////////////// Monitor Code /////////////////////////////////////////

static HMONITOR (WINAPI* s_pfnMonitorFromWindow)(HWND, DWORD);
static HMONITOR	(WINAPI* s_pfnMonitorFromPoint)(POINT, DWORD);
static BOOL		(WINAPI* s_pfnGetMonitorInfo)(HMONITOR, LPMONITORINFO);

#ifndef	xPRIMARY_MONITOR
#define xPRIMARY_MONITOR ((HMONITOR)0x12340042)
#endif

#ifndef	MONITORINFOF_PRIMARY
#define MONITORINFOF_PRIMARY		0x00000001
#endif

static bool InitMonitorStubs()
{
	static bool fTried = false;
	if (fTried)
		return (s_pfnGetMonitorInfo != NULL);

	fTried = true;
	HMODULE hUser32 = GetModuleHandle(TEXT("USER32"));

	if (hUser32 &&
			(*(FARPROC*)&s_pfnMonitorFromWindow	  = GetProcAddress(hUser32,"MonitorFromWindow")) != NULL &&
			(*(FARPROC*)&s_pfnMonitorFromPoint	  = GetProcAddress(hUser32,"MonitorFromPoint")) != NULL &&
			(*(FARPROC*)&s_pfnGetMonitorInfo	  = GetProcAddress(hUser32,"GetMonitorInfoA")) != NULL)
		return true;

	return false;
}

static HMONITOR ttKeyMonitorFromWindow(HWND hwnd, DWORD dwFlags)
{
	if (InitMonitorStubs())
		return s_pfnMonitorFromWindow(hwnd, dwFlags);
	else
		return xPRIMARY_MONITOR;
}

#if 0
static HMONITOR ttKeyMonitorFromPoint(POINT pt, DWORD dwFlags)
{
	if (InitMonitorStubs())
		return s_pfnMonitorFromPoint(pt, dwFlags);
	else
		return xPRIMARY_MONITOR;
}
#endif

static BOOL ttKeyMonitorFromPoint(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
	if (InitMonitorStubs())
		return s_pfnGetMonitorInfo(hMonitor, lpmi);
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
