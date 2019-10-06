/////////////////////////////////////////////////////////////////////////////
// Name:      ttcasemap.h
// Purpose:   Macros for mapping Windows messages to functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
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
                return true;    // to indicate we processed the message
        END_TTMSG_MAP()

    If you cannot find a macro for the message you want to process, use the TTMSG_ANY macro
*/

#pragma once

// clang-format off
#ifndef GET_X_LPARAM
    #define GET_X_LPARAM(lp) ((int) (short) LOWORD(lp))
    #define GET_Y_LPARAM(lp) ((int) (short) HIWORD(lp))
#endif

#define BEGIN_TTCMD_MAP() bool OnCmdCaseMap(int id, int NotifyCode, LRESULT& lResult) { NotifyCode; lResult; switch (id) {
#define BEGIN_TTMSG_MAP() bool OnMsgMap(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) { wParam; lParam; lResult; switch (msg) {
#define END_TTMSG_MAP() default: return false; } }

////////////////////////////////////////////////////////////////
// The following macros are for use in a BEGIN_TTCMD_MAP block
////////////////////////////////////////////////////////////////

// void func(); // TTCASE_CMD calls func() without passing the NotifyCode
#define TTCASE_CMD(id, func) \
    case id: \
        func(); \
        return true;

// void func(int NotifyCode);   // use this to check the notification code
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

// LRESULT OnNotify(int id, NMHDR* pNmHdr);
#define TTMSG_WM_NOTIFY(func) \
    case WM_NOTIFY: \
        lResult = func((int) wParam, (NMHDR*) lParam); \
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

// void OnSize(UINT fSizeType, int xPos, int yPos);
#define TTMSG_WM_SIZE(func) \
    case WM_SIZE: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnActivate(UINT flag, BOOL bMinimized, HWND hwndPrevious);
#define TTMSG_WM_ACTIVATE(func) \
    case WM_ACTIVATE: \
        func((UINT) LOWORD(wParam), (BOOL) HIWORD(wParam), (HWND) lParam); \
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

// LRESULT OnQueryEndSession(UINT flags);
#define TTMSG_WM_QUERYENDSESSION(func) \
    case WM_QUERYENDSESSION: \
        lResult = func((UINT) lParam); \
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

// void OnEndSession(BOOL bSessionEnding, UINT flags);
#define TTMSG_WM_ENDSESSION(func) \
    case WM_ENDSESSION: \
        func((BOOL) wParam, (UINT) lParam); \
        return true;

// void OnShowWindow(BOOL bShow, UINT fStatus);
#define TTMSG_WM_SHOWWINDOW(func) \
    case WM_SHOWWINDOW: \
        func((BOOL) wParam, (UINT) lParam); \
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

// void OnSettingChange(UINT flag, const char* pszMetrics);
#define TTMSG_WM_SETTINGCHANGE(func) \
    case WM_SETTINGCHANGE: \
        func((UINT) wParam, (const char*) lParam); \
        return true;

// void OnDevModeChange(const char* pszDevice)
#define TTMSG_WM_DEVMODECHANGE(func) \
    case WM_DEVMODECHANGE: \
        func((const char*) lParam); \
        return true;

// void OnActivateApp(BOOL bActivated, int idThread);
#define TTMSG_WM_ACTIVATEAPP(func) \
    case WM_ACTIVATEAPP: \
        func((BOOL) wParam, (int) lParam); \
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

