/////////////////////////////////////////////////////////////////////////////
// Name:      ttshadebtn.h
// Purpose:   Class for implementing an owner-draw button
// Author:    Davide Pizzolato
// Copyright: Copyright (c) [2001-2019] Davide Pizzolato
// License:   CPOL License -- see ../license_code_project.htm
/////////////////////////////////////////////////////////////////////////////

//////////////// Derivative work ////////////////////////////////////////////
// Changes:   Derived from CxShadeButton, changed to remove MFC requirement
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2020 KeyWorks Software (Ralph Walden)
/////////////////////////////////////////////////////////////////////////////

// For the original, see article at https://www.codeproject.com/articles/1121/ShadeBtn

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <wingdi.h>
#include <wtypes.h>

#include "ttdib.h"  // ttCDib
#include "ttwin.h"  // ttlib::win -- Class for working with windows

namespace tt
{
    enum class SHADE
    {
        noise,
        diagonal,
        horizontal,
        vertical,
        bump_hrz,
        bump_vrt,
        bump_soft,
        bump_hard,
        metal,
    };
}  // namespace tt

namespace ttlib
{
    // Class for implementing an owner-draw button
    class ShadeBtn final : public ttlib::win
    {
    public:
        ShadeBtn();
        ~ShadeBtn();

        // Class functions

        void Draw3dRect(HDC hdc, RECT* pRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
        void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
        void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);
        void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE);
        void SetFlat(bool bFlag) { m_flat = bFlag; }

        void SetIcon(UINT nIcon, UINT nIconAlign = BS_CENTER, UINT nIconDown = 0, UINT nIconHighLight = 0);
        void SetIcon(const std::string& IconName, UINT nIconAlign = BS_CENTER, UINT nIconDown = 0, UINT nIconHighLight = 0);
        void SetIcon(HICON hIcon, UINT nIconAlign, UINT nIconDown, UINT nIconHighLight);

        void SetShade(tt::SHADE shadeID = tt::SHADE::noise, BYTE granularity = 8, BYTE highlight = 10, BYTE coloring = 0,
                      COLORREF color = 0);
        void SetTextAlign(UINT nTextAlign);
        COLORREF SetTextColor(COLORREF new_color);

        HWND gethwnd() { return m_hwnd; }
        int getID() { return GetDlgCtrlID(m_hwnd); }
        bool isSameID(int id) { return getID() == id; }

        bool operator==(ShadeBtn* pShade) { return m_hwnd == pShade->m_hwnd; }
        operator HWND() const { return m_hwnd; }

        void Initialize(HWND hwnd, tt::SHADE shadeID);

    protected:
        // Message handlers

        BEGIN_TTMSG_MAP()
            TTMSG_WM_PAINT(OnPaint)

            case WM_SETTEXT:
                lResult = DefWindowProc(*this, msg, wParam, lParam);
                OnSetText();
                return true;

            case WM_ENABLE:
                [[fallthrough]];
            case WM_SETFOCUS:
                [[fallthrough]];
            case BM_SETSTATE:
                InvalidateRect(*this, NULL, TRUE);
                return false;

        END_TTMSG_MAP()

        void OnPaint();
        LRESULT OnCreate(CREATESTRUCT* pcs);
        void OnSetText();

    private:
        std::wstring m_btntext;

        BOOL m_Border;         // 0=flat; 1=3D;
        COLORREF m_TextColor;  // button text color
        DWORD m_Style;
        HFONT m_hFont;  // font object
        HICON m_hIcon, m_hIconDown, m_hIconHighLight;
        RECT m_rcIconBox;
        UINT m_hIconAlign;
        UINT m_TextAlign;
        ttCDib m_dNormal, m_dDown, m_dDisabled, m_dOver, m_dh, m_dv;
        bool m_Checked;     // radio & check buttons
        bool m_IsPushLike;  // radio & check buttons
        bool m_flat;
        short m_FocusRectMargin;  // dotted margin offset
    };

}  // namespace ttlib
