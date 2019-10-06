/////////////////////////////////////////////////////////////////////////////
// Name:      ttmsgmap.h
// Purpose:   Macros for mapping Windows messages to functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/*
    Place the following within a class that inherits from ttDlg or ttWin:

    BEGIN_TTMSG_MAP()
        // Insert a TTMSG macro for every message you want to process
    END_TTMSG_MAP()

    All macros return true to indicate that you handled the message and that the default window/dialog procedure should
    not be called. If the message requires a non-zero return, the called function will have a return type of LRESULT.
    Otherwise, the return type is void.

    For each macro you process you will need to declare a matching function. For example:

    BEGIN_TTMSG_MAP()
        TTMSG_BUTTON_CLICK(IDBTN_MYBTN, OnMyBtn)
        TTMSG(WM_INITMENU, OnInitMenu)
    END_TTMSG_MAP()

    void OnMyBtn();
    void OnInitMenu(WPARAM wParam, LPARAM lParam);

    Additional Notes:

        TTMSG(msg, func) can be used to process any message. However, any macros placed below TTMSG() will not be
        called if they are checking for the same msg as TTMSG().

        TTMSG_COMMAND(id, func) can be used to process any WM_COMMAND message. However, any of the other macros that
        handle WM_COMMAND will not be called unless placed BEFORE you call this macro.

    If you have a large number of menu commands to handle, you can use the following to convert them into a switch
    statement instead of multiple if statements which may result in better compiler optimization.

    BEGIN_TTMSG_MAP()
        BEGIN_TTCMD_SWITCH()
            TTCMD(IDM_MYMENU, OnMyMenu)
            TTCMD(IDM_ABOUT, OnAbout)
        END_TTCMD_SWITCH()
    END_TTMSG_MAP()

    void OnMyMenu();
    void OnAbout();

*/

#pragma once

// lResult is set to zero before OnMsgMap is called, so only change if non-zero result is needed
#define BEGIN_TTMSG_MAP()                                                    \
    bool OnMsgMap(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) \
    {                                                                        \
        uMsg;                                                                \
        wParam;                                                              \
        lParam;                                                              \
        lResult;
#define END_TTMSG_MAP() \
    return false;       \
    }

#define TTMSG_BUTTON_CLICK(id, func)                                                \
    if (uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == id) \
    {                                                                               \
        func();                                                                     \
        return true;                                                                \
    }

#define TTMSG_EDIT_CHANGE(id, func)                                                                \
    if (uMsg == WM_COMMAND && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == CBN_EDITCHANGE) && \
        LOWORD(wParam) == id)                                                                      \
    {                                                                                              \
        func();                                                                                    \
        return true;                                                                               \
    }

#define TTMSG_SEL_CHANGE(id, func)                                                                    \
    if (uMsg == WM_COMMAND && (HIWORD(wParam) == LBN_SELCHANGE || HIWORD(wParam) == CBN_SELCHANGE) && \
        LOWORD(wParam) == id)                                                                         \
    {                                                                                                 \
        func();                                                                                       \
        return true;                                                                                  \
    }

#define TTMSG_LISTBOX_DBL_CLICK(id, func)                                           \
    if (uMsg == WM_COMMAND && HIWORD(wParam) == LBN_DBLCLK && LOWORD(wParam) == id) \
    {                                                                               \
        func();                                                                     \
        return true;                                                                \
    }

// Use this for any WM_COMMAND messages not handled above. See TTCMD below for placing these in a switch statement
#define TTMSG_COMMAND(id, func)                             \
    if (uMsg == WM_COMMAND && LOWORD(wParam) == id)         \
    {                                                       \
        func((UINT) HIWORD(wParam), (UINT) LOWORD(wParam)); \
        return true;                                        \
    }

#define TTMSG_NOTIFY(id, func)                                       \
    if (uMsg == WM_NOTIFY && ((NMHDR*) lParam)->idFrom == (UINT) id) \
    {                                                                \
        func((NMHDR*) lParam);                                       \
        return true;                                                 \
    }

#define TTMSG_PAINT(func) \
    if (uMsg == WM_PAINT) \
    {                     \
        func();           \
        return true;      \
    }

#define TTMSG_CLOSE(func) \
    if (uMsg == WM_CLOSE) \
    {                     \
        func();           \
        return true;      \
    }

// LRESULT func(CREATESTRUCT* pcs)
#define TTMSG_CREATE(func)                      \
    if (uMsg == WM_CREATE)                      \
    {                                           \
        lResult = func((CREATESTRUCT*) lParam); \
        return true;                            \
    }

// LRESULT func(DRAWITEMSTRUCT* pdis)
#define TTMSG_DRAWITEM(func)                      \
    if (uMsg == WM_DRAWITEM)                      \
    {                                             \
        lResult = func((DRAWITEMSTRUCT*) lParam); \
        return true;                              \
    }

#define TTMSG_DESTROY(func) \
    if (uMsg == WM_DESTROY) \
    {                       \
        func();             \
        return true;        \
    }

// Use this for any WM_ messages not handled above
// LRESULT func(WPARAM wParam, LPARAM lParam)
#define TTMSG(msg, func)                \
    if (uMsg == msg)                    \
    {                                   \
        lResult = func(wParam, lParam); \
        return true;                    \
    }

// Use the following if you have a large number of commands to process. The switch statement will typically result in
// better performance with a large number of items then the if statements used above.

#ifndef __TTLIB_TTMSG_SWITCH__
    #define __TTLIB_TTMSG_SWITCH__

    #define BEGIN_TTCMD_SWITCH()    \
        if (uMsg == WM_COMMAND)     \
        {                           \
            switch (LOWORD(wParam)) \
            {
    #define END_TTCMD_SWITCH() \
        default:               \
            return false;      \
            }                  \
            }

    #define ttCASE_CMD(id, func) \
        case id:                 \
            func();              \
            return true;

    #define BEGIN_TTMSG_SWITCH() \
        {                        \
            switch (uMsg)        \
            {
    #define END_TTMSG_SWITCH() \
        default:               \
            return false;      \
            }                  \
            }

    // LRESULT func(WPARAM wParam, LPARAM lParam)
    #define ttCASE_MSG(msg, func)           \
        case msg:                           \
            lResult = func(wParam, lParam); \
            return true;

#endif  // __TTLIB_TTMSG_SWITCH__