// BOOL OnSetCursor(HWND hwnd, UINT hitTest, UINT msg);
#define TTMSG_WM_SETCURSOR(func) \
    case WM_SETCURSOR: \
        lResult = (LRESULT) func((HWND) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
        return true;

// LRESULT OnMouseActivate(HWND hwndTopParent, UINT hitTest, UINT msg);
#define TTMSG_WM_MOUSEACTIVATE(func) \
    case WM_MOUSEACTIVATE: \
        lResult = func((HWND) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
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

// void OnSpoolerStatus(UINT flag, UINT cJobsLeft);
#define TTMSG_WM_SPOOLERSTATUS(func) \
    case WM_SPOOLERSTATUS: \
        func((UINT) wParam, (UINT) LOWORD(lParam)); \
        return true;

// BOOL OnDrawItem(int id, DRAWITEMSTRUCT* pdis)
#define TTMSG_WM_DRAWITEM(func) \
    case WM_DRAWITEM: \
        lResult = (LRESULT) func((int) wParam, (DRAWITEMSTRUCT*) lParam); \
        return true;

// BOOL OnMeasureItem(int id, MEASUREITEMSTRUCT* pmis);
#define TTMSG_WM_MEASUREITEM(func) \
    case WM_MEASUREITEM: \
        lResult = (LRESULT) func((int) wParam, (MEASUREITEMSTRUCT*) lParam); \
        return true;

// void OnDeleteItem(int id, DELETEITEMSTRUCT* pdis);
#define TTMSG_WM_DELETEITEM(func) \
    case WM_DELETEITEM: \
        lResult = TRUE; \
        func((int) wParam, (DELETEITEMSTRUCT*) lParam); \
        return true;

// LRESULT OnCharToItem(UINT key, UINT CaretPos, HWND hwndLB);
#define TTMSG_WM_CHARTOITEM(func) \
    case WM_CHARTOITEM: \
        lResult = func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), (HWND) lParam); \
        return true;

// LRESULT OnVKeyToItem(UINT vkey, UINT CaretPos, HWND hwndLB);
#define TTMSG_WM_VKEYTOITEM(func) \
    case WM_VKEYTOITEM: \
        lResult = func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam), (HWND) lParam); \
        return true;

// HANDLE OnQueryDragIcon();
#define TTMSG_WM_QUERYDRAGICON(func) \
    case WM_QUERYDRAGICON: \
        lResult = (LRESULT) func(); \
        return true;

// LRESULT OnCompareItem(int idCtrl, COMPAREITEMSTRUCT* pcis);
#define TTMSG_WM_COMPAREITEM(func) \
    case WM_COMPAREITEM: \
        lResult = func((int) wParam, (COMPAREITEMSTRUCT*) lParam); \
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

