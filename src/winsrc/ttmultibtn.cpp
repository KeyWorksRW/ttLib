/////////////////////////////////////////////////////////////////////////////
// Name:      ttmultibtn.cpp
// Purpose:   Class for applying ttlib::ShadeBtn to every button in a dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <sstream>

#include "ttdebug.h"
#include "ttlibspace.h"
#include "ttmultibtn.h"  // MultiBtn
#include "ttshadebtn.h"  // ttlib::ShadeBtn

using namespace ttlib;

BOOL WINAPI ttlib::EnumBtnProc(HWND hwnd, LPARAM lval)
{
    char szClass[MAX_PATH];
    if ((GetWindowLong(hwnd, GWL_STYLE) & 0x0f) < BS_CHECKBOX)
    {
        GetClassNameA(hwnd, szClass, sizeof(szClass));
        if (ttlib::issameas(szClass, "Button", tt::CASE::either))
        {
            ttlib::MultiBtn* pMultiBtn = reinterpret_cast<MultiBtn*>(lval);
            ttlib::ShadeBtn* pBtn = new ttlib::ShadeBtn;
            pBtn->Initialize(hwnd, pMultiBtn->m_btnShade);
            pMultiBtn->m_Buttons.emplace_back(pBtn);

        }
    }
    return TRUE;
}

MultiBtn::~MultiBtn()
{
    for (auto pbtn: m_Buttons)
        delete pbtn;
}

void MultiBtn::Initialize(HWND hwndParent, tt::SHADE shade)
{
    m_btnShade = shade;

    // The dialog can be created more than once, which means we get Initialized more than once.
    for (auto pbtn: m_Buttons)
        delete pbtn;
    m_Buttons.clear();

    EnumChildWindows(hwndParent, (WNDENUMPROC) ttlib::EnumBtnProc, (LPARAM) this);
}

void MultiBtn::SetIcon(int idBtn, int idIcon, UINT nIconAlign)
{
    ttASSERT_MSG(m_Buttons.size(), "Calling SetIcon without any buttons to set (Initialize not called? EnableShadeBtns not called?)");

    for (auto pbtn: m_Buttons)
    {
        if (IsWindow(*pbtn) && GetDlgCtrlID(*pbtn) == idBtn)
        {
            pbtn->SetIcon(idIcon, nIconAlign);
            return;
        }
    }

#if !defined(NDEBUG)  // Starts debug section.
    std::stringstream msg;
    msg << "MultiBtn::SetIcon was unable to find the button id: " << idBtn;
    ttlib::wintrace(msg.str());
#endif
}

void MultiBtn::SetIcon(int idBtn, const std::string& IconName, UINT nIconAlign)
{
    ttASSERT_MSG(m_Buttons.size(), "Calling SetIcon without any buttons to set (Initialize not called? EnableShadeBtns not called?)");

    for (auto pbtn: m_Buttons)
    {
        if (IsWindow(*pbtn) && GetDlgCtrlID(*pbtn) == idBtn)
        {
            pbtn->SetIcon(IconName, nIconAlign);
            return;
        }
    }

#if !defined(NDEBUG)  // Starts debug section.
    std::stringstream msg;
    msg << "MultiBtn::SetIcon was unable to find the button id: " << idBtn;
    ttlib::wintrace(msg.str());
#endif
}
