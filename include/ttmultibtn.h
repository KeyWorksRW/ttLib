/////////////////////////////////////////////////////////////////////////////
// Name:      ttCMultiBtn
// Purpose:   Class for applying CShadeBtn to every button in a dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(_WIN32)

#include "ttarray.h"  // ttCArray

#ifdef _MSC_VER
#pragma warning(disable : 26495)  // m_btnShade is uninitialized (r.w.: which is fine, it's not used until Initialize is called which sets it)
#endif

namespace ttpriv
{
    BOOL WINAPI EnumBtnProc(HWND hwnd, LPARAM lval);
}

class ttCShadeBtn;

class ttCMultiBtn
{
public:
    ttCMultiBtn() {}
    ~ttCMultiBtn();

    void         SetIcon(int idBtn, int idIcon, UINT nIconAlign = BS_LEFT);
    void         SetIcon(int idBtn, const char* pszIconName, UINT nIconAlign = BS_LEFT);
    ttCShadeBtn* FindShadeBtn(int id);

    typedef enum
    {  // must be identical to ttCShadeBtn (ttshadebtn.h)
        SHS_NOISE = 0,
        SHS_DIAGSHADE = 1,
        SHS_HSHADE = 2,
        SHS_VSHADE = 3,
        SHS_HBUMP = 4,
        SHS_VBUMP = 5,
        SHS_SOFTBUMP = 6,
        SHS_HARDBUMP = 7,
        SHS_METAL = 8,
    } BTN_SHADE;

    // Class functions

    void Initialize(HWND hwndParent, BTN_SHADE shade = SHS_HARDBUMP);

private:
    friend BOOL WINAPI ttpriv::EnumBtnProc(HWND hwnd, LPARAM lval);

    // Class members

    ttCArray<ttCShadeBtn*> m_aBtns;
    BTN_SHADE              m_btnShade;
};

#endif  // defined(_WIN32)
