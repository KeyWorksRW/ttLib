/////////////////////////////////////////////////////////////////////////////
// Name:      shadebtn.h
// Purpose:   Class for implementing an owner-draw button
// Author:    Davide Pizzolato
// Copyright: Copyright (c) [2001-2019] Davide Pizzolato
// Licence:   The Code Project Open License (see ../CPOL.md)
/////////////////////////////////////////////////////////////////////////////

//////////////// Derivative work ////////////////////////////////////////////
// Changes:   Derived from CxShadeButton, changed to remove MFC requirement
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// Notes:     The above Code Project License also applies to the derivative work
/////////////////////////////////////////////////////////////////////////////

// For the original, see article at https://www.codeproject.com/articles/1121/ttCShadeBtn

#pragma once

#if defined(_WIN32)

#include "ttdib.h"  // ttCDib
#include "ttwin.h"  // ttCWin

class ttCShadeBtn : public ttCWin
{
public:
    ttCShadeBtn();
    ~ttCShadeBtn();

    typedef enum
    {
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

    void      Draw3dRect(HDC hdc, RECT* pRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
    void      Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
    void      FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);
    LOGFONTA* GetFont() { return m_pLF; }
    void      SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE);
    void      SetFlat(bool bFlag) { m_flat = bFlag; }
    bool      SetFont(LOGFONTA* pNewStyle);
    bool      SetFont(const char* pszFontName, long lSize = 0, long lWeight = 400, BYTE bItalic = 0, BYTE bUnderline = 0);

    void SetIcon(UINT nIcon, UINT nIconAlign = BS_CENTER, UINT nIconDown = 0, UINT nIconHighLight = 0);
    void SetIcon(const char* pszIconName, UINT nIconAlign = BS_CENTER, UINT nIconDown = 0, UINT nIconHighLight = 0);
    void SetIcon(HICON hIcon, UINT nIconAlign, UINT nIconDown, UINT nIconHighLight);

    void     SetShade(BTN_SHADE shadeID = SHS_NOISE, BYTE granularity = 8, BYTE highlight = 10, BYTE coloring = 0, COLORREF color = 0);
    void     SetTextAlign(UINT nTextAlign);
    COLORREF SetTextColor(COLORREF new_color);

    bool operator==(ttCShadeBtn* pShade) { return m_hwnd == pShade->m_hwnd; }
    operator HWND() const { return m_hwnd; }

protected:
    // Message handlers

    BEGIN_TTMSG_MAP()
        TTMSG_WM_PAINT(OnPaint)

        case WM_ENABLE:
            InvalidateRect(*this, NULL, TRUE);  // REVIEW: [randalphwa - 1/26/2019] Can we get away with setting FALSE for bErase?
            return true;

        case BM_SETSTATE:
            InvalidateRect(*this, NULL, TRUE);
            return false;  // let default process this
    END_TTMSG_MAP()

    void OnPaint();

    // Class members

    BOOL      m_Border;     // 0=flat; 1=3D;
    COLORREF  m_TextColor;  // button text color
    DWORD     m_Style;
    HFONT     m_hFont;  // font object
    HICON     m_hIcon, m_hIconDown, m_hIconHighLight;
    LOGFONTA* m_pLF;  // font structure
    RECT      m_rcIconBox;
    UINT      m_hIconAlign;
    UINT      m_TextAlign;
    ttCDib    m_dNormal, m_dDown, m_dDisabled, m_dOver, m_dh, m_dv;
    bool      m_Checked;     // radio & check buttons
    bool      m_IsPushLike;  // radio & check buttons
    bool      m_flat;
    short     m_FocusRectMargin;  // dotted margin offset
};

#endif  // defined(_WIN32)