// void OnNcMouseMove(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMOUSEMOVE(func) \
    case WM_NCMOUSEMOVE: \
        func((int) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcLBtnDwn(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCLBUTTONDOWN(func) \
    case WM_NCLBUTTONDOWN: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcLBtnUp(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCLBUTTONUP(func) \
    case WM_NCLBUTTONUP: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcLBtnDblClk(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCLBUTTONDBLCLK(func) \
    case WM_NCLBUTTONDBLCLK: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcRBtnDwn(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCRBUTTONDOWN(func) \
    case WM_NCRBUTTONDOWN: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcRBtnUp(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCRBUTTONUP(func) \
    case WM_NCRBUTTONUP: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcRBtnDblClk(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCRBUTTONDBLCLK(func) \
    case WM_NCRBUTTONDBLCLK: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcMBtnDwn(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMBUTTONDOWN(func) \
    case WM_NCMBUTTONDOWN: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcMBtnUp(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMBUTTONUP(func) \
    case WM_NCMBUTTONUP: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnNcMBtnDblClk(UINT hitTest, int xPos, int yPos);
#define TTMSG_WM_NCMBUTTONDBLCLK(func) \
    case WM_NCMBUTTONDBLCLK: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnKeyDown(UINT virtKey, int cReps, UINT flags);
#define TTMSG_WM_KEYDOWN(func) \
    case WM_KEYDOWN: \
        func((UINT) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnKeyUp(size_t virtKey, int cReps, UINT flags);
#define TTMSG_WM_KEYUP(func) \
    case WM_KEYUP: \
        func((UINT) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnChar(char ch, int cReps, UINT flags);
#define TTMSG_WM_CHAR(func) \
    case WM_CHAR: \
        func((char) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnDeadChar(char ch, int cReps, UINT flags);
#define TTMSG_WM_DEADCHAR(func) \
    case WM_DEADCHAR: \
        func((char) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnSysKeyDown(size_t virtKey, int cReps, UINT flags);
#define TTMSG_WM_SYSKEYDOWN(func) \
    case WM_SYSKEYDOWN: \
        func((int) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnSysKeyUp(size_t virtKey, int cReps, UINT flags);
#define TTMSG_WM_SYSKEYUP(func) \
    case WM_SYSKEYUP: \
        func((int) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnSysChar(char ch, int cReps, UINT flags);
#define TTMSG_WM_SYSCHAR(func) \
    case WM_SYSCHAR: \
        func((char) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnSysDeadChar(char ch, int cReps, UINT flags);
#define TTMSG_WM_SYSDEADCHAR(func) \
    case WM_SYSDEADCHAR: \
        func((char) wParam, (int) lParam & 0xFFFF, (UINT) ((lParam & 0xFFFF0000) >> 16)); \
        return true;

// void OnSysCommand(UINT cmdTYpe, int x, int y);
#define TTMSG_WM_SYSCOMMAND(func) \
    case WM_SYSCOMMAND: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnTCard(size_t idAction, long data);
#define TTMSG_WM_TCARD(func) \
    case WM_TCARD: \
        func((int) wParam, (long) lParam); \
        return true;

// void OnTimer(int id);
#define TTMSG_WM_TIMER(func) \
    case WM_TIMER: \
        func((int) wParam); \
        return true;

// void OnHScroll(UINT SBCode, int pos, HWND hwndSB);
#define TTMSG_WM_HSCROLL(func) \
    case WM_HSCROLL: \
        func((UINT) LOWORD(wParam), (int) HIWORD(wParam), (HWND) lParam); \
        return true;

// void OnVScroll(UINT SBCode, int pos, HWND hwndSB);
#define TTMSG_WM_VSCROLL(func) \
    case WM_VSCROLL: \
        func((UINT) LOWORD(wParam), (int) HIWORD(wParam), (HWND) lParam); \
        return true;

// void OnInitMenu(HMENU hMenu);
#define TTMSG_WM_INITMENU(func) \
    case WM_INITMENU: \
        func((HMENU) wParam); \
        return true;

// void OnInitMenuPopup(HMENU hSubMenu, int pos, BOOL bSysMenu);
#define TTMSG_WM_INITMENUPOPUP(func) \
    case WM_INITMENUPOPUP: \
        func((HMENU) wParam, (int) LOWORD(lParam), (BOOL) HIWORD(lParam)); \
        return true;

// void OnMenuSelect(int id, UINT flags, HMENU hMenu);
#define TTMSG_WM_MENUSELECT(func) \
    case WM_MENUSELECT: \
        func((int) LOWORD(wParam), (UINT) HIWORD(wParam), (HMENU) lParam); \
        return true;

// LRESULT OnMenuChar(char ch, UINT flag, HMENU hMenu);
#define TTMSG_WM_MENUCHAR(func) \
    case WM_MENUCHAR: \
        lResult = func((char) LOWORD(wParam), (UINT) HIWORD(wParam), (HMENU) lParam); \
        return true;

// void OnEnterIdle(UINT flag, HWND hwnd);
#define TTMSG_WM_ENTERIDLE(func) \
    case WM_ENTERIDLE: \
        func((UINT) wParam, (HWND) lParam); \
        return true;

// void OnMouseMove(UINT flags, int xPos, int yPos);
#define TTMSG_WM_MOUSEMOVE(func) \
    case WM_MOUSEMOVE: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnMouseWheel(UINT flags, ptrdiff_t delta, int xPos, int yPos);
#define TTMSG_WM_MOUSEWHEEL(func) \
    case WM_MOUSEWHEEL: \
        func((UINT) LOWORD(wParam), (ptrdiff_t) HIWORD(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnLBtnDwn(UINT flags, int xPos, int yPos);
#define TTMSG_WM_LBUTTONDOWN(func) \
    case WM_LBUTTONDOWN: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnLBtnUp(UINT flags, int xPos, int yPos);
#define TTMSG_WM_LBUTTONUP(func) \
    case WM_LBUTTONUP: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnLBtnDblClk(UINT flagVKeys, int xPos, int yPos);  // flagVKeys indicates which virtual keys are pressed (Ctrl, Shift, etc.)
#define TTMSG_WM_LBUTTONDBLCLK(func) \
    case WM_LBUTTONDBLCLK: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnRBtnDwn(UINT flags, int xPos, int yPos);
#define TTMSG_WM_RBUTTONDOWN(func) \
    case WM_RBUTTONDOWN: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnRBtnUp(UINT flags, int xPos, int yPos);
#define TTMSG_WM_RBUTTONUP(func) \
    case WM_RBUTTONUP: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnRBtnDblClk(UINT flags, int xPos, int yPos);
#define TTMSG_WM_RBUTTONDBLCLK(func) \
    case WM_RBUTTONDBLCLK: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnMBtnDwn(UINT flags, int xPos, int yPos);
#define TTMSG_WM_MBUTTONDOWN(func) \
    case WM_MBUTTONDOWN: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnMBtnUp(UINT flags, int xPos, int yPos);
#define TTMSG_WM_MBUTTONUP(func) \
    case WM_MBUTTONUP: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnMBtnDblClk(UINT flags, int xPos, int yPos);
#define TTMSG_WM_MBUTTONDBLCLK(func) \
    case WM_MBUTTONDBLCLK: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnEnable(BOOL bEnable);
#define TTMSG_WM_ENABLE(func) \
    case WM_ENABLE: \
        func((BOOL) wParam); \
        return true;

// void OnParentNotify(UINT event, int idChild, LPARAM lParam);
#define TTMSG_WM_PARENTNOTIFY(func) \
    case WM_PARENTNOTIFY: \
        func((UINT) LOWORD(wParam), (int) HIWORD(wParam), lParam); \
        return true;

// void OnMDIActivate(HWND hwndActivate, HWND hwndDeactivate);
#define TTMSG_WM_MDIACTIVATE(func) \
    case WM_MDIACTIVATE: \
        func((HWND) wParam, (HWND) lParam); \
        return true;

// void OnRenderFormat(UINT format);
#define TTMSG_WM_RENDERFORMAT(func) \
    case WM_RENDERFORMAT: \
        func((UINT) wParam); \
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

// void OnVScrollClipboard(HWND hwndViewer, UINT event, int pos);
#define TTMSG_WM_VSCROLLCLIPBOARD(func) \
    case WM_VSCROLLCLIPBOARD: \
        func((HWND) wParam, (UINT) LOWORD(lParam), (int) HIWORD(lParam)); \
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

// void OnHScrollClipboard(HWND hwndViewer, UINT event, int pos);
#define TTMSG_WM_HSCROLLCLIPBOARD(func) \
    case WM_HSCROLLCLIPBOARD: \
        func((HWND) wParam, (UINT) LOWORD(lParam), (int) HIWORD(lParam)); \
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

// void OnHotKey(id idKey, UINT modifiers, UINT vkey);
#define TTMSG_WM_HOTKEY(func) \
    case WM_HOTKEY: \
        func((int) wParam, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam)); \
        return true;

// void OnInputLangChange(UINT charset, HKL hKbdLayout);
#define TTMSG_WM_INPUTLANGCHANGE(func) \
    case WM_INPUTLANGCHANGE: \
        func((UINT) wParam, (HKL) lParam); \
        return true;

// void OnInputLangChangeRequest(UINT flag, HKL hKbdLayout);
#define TTMSG_WM_INPUTLANGCHANGEREQUEST(func) \
    case WM_INPUTLANGCHANGEREQUEST: \
        func((UINT) wParam, (HKL) lParam); \
        return true;

// void OnNextDlgCtl(int idFlag, BOOL bFlagType);
#define TTMSG_WM_NEXTDLGCTL(func) \
    case WM_NEXTDLGCTL: \
        func((int) wParam, (BOOL) LOWORD(lParam)); \
        return true;

// void OnNextMenu(UINT vkey, MDINEXTMENU* pMdiNextMenu);
#define TTMSG_WM_NEXTMENU(func) \
    case WM_NEXTMENU: \
        func((UINT) wParam, (MDINEXTMENU*) lParam); \
        return true;

// LRESULT OnNotifyFormat(HWND hwndFrom, UINT cmd);
#define TTMSG_WM_NOTIFYFORMAT(func) \
    case WM_NOTIFYFORMAT: \
        lResult = func((HWND) wParam, (UINT) lParam); \
        return true;

// LRESULT OnPowerBroadcast(UINT event, POWERBROADCAST_SETTING* pPBDSetting);
#define TTMSG_WM_POWERBROADCAST(func) \
    case WM_POWERBROADCAST: \
        lResult = func((UINT) wParam, (POWERBROADCAST_SETTING*) lParam); \
        return true;

// void OnPrint(HDC hdc, UINT drwFlags);
#define TTMSG_WM_PRINT(func) \
    case WM_PRINT: \
        func((HDC) wParam, (UINT) lParam); \
        return true;

// void OnPrintClient(HDC hdc, UINT drwFlags);
#define TTMSG_WM_PRINTCLIENT(func) \
    case WM_PRINTCLIENT: \
        func((HDC) wParam, (UINT) lParam); \
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

// LRESULT OnSetHotKey(UINT vkey, UINT uFlags);
#define TTMSG_WM_SETHOTKEY(func) \
    case WM_SETHOTKEY: \
        lResult = func((UINT) LOBYTE(LOWORD(wParam)), (UINT) HIBYTE(LOWORD(wParam))); \
        return true;

// HICON OnSetIcon(UINT type, HICON hIcon);
#define TTMSG_WM_SETICON(func) \
    case WM_SETICON: \
        lResult = (LRESULT) func((UINT) wParam, (HICON) lParam); \
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

// void OnMouseHover(UINT flags, int xPos, int yPos);
#define TTMSG_WM_MOUSEHOVER(func) \
    case WM_MOUSEHOVER: \
        func((UINT) wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnMouseLeave();
#define TTMSG_WM_MOUSELEAVE(func) \
    case WM_MOUSELEAVE: \
        func(); \
        return true;

// void OnMenuRButtonUp(UINT index, HANDLE hMenu);
#define TTMSG_WM_MENURBUTTONUP(func) \
    case WM_MENURBUTTONUP: \
        func((UINT) wParam, (HMENU) lParam); \
        return true;

// LRESULT OnMenuDrag(int pos, HANDLE hMenu);
#define TTMSG_WM_MENUDRAG(func) \
    case WM_MENUDRAG: \
        lResult = func((int) wParam, (HMENU) lParam); \
        return true;

// LRESULT OnMenuGetObject(MENUGETOBJECTINFO* pinfo);
#define TTMSG_WM_MENUGETOBJECT(func) \
    case WM_MENUGETOBJECT: \
        lResult = func((MENUGETOBJECTINFO*) lParam); \
        return true;

// void OnUnInitMenuPopup(HANDLE hMenu, UINT flag);
#define TTMSG_WM_UNINITMENUPOPUP(func) \
    case WM_UNINITMENUPOPUP: \
        func((HMENU) wParam, (UINT) HIWORD(lParam)); \
        return true;

// void OnMenuCommand(UINT index, HANDLE hMenu);
#define TTMSG_WM_MENUCOMMAND(func) \
    case WM_MENUCOMMAND: \
        func((UINT) wParam, (HMENU) lParam); \
        return true;

// BOOL OnAppCommand(HWND hwnd, UINT cmd, UINT device, UINT keyFlags);
#define TTMSG_WM_APPCOMMAND(func) \
    case WM_APPCOMMAND: \
        lResult = (LRESULT) func((HWND) wParam, (UINT) GET_APPCOMMAND_LPARAM(lParam), (UINT) GET_DEVICE_LPARAM(lParam), (UINT) GET_KEYSTATE_LPARAM(lParam)); \
        return true;

// BOOL OnNCXButtonDown(UINT fwButton, UINT HitTest, int xPos, int yPos);
#define TTMSG_WM_NCXBUTTONDOWN(func) \
    case WM_NCXBUTTONDOWN: \
        lResult = (LRESULT) func((UINT) GET_XBUTTON_WPARAM(wParam), (UINT) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// BOOL OnNCXButtonUp(UINT fwButton, UINT HitTest, int xPos, int yPos);
#define TTMSG_WM_NCXBUTTONUP(func) \
    case WM_NCXBUTTONUP: \
        lResult = (LRESULT) func((UINT) GET_XBUTTON_WPARAM(wParam), (UINT) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// BOOL OnNCXButtonDblClk(UINT fwButton, UINT HitTest, int xPos, int yPos);
#define TTMSG_WM_NCXBUTTONDBLCLK(func) \
    case WM_NCXBUTTONDBLCLK: \
        lResult = (LRESULT) func((UINT) GET_XBUTTON_WPARAM(wParam), (UINT) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// BOOL OnXButtonDown(UINT fwButton, UINT HitTest, int xPos, int yPos);
#define TTMSG_WM_XBUTTONDOWN(func) \
    case WM_XBUTTONDOWN: \
        lResult = (LRESULT) func((UINT) GET_XBUTTON_WPARAM(wParam), (UINT) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// BOOL OnXButtonUp(UINT fwButton, UINT HitTest, int xPos, int yPos);
#define TTMSG_WM_XBUTTONUP(func) \
    case WM_XBUTTONUP: \
        lResult = (LRESULT) func((UINT) GET_XBUTTON_WPARAM(wParam), (UINT) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// BOOL OnXButtonDblClk(UINT fwButton, UINT HitTest, int xPos, int yPos);
#define TTMSG_WM_XBUTTONDBLCLK(func) \
    case WM_XBUTTONDBLCLK: \
        lResult = (LRESULT) func((UINT) GET_XBUTTON_WPARAM(wParam), (UINT) GET_NCHITTEST_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
        return true;

// void OnChangeUIState(UINT action, UINT state);
#define TTMSG_WM_CHANGEUISTATE(func) \
    case WM_CHANGEUISTATE: \
        func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam)); \
        return true;

// void OnUpdateUIState(UINT action, UINT state);
#define TTMSG_WM_UPDATEUISTATE(func) \
    case WM_UPDATEUISTATE: \
        func((UINT) LOWORD(wParam), (UINT) HIWORD(wParam)); \
        return true;

// LRESULT OnQueryUIState()
#define TTMSG_WM_QUERYUISTATE(func) \
    case WM_QUERYUISTATE: \
        lResult = func(); \
        return true;

// void OnInput(UINT RawInputCode, HRAWINPUT hRawInput)
#define TTMSG_WM_INPUT(func) \
    case WM_INPUT: \
        func((UINT) GET_RAWINPUT_CODE_WPARAM(wParam), (HRAWINPUT) lParam); \
        return true;

// void OnThemeChanged()
#define TTMSG_WM_THEMECHANGED(func) \
    case WM_THEMECHANGED: \
        func(); \
        return true;

// BOOL OnMouseHWheel(UINT flags, int delta, int xPos, int yPos);
#define TTMSG_WM_MOUSEHWHEEL(func) \
    case WM_MOUSEHWHEEL: \
        lResult = (LRESULT) func((UINT) LOWORD(wParam), (int) HIWORD(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); \
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

// void OnDpiChanged(int xDpi, int yDpi, RECT* prc);
#define TTMSG_WM_DPICHANGED(func) \
    case WM_DPICHANGED: \
        func((int) LOWORD(wParam), (int) HIWORD(wParam), (RECT*) lParam); \
        return true;

// LRESULT func(WPARAM wParam, LPARAM lParam);
#define TTMSG_ANY(msg, func) \
    case msg: \
        lResult = func(wParam, lParam); \
        return true;

// clang-format on
