/////////////////////////////////////////////////////////////////////////////
// Name:      ttCDlg
// Purpose:   Class for creating a Modal dialog box
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef _WINDOWS_
    #error This code will only work on Windows
#endif

#include <VersionHelpers.h>

#include "../include/ttdlg.h"

static HMONITOR ttKeyMonitorFromWindow(HWND hwnd, DWORD dwFlags);
static BOOL     ttKeyMonitorFromPoint(HMONITOR hMonitor, LPMONITORINFO lpmi);

ttCDlg::ttCDlg(UINT idTemplate)
{
    m_idTemplate = idTemplate;
    m_hwnd = NULL;
    m_hwndParent = NULL;
    m_bInitializing = false;
    m_bModeless = false;
    m_pShadedBtns = nullptr;
};

// Will not return until the dialog is dismissed

INT_PTR ttCDlg::DoModal(HWND hwndParent)
{
    if (hwndParent)
        m_hwndParent = hwndParent;

    HWND hwndSave = tt::hwndMsgBoxParent;
    m_bModeless = false;
    INT_PTR result = ::DialogBoxParam(tt::hinstResources, MAKEINTRESOURCE(m_idTemplate), m_hwndParent, (DLGPROC) ttpriv::DlgProc, (LPARAM) this);
    tt::hwndMsgBoxParent = hwndSave;

#ifdef _DEBUG
    // If creation failed because there was no dialog resource, report that specific condition under _DEBUG
    if (result == -1)
    {
        HRSRC hrsrc = FindResource(tt::hinstResources, MAKEINTRESOURCE(m_idTemplate), RT_DIALOG);
        ttASSERT_MSG(hrsrc, "Missing dialog template");
        if (!hrsrc)
            return result;
    }
#endif
    ttASSERT_MSG(result != -1, "Failed to create dialog box");
    return result;
}

// Parent message loop must use returned handle in IsDialogMessage for keys to work in dialog

HWND ttCDlg::DoModeless(HWND hwndParent)
{
    if (hwndParent)
        m_hwndParent = hwndParent;
    m_bModeless = true;
    return ::CreateDialogParam(tt::hinstResources, MAKEINTRESOURCE(m_idTemplate), m_hwndParent, (DLGPROC) ttpriv::DlgProc, (LPARAM) this);
}

INT_PTR WINAPI ttpriv::DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INITDIALOG) // this is the only time that pThis will be NULL
    {
        SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR) lParam);
        ttCDlg* pThis = (ttCDlg*) lParam;
        pThis->m_hwnd = hdlg;
        tt::hwndMsgBoxParent = hdlg;
        if (!ttIsValidWindow(pThis->m_hwndParent))
            pThis->m_hwndParent = GetActiveWindow();

        LRESULT lResult = 0;
        if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
            return lResult;

        pThis->m_bInitializing = true;  // needed to make sure ttDDX_ macros work correctly
        pThis->m_bCancelEnd = false;
        pThis->OnBegin();
        pThis->m_bInitializing = false;
        return TRUE;
    }

    ttCDlg* pThis = (ttCDlg*) GetWindowLongPtr(hdlg, DWLP_USER);
    if (!pThis)
        return FALSE;

    if (msg == WM_DESTROY) {
        if (pThis->m_pShadedBtns)
        {
            delete pThis->m_pShadedBtns;
            pThis->m_pShadedBtns = nullptr;
        }
#ifdef _DEBUG
        // This allows a destructor to verify that the window was destroyed before the destructor was called

        if (pThis->m_bModeless)
            pThis->m_hwnd = nullptr;
#endif
    }

    LRESULT lResult = 0;    // This is passed by reference to OnCmdCaseMap
    if (msg == WM_COMMAND && pThis->OnCmdCaseMap((int) LOWORD(wParam), (int) HIWORD(wParam), lResult))
        return lResult;

    if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
        return lResult;

    switch (msg)
    {
        case WM_COMMAND:
            {
                switch (LOWORD(wParam))
                {
                    case IDOK:
                        pThis->m_bInitializing = false;
                        pThis->OnOK();
                        if (pThis->m_bCancelEnd)
                            pThis->m_bCancelEnd = false;
                        else
                            pThis->CloseDialog(IDOK);   // do NOT call EndDialog--it will fail if this is a modeless dialog
                        break;

                    case IDCANCEL:
                        pThis->OnCancel();
                        if (pThis->m_bCancelEnd)
                            pThis->m_bCancelEnd = false;
                        else
                            pThis->CloseDialog(IDCANCEL);
                        break;

                    default:
                        break;
                }
            }
            break;
    }

    return 0;
}

ptrdiff_t ttCDlg::GetControlInteger(int id) const
{
    char szBuf[20];
    GetControlText(id, szBuf, sizeof(szBuf) - 1);
    return ttAtoi(szBuf);
}

void ttCDlg::SetControlInteger(int id, ptrdiff_t val) const
{
    char szBuf[20];
    ttItoa(val, szBuf, sizeof(szBuf));
    SetControlText(id, szBuf);
}

