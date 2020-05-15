/////////////////////////////////////////////////////////////////////////////
// Name:      ttmultibtn.h
// Purpose:   Class for applying CShadeBtn to every button in a dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <vector>

#include "ttshadebtn.h"  // Specifies all tt::SHADE values

namespace ttlib
{
    BOOL WINAPI EnumBtnProc(HWND hwnd, LPARAM lval);

    // Class for applying CShadeBtn to every button in a dialog
    class MultiBtn
    {
    public:
        MultiBtn() {}
        ~MultiBtn();

        void SetIcon(int idBtn, int idIcon, UINT nIconAlign = BS_LEFT);
        void SetIcon(int idBtn, const std::string& IconName, UINT nIconAlign = BS_LEFT);

        // Class functions

        void Initialize(HWND hwndParent, tt::SHADE shade = tt::SHADE::bump_hard);

    private:
        friend BOOL WINAPI ttlib::EnumBtnProc(HWND hwnd, LPARAM lval);

        // Class members

        std::vector<ttlib::ShadeBtn*> m_Buttons;

        tt::SHADE m_btnShade;
    };

}  // namespace ttlib
