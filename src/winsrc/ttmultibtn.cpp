/////////////////////////////////////////////////////////////////////////////
// Name:      ttCMultiBtn
// Purpose:   Class for applying ttCShadeBtn to every button in a dialog
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <sstream>

#include "ttdebug.h"
#include "ttlibspace.h"
#include "ttmultibtn.h"  // ttCMultiBtn
#include "ttshadebtn.h"  // ttCShadeBtn

BOOL WINAPI ttpriv::EnumBtnProc(HWND hwnd, LPARAM lval)
{
    char szClass[MAX_PATH];
    if ((GetWindowLong(hwnd, GWL_STYLE) & 0x0f) < BS_CHECKBOX)
    {
        GetClassNameA(hwnd, szClass, sizeof(szClass));
        if (ttlib::issameas(szClass, "Button", tt::CASE::either))
        {
            ttCMultiBtn* pMultiBtn = (ttCMultiBtn*) lval;
            auto& btn = pMultiBtn->m_lstButtons.emplace_back();
            btn.SubClass(hwnd);
            btn.SetShade((ttCShadeBtn::BTN_SHADE) pMultiBtn->m_btnShade);
        }
    }
    return TRUE;
}

ttCMultiBtn::~ttCMultiBtn() {}

void ttCMultiBtn::Initialize(HWND hwndParent, BTN_SHADE shade)
{
    m_btnShade = shade;
    m_lstButtons.clear();

    EnumChildWindows(hwndParent, (WNDENUMPROC) ttpriv::EnumBtnProc, (LPARAM) this);
}

void ttCMultiBtn::SetIcon(int idBtn, int idIcon, UINT nIconAlign)
{
    ttASSERT_MSG(
        m_lstButtons.size(),
        "Calling SetIcon without any buttons to set (Initialize not called? EnableShadeBtns not called?)");

    for (auto& iter: m_lstButtons)
    {
        if (IsWindow(iter.gethwnd()) && iter.isSameID(idBtn))
        {
            iter.SetIcon(idIcon, nIconAlign);
            return;
        }
    }

#if !defined(NDEBUG)  // Starts debug section.
    std::stringstream msg;
    msg << "ttCMultiBtn::SetIcon was unable to find the button id: " << idBtn;
    ttlib::wintrace(msg.str());
#endif
}

void ttCMultiBtn::SetIcon(int idBtn, const char* pszIconName, UINT nIconAlign)
{
    ttASSERT_MSG(
        m_lstButtons.size(),
        "Calling SetIcon without any buttons to set (Initialize not called? EnableShadeBtns not called?)");

    for (auto& iter: m_lstButtons)
    {
        if (IsWindow(iter.gethwnd()) && iter.isSameID(idBtn))
        {
            iter.SetIcon(pszIconName, nIconAlign);
            return;
        }
    }
#if !defined(NDEBUG)  // Starts debug section.
    std::stringstream msg;
    msg << "ttCMultiBtn::SetIcon was unable to find the button id: " << idBtn;
    ttlib::wintrace(msg.str());
#endif
}

ttCShadeBtn* ttCMultiBtn::FindShadeBtn(int id)
{
    ttASSERT_MSG(
        m_lstButtons.size(),
        "Calling FindShadeBtn without any buttons to set (Initialize not called? EnableShadeBtns not called?)");

    for (size_t pos = 0; pos < m_lstButtons.size(); ++pos)
    {
        if (m_lstButtons[pos].isSameID(id))
            return &m_lstButtons[pos];
    }

    return nullptr;
}
