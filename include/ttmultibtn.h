/////////////////////////////////////////////////////////////////////////////
// Name:      ttCMultiBtn
// Purpose:   Class for applying CShadeBtn to every button in a dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <vector>

#include "ttshadebtn.h"

namespace ttpriv
{
    BOOL WINAPI EnumBtnProc(HWND hwnd, LPARAM lval);
}

// class ttCShadeBtn;

// Class for applying CShadeBtn to every button in a dialog
class ttCMultiBtn
{
public:
    ttCMultiBtn() {}
    ~ttCMultiBtn();

    void SetIcon(int idBtn, int idIcon, UINT nIconAlign = BS_LEFT);
    void SetIcon(int idBtn, const char* pszIconName, UINT nIconAlign = BS_LEFT);
    ttCShadeBtn* FindShadeBtn(int id);

    enum BTN_SHADE
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
    };

    // Class functions

    void Initialize(HWND hwndParent, BTN_SHADE shade = SHS_HARDBUMP);

private:
    friend BOOL WINAPI ttpriv::EnumBtnProc(HWND hwnd, LPARAM lval);

    // Class members

    std::vector<ttCShadeBtn> m_lstButtons;

    BTN_SHADE m_btnShade { SHS_HARDBUMP };
};
