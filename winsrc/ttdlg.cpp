/////////////////////////////////////////////////////////////////////////////
// Name:      ttCDlg
// Purpose:   Class for creating a Modal dialog box
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <VersionHelpers.h>

#include "../include/ttlibwin.h"

#include "../include/ttdlg.h"

static HMONITOR ttKeyMonitorFromWindow(HWND hwnd, DWORD dwFlags);
static BOOL ttKeyMonitorFromPoint(HMONITOR hMonitor, LPMONITORINFO lpmi);

ttCDlg::ttCDlg(UINT idTemplate)
{
    m_idTemplate = idTemplate;
    m_hwnd = NULL;
    m_hwndParent = NULL;
    m_isInitializing = false;
    m_isModeless = false;
    m_pShadedBtns = nullptr;
};

// Will not return until the dialog is dismissed

INT_PTR ttCDlg::DoModal(HWND hwndParent)
{
    if (hwndParent)
        m_hwndParent = hwndParent;

    m_isModeless = false;
    INT_PTR result = ::DialogBoxParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(m_idTemplate), m_hwndParent,
                                       (DLGPROC) ttpriv::DlgProc, (LPARAM) this);

#if !defined(NDEBUG)  // Starts debug section.
    // If creation failed because there was no dialog resource, report that specific condition in Debug builds.
    if (result == -1)
    {
        HRSRC hrsrc = FindResourceA(GetModuleHandle(NULL), MAKEINTRESOURCEA(m_idTemplate), (char*) RT_DIALOG);
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
    m_isModeless = true;
    return ::CreateDialogParamA(GetModuleHandle(NULL), MAKEINTRESOURCEA(m_idTemplate), m_hwndParent,
                                (DLGPROC) ttpriv::DlgProc, (LPARAM) this);
}

INT_PTR WINAPI ttpriv::DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INITDIALOG)  // this is the only time that pThis will be NULL
    {
        SetWindowLongPtrA(hdlg, DWLP_USER, (LONG_PTR) lParam);
        ttCDlg* pThis = (ttCDlg*) lParam;
        pThis->m_hwnd = hdlg;
        if (!(pThis->m_hwndParent && IsWindow(pThis->m_hwndParent)))
            pThis->m_hwndParent = GetActiveWindow();

        LRESULT lResult = 0;
        if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
            return lResult;

        pThis->m_isInitializing = true;  // needed to make sure ttDDX_ macros work correctly
        pThis->m_isCancelEnd = false;
        pThis->OnBegin();
        pThis->m_isInitializing = false;
        return TRUE;
    }

    ttCDlg* pThis = (ttCDlg*) GetWindowLongPtrA(hdlg, DWLP_USER);
    if (!pThis)
        return FALSE;

    if (msg == WM_DESTROY)
    {
        if (pThis->m_pShadedBtns)
        {
            delete pThis->m_pShadedBtns;
            pThis->m_pShadedBtns = nullptr;
        }
#if !defined(NDEBUG)  // Starts debug section.
        // This allows a destructor to verify that the window was destroyed before the destructor was called

        if (pThis->m_isModeless)
            pThis->m_hwnd = nullptr;
#endif
    }

    LRESULT lResult = 0;  // This is passed by reference to OnCmdCaseMap
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
                    pThis->m_isInitializing = false;
                    pThis->OnOK();
                    if (pThis->m_isCancelEnd)
                        pThis->m_isCancelEnd = false;
                    else
                        pThis->CloseDialog(
                            IDOK);  // do NOT call EndDialog--it will fail if this is a modeless dialog
                    break;

                case IDCANCEL:
                    pThis->OnCancel();
                    if (pThis->m_isCancelEnd)
                        pThis->m_isCancelEnd = false;
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

void ttCDlg::SetBtnIcon(int idBtn, const char* pszIconName, UINT nIconAlign)
{
    if (!m_pShadedBtns)
    {
        EnableShadeBtns();
        if (!m_pShadedBtns)
            return;
    }
    m_pShadedBtns->SetIcon(idBtn, pszIconName, nIconAlign);
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
    int top = rc.top + (ttRC_HEIGHT(rc) - cy) / 2;

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
        else
        {
            SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
        }
    }
    else
    {
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

LRESULT ttCListView::add(std::string_view str, LPARAM lparam)
{
    std::wstring str16;
    utf8::unchecked::utf8to16(str.begin(), str.end(), back_inserter(str16));

    LVITEMW lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (LPWSTR) str16.c_str();
    lvi.iItem = 0x7fffffff;  // ensure the item is appended
    if (lparam != -1)
    {
        lvi.mask |= LVIF_PARAM;
        lvi.lParam = lparam;
    }
    return (LRESULT)::SendMessageW(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) &lvi);
}

