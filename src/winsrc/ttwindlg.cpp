/////////////////////////////////////////////////////////////////////////////
// Name:      ttwindlg.cpp
// Purpose:   Class for creating a Modal or Modeless dialog box
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <sstream>

#include <VersionHelpers.h>

#include "ttwindlg.h"

static HMONITOR ttKeyMonitorFromWindow(HWND hwnd, DWORD dwFlags);
static BOOL ttKeyMonitorFromPoint(HMONITOR hMonitor, LPMONITORINFO lpmi);

using namespace ttlib;

dlg::dlg(UINT idTemplate)
{
    m_idTemplate = idTemplate;
};

// Will not return until the dialog is dismissed

INT_PTR dlg::DoModal(HWND hwndParent)
{
    if (hwndParent)
        m_hwndParent = hwndParent;

    m_isModeless = false;
    INT_PTR result =
        ::DialogBoxParamW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(m_idTemplate), m_hwndParent, (DLGPROC) ttlib::DlgProc, (LPARAM) this);

#if !defined(NDEBUG)  // Starts debug section.
    // If creation failed because there was no dialog resource, report that specific condition in Debug builds.
    if (result == -1)
    {
        HRSRC hrsrc = FindResourceW(GetModuleHandle(NULL), MAKEINTRESOURCEW(m_idTemplate), (wchar_t*) RT_DIALOG);
        ttASSERT_MSG(hrsrc, "Missing dialog template");
        if (!hrsrc)
            return result;
    }
#endif
    ttASSERT_MSG(result != -1, "Failed to create dialog box");

    return result;
}

// Parent message loop must use returned handle in IsDialogMessage for keys to work in dialog

HWND dlg::DoModeless(HWND hwndParent)
{
    if (hwndParent)
        m_hwndParent = hwndParent;
    m_isModeless = true;
    return ::CreateDialogParamW(GetModuleHandle(NULL), MAKEINTRESOURCEW(m_idTemplate), m_hwndParent, (DLGPROC) ttlib::DlgProc, (LPARAM) this);
}