void ttCDlg::EnableShadeBtns()
{
    if (!m_pShadedBtns)
    {
        m_pShadedBtns = new ttCMultiBtn;
        if (m_pShadedBtns)
            m_pShadedBtns->Initialize(*this);
    }
}

void ttCDlg::SetBtnIcon(int idBtn, int idIcon, UINT nIconAlign)
{
    if (!m_pShadedBtns)
    {
        EnableShadeBtns();
        if (!m_pShadedBtns)
            return;
    }
    m_pShadedBtns->SetIcon(idBtn, idIcon, nIconAlign);
}

void ttCDlg::CenterWindow(bool bCenterOnDesktop)
{
    RECT rc;
    GetWindowRect(*this, &rc);

    int cx = ttRC_WIDTH(rc);
    int cy = ttRC_HEIGHT(rc);

    if (!bCenterOnDesktop && m_hwndParent)
        GetWindowRect(m_hwndParent, &rc);
    else
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

    int left = rc.left + (ttRC_WIDTH(rc) - cx) / 2;
    int top  = rc.top + (ttRC_HEIGHT(rc) - cy) / 2;

    // Make certain the dialog doesn't spawn between two monitors

    RECT rcDesktop;
    HMONITOR hmon = ttKeyMonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY);
    if (hmon)
    {
        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        if (ttKeyMonitorFromPoint(hmon, &mi))
        {
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
    ::MoveWindow(*this, left, top, cx, cy, FALSE);
}

LRESULT ttCListView::AddString(const char* psz, LPARAM lParam)
{
    ttASSERT(psz);
    if (!psz)
        return -1;
    LVITEMA lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (char*) psz;
    lvi.iItem = 0x7fffffff;     // ensure the item is appended
    if (lParam != -1)
    {
        lvi.mask |= LVIF_PARAM;
        lvi.lParam = lParam;
    }
    return (LRESULT) ::SendMessage(m_hwnd, LVM_INSERTITEMA, 0, (LPARAM) &lvi);
}

LRESULT ttCListView::AddString(const wchar_t* pwsz, LPARAM lParam)
{
    ttASSERT(pwsz);
    if (!pwsz)
        return -1;
    LVITEMW lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (wchar_t*) pwsz;
    lvi.iItem = 0x7fffffff;
    if (lParam != -1)
    {
        lvi.mask |= LVIF_PARAM;
        lvi.lParam = lParam;
    }
    return (LRESULT) ::SendMessage(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) &lvi);
}

BOOL ttCListView::AddSubString(int iItem, int iSubItem, const char* psz)
{
    ttASSERT(psz);
    if (!psz)
        return -1;
    LVITEMA lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (char*) psz;
    lvi.iItem = iItem;
    lvi.iSubItem = iSubItem;
    return (BOOL) ::SendMessage(m_hwnd, LVM_SETITEMA, 0, (LPARAM) &lvi);
}

BOOL ttCListView::AddSubString(int iItem, int iSubItem, const wchar_t* pwsz)
{
    ttASSERT(pwsz);
    if (!pwsz)
        return -1;
    LVITEMW lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (wchar_t*) pwsz;
    lvi.iItem = iItem;
    lvi.iSubItem = iSubItem;
    return (BOOL) ::SendMessage(m_hwnd, LVM_SETITEMW, 0, (LPARAM) &lvi);
}

void ttCListView::InsertColumn(int iColumn, const char* pszText, int width)
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

void ttCListView::InsertColumn(int iColumn, const wchar_t* pszText, int width)
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
static HMONITOR (WINAPI* s_pfnMonitorFromPoint)(POINT, DWORD);
static BOOL     (WINAPI* s_pfnGetMonitorInfo)(HMONITOR, LPMONITORINFO);

#ifndef xPRIMARY_MONITOR
#define xPRIMARY_MONITOR ((HMONITOR)0x12340042)
#endif

#ifndef MONITORINFOF_PRIMARY
#define MONITORINFOF_PRIMARY        0x00000001
#endif

static bool InitMonitorStubs()
{
    static bool fTried = false;
    if (fTried)
        return (s_pfnGetMonitorInfo != NULL);

    fTried = true;
    HMODULE hUser32 = GetModuleHandle(TEXT("USER32"));

    if (hUser32 &&
            (*(FARPROC*)&s_pfnMonitorFromWindow   = GetProcAddress(hUser32,"MonitorFromWindow")) != NULL &&
            (*(FARPROC*)&s_pfnMonitorFromPoint    = GetProcAddress(hUser32,"MonitorFromPoint")) != NULL &&
            (*(FARPROC*)&s_pfnGetMonitorInfo      = GetProcAddress(hUser32,"GetMonitorInfoA")) != NULL)
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
        lpmi->rcMonitor.top  = 0;
        lpmi->rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
        lpmi->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        lpmi->dwFlags = MONITORINFOF_PRIMARY;
        return TRUE;
    }
}