BOOL ttCListView::addsubstring(std::string_view str, int iItem, int iSubItem)
{
    std::wstring str16;
    utf8::unchecked::utf8to16(str.begin(), str.end(), back_inserter(str16));

    LVITEMW lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (LPWSTR) str16.c_str();
    lvi.iItem = iItem;
    lvi.iSubItem = iSubItem;
    return (BOOL)::SendMessageW(m_hwnd, LVM_SETITEMW, 0, (LPARAM) &lvi);
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
    lvi.iItem = 0x7fffffff;  // ensure the item is appended
    if (lParam != -1)
    {
        lvi.mask |= LVIF_PARAM;
        lvi.lParam = lParam;
    }
    return (LRESULT)::SendMessageA(m_hwnd, LVM_INSERTITEMA, 0, (LPARAM) &lvi);
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
    return (LRESULT)::SendMessageW(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) &lvi);
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
    return (BOOL)::SendMessageA(m_hwnd, LVM_SETITEMA, 0, (LPARAM) &lvi);
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
    return (BOOL)::SendMessageW(m_hwnd, LVM_SETITEMW, 0, (LPARAM) &lvi);
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

    ::SendMessageA(m_hwnd, LVM_INSERTCOLUMNA, (WPARAM) iColumn, (LPARAM) &lvc);
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

    ::SendMessageW(m_hwnd, LVM_INSERTCOLUMNW, (WPARAM) iColumn, (LPARAM) &lvc);
}

LRESULT ttCListView::SetCurSel(int pos)
{
    LVITEMA lvi;
    lvi.stateMask = 0x0F;
    lvi.state = LVIS_FOCUSED | LVIS_SELECTED;
    return ::SendMessageA(m_hwnd, LVM_SETITEMSTATE, pos, (LPARAM) &lvi);
}

LRESULT ttCListView::SetCurSel(const char* pszItem)
{
    LV_FINDINFOA lvfi;
    lvfi.flags = LVFI_STRING;
    lvfi.psz = pszItem;
    auto pos = ::SendMessageA(m_hwnd, LVM_FINDITEM, (WPARAM) -1, (LPARAM) &lvfi);
    return (pos != -1) ? SetCurSel((int) pos) : -1;
}

///////////////////// Monitor Code /////////////////////////////////////////

static HMONITOR(WINAPI* s_pfnMonitorFromWindow)(HWND, DWORD);
static HMONITOR(WINAPI* s_pfnMonitorFromPoint)(POINT, DWORD);
static BOOL(WINAPI* s_pfnGetMonitorInfo)(HMONITOR, LPMONITORINFO);

#ifndef xPRIMARY_MONITOR
    #define xPRIMARY_MONITOR ((HMONITOR) 0x12340042)
#endif

#ifndef MONITORINFOF_PRIMARY
    #define MONITORINFOF_PRIMARY 0x00000001
#endif

static bool InitMonitorStubs()
{
    static bool fTried = false;
    if (fTried)
        return (s_pfnGetMonitorInfo != NULL);

    fTried = true;
    HMODULE hUser32 = GetModuleHandle(TEXT("USER32"));

    if (hUser32 && (*(FARPROC*) & s_pfnMonitorFromWindow = GetProcAddress(hUser32, "MonitorFromWindow")) != NULL &&
        (*(FARPROC*) & s_pfnMonitorFromPoint = GetProcAddress(hUser32, "MonitorFromPoint")) != NULL &&
        (*(FARPROC*) & s_pfnGetMonitorInfo = GetProcAddress(hUser32, "GetMonitorInfoA")) != NULL)
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
    else
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &lpmi->rcWork, 0);
        lpmi->rcMonitor.left = 0;
        lpmi->rcMonitor.top = 0;
        lpmi->rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        lpmi->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        lpmi->dwFlags = MONITORINFOF_PRIMARY;
        return TRUE;
    }
}