INT_PTR WINAPI ttlib::DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INITDIALOG)  // this is the only time that pThis will be NULL
    {
        SetWindowLongPtrW(hdlg, DWLP_USER, (LONG_PTR) lParam);
        auto pThis = reinterpret_cast<ttlib::dlg*>(lParam);
        pThis->m_hwnd = hdlg;
        if (!(pThis->m_hwndParent && IsWindow(pThis->m_hwndParent)))
            pThis->m_hwndParent = GetActiveWindow();

        INT_PTR lResult = 0;
        if (pThis->OnMsgMap(msg, wParam, lParam, lResult))
            return lResult;

        pThis->m_isInitializing = true;  // needed to make sure ttDDX_ macros work correctly
        pThis->m_isCancelEnd = false;
        pThis->OnBegin();
        pThis->m_isInitializing = false;
        return TRUE;
    }

    auto pThis = reinterpret_cast<ttlib::dlg*>(GetWindowLongPtrW(hdlg, DWLP_USER));
    if (!pThis)
        return FALSE;

    if (msg == WM_DESTROY)
    {
        if (pThis->m_pShadedBtns)
        {
            delete pThis->m_pShadedBtns;
            pThis->m_pShadedBtns = nullptr;
        }

        // We let the calling app see the WM_DESTROY message, but we ignore it's result
        LRESULT lResult;
        pThis->OnMsgMap(msg, wParam, lParam, lResult);

#if !defined(NDEBUG)  // Starts debug section.
        // This allows a destructor to verify that the window was destroyed before the destructor was called

        if (pThis->m_isModeless)
            pThis->m_hwnd = nullptr;
#endif

        return 0;
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
                    {
                        pThis->m_isCancelEnd = false;
                    }
                    else
                    {
                        // do NOT call EndDialog--it will fail if this is a modeless dialog
                        pThis->CloseDialog(IDOK);
                    }
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

void dlg::EnableShadeBtns()
{
    if (!m_pShadedBtns)
    {
        // This will be destroyed when processing WM_DESTROY rather than in the class's dtor
        m_pShadedBtns = new ttlib::MultiBtn;
        if (m_pShadedBtns)
            m_pShadedBtns->Initialize(*this);
    }
}

void dlg::SetBtnIcon(int idBtn, int idIcon, UINT nIconAlign)
{
    if (!m_pShadedBtns)
    {
        EnableShadeBtns();
        if (!m_pShadedBtns)
            return;
    }
    m_pShadedBtns->SetIcon(idBtn, idIcon, nIconAlign);
}

void dlg::SetBtnIcon(int idBtn, const std::string& IconName, UINT nIconAlign)
{
    if (!m_pShadedBtns)
    {
        EnableShadeBtns();
        if (!m_pShadedBtns)
            return;
    }
    m_pShadedBtns->SetIcon(idBtn, IconName.c_str(), nIconAlign);
}

void dlg::CenterWindow(bool isCenterOnDesktop)
{
    RECT rc;
    GetWindowRect(*this, &rc);

    int cx = ttlib::rcWidth(rc);
    int cy = ttlib::rcHeight(rc);

    if (!isCenterOnDesktop && m_hwndParent)
        GetWindowRect(m_hwndParent, &rc);
    else
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

    int left = rc.left + (ttlib::rcWidth(rc) - cx) / 2;
    int top = rc.top + (ttlib::rcHeight(rc) - cy) / 2;

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

int dlgListView::add(std::string_view str, LPARAM lparam)
{
    std::wstring str16;
    ttlib::utf8to16(str, str16);

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
    return static_cast<int>(::SendMessageW(m_hwnd, LVM_INSERTITEMW, 0, (LPARAM) &lvi));
}

BOOL dlgListView::addsubstring(std::string_view str, int iItem, int iSubItem)
{
    std::wstring str16;
    ttlib::utf8to16(str, str16);

    LVITEMW lvi;
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_TEXT;
    lvi.pszText = (LPWSTR) str16.c_str();
    lvi.iItem = iItem;
    lvi.iSubItem = iSubItem;
    return (BOOL)::SendMessageW(m_hwnd, LVM_SETITEMW, 0, (LPARAM) &lvi);
}

void dlgListView::InsertColumn(int iColumn, std::string_view utf8str, int width)
{
    std::wstring str16;
    ttlib::utf8to16(utf8str, str16);
    InsertColumn(iColumn, str16, width);
}

void dlgListView::InsertColumn(int iColumn, const std::wstring& str, int width)
{
    LV_COLUMNW lvc;
    lvc.mask = LVCF_FMT | LVCF_TEXT | (width == -1 ? 0 : LVCF_WIDTH);
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = width;
    lvc.pszText = const_cast<LPWSTR>(str.c_str());

    ::SendMessageW(m_hwnd, LVM_INSERTCOLUMNW, (WPARAM) iColumn, (LPARAM) &lvc);
}

LRESULT dlgListView::SetSel(WPARAM index)
{
    LVITEMW lvi;
    lvi.stateMask = 0x0F;
    lvi.state = LVIS_FOCUSED | LVIS_SELECTED;
    return ::SendMessageW(m_hwnd, LVM_SETITEMSTATE, index, (LPARAM) &lvi);
}

ttlib::cstr dlgListView::GetItemText(int item, int subitem, int maxTextLen)
{
    auto str16 = std::make_unique<wchar_t[]>(maxTextLen);
    LVITEMW lvi;
    lvi.mask = LVIF_TEXT;
    lvi.iItem = item;
    lvi.iSubItem = subitem;
    lvi.cchTextMax = maxTextLen;
    lvi.pszText = str16.get();
    auto len = ::SendMessageW(m_hwnd, LVM_GETITEMTEXTW, (WPARAM) item, (LPARAM) &lvi);
    ttASSERT(len < maxTextLen);

    ttlib::cstr utf8;
    ttlib::utf16to8({ str16.get(), static_cast<size_t>(len) }, utf8);
    return utf8;
}

LRESULT dlgListView::SetCurSel(std::string_view item)
{
    std::wstring str16;
    ttlib::utf8to16(item, str16);
    LV_FINDINFOW lvfi;
    lvfi.flags = LVFI_STRING;
    lvfi.psz = str16.data();
    auto pos = ::SendMessageW(m_hwnd, LVM_FINDITEM, (WPARAM) -1, (LPARAM) &lvfi);
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
    static bool hasTried = false;
    if (hasTried)
        return (s_pfnGetMonitorInfo != NULL);

    hasTried = true;
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
