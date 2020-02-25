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

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#if !defined(TTALL_LIB)
    #if defined(NDEBUG)
        #pragma comment(lib, "ttLibwin.lib")
    #else
        #pragma comment(lib, "ttLibwinD.lib")
    #endif
#endif

#include "../include/ttlibwin.h"

#include "../include/ttstr.h"       // ttCStr
#include "../include/ttdebug.h"     // ttASSERT macros
#include "../include/ttshadebtn.h"  // ttCShadeBtn

/*
    This class only works on non-image buttons. I.e., this class will not work on a button that is drawn with a
    bitmap. Changing from MFC/WTL to ttWin also removed the check for a bitmap button. We should probably do some
    kind of check for that condition and fail for bitmap buttons. While the review comment is here, the more
    important check will need to be in ttMultiBtn -- since that's where all the button subclassing is done.
*/

ttCShadeBtn::ttCShadeBtn()
{
    m_Border = TRUE;                           // draw 3D border
    m_FocusRectMargin = 4;                     // focus dotted rect margin
    m_TextColor = GetSysColor(COLOR_BTNTEXT);  // default button text color
    m_flat = m_Checked = false;

    m_hIcon = m_hIconDown = m_hIconHighLight = NULL;       // icon handle
    SetRectEmpty(&m_rcIconBox);                            // icon location
    m_hIconAlign = BS_CENTER;                              // icon alignment
    m_TextAlign = DT_SINGLELINE | DT_CENTER | DT_VCENTER;  // text alignment
    m_pLF = nullptr;                                       // font structure pointer

    // [randalphwa - 09-17-2018] The original code would default to using DEFAULT_GUI_FONT -- which is obsolete.
    // "MS Shell Dlg" is typically a MS Sans Serif font, and "MS Shell Dlg 2" is typically a Tahoma font.
    // "MS Shell Dlg" provides compatibility with pre-Windows 2000 systems.

    // Forcing the font size to 8 may cause display problems with asian languages. However, setting it to
    // 0 may cause it to be much larger then originally intended for the button. Ultimately, if this
    // default font is unsatisfactory, the caller should call ttCShadeBtn::SetFont().

    m_hFont = ttCreateLogFont("MS Shell Dlg", 8);

    // Another option is to get the font for Message Boxes and use that. However, it's quite possible that
    // the user changed that font specifically for message boxes without expecting it to also change the font
    // on dialog buttons, and you might end up with an unreadable button as a result.

#if 0
    NONCLIENTMETRICS* pmetrics = (NONCLIENTMETRICS*) ttcalloc(sizeof(NONCLIENTMETRICS));
    pmetrics->cbSize = sizeof(NONCLIENTMETRICS);
    if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, pmetrics, 0))
        SetFont(&pmetrics->lfMessageFont);
    ttFree(pmetrics);
#endif
}

ttCShadeBtn::~ttCShadeBtn()
{
    if (m_hFont)
        DeleteObject(m_hFont);
    if (m_pLF)
        ttFree(m_pLF);
    if (m_hIconDown != m_hIcon && m_hIconDown)
        DestroyIcon(m_hIconDown);
    if (m_hIconHighLight != m_hIcon && m_hIconHighLight)
        DestroyIcon(m_hIconHighLight);
    if (m_hIcon)
        DestroyIcon(m_hIcon);
    if (IsWindow(*this))
        DestroyWindow(*this);
    m_hwnd = NULL;
}

bool ttCShadeBtn::SetFont(LOGFONTA* pNewStyle)
{
    if (pNewStyle)
    {
        if (m_pLF == NULL)
            m_pLF = (LOGFONTA*) ttCalloc(sizeof(LOGFONTA));
        if (m_pLF)
        {
            memcpy(m_pLF, pNewStyle, sizeof(LOGFONTA));
            if (m_hFont)
                DeleteObject(m_hFont);
            m_hFont = CreateFontIndirectA(m_pLF);
            return (m_hFont != NULL);
        }
    }
    return false;
}

bool ttCShadeBtn::SetFont(const char* pszFontName, long lSize, long lWeight, BYTE bItalic, BYTE bUnderline)
{
    if (m_pLF == NULL)
        m_pLF = (LOGFONTA*) ttCalloc(sizeof(LOGFONTA));
    if (m_pLF)
    {
        ttStrCpy(m_pLF->lfFaceName, sizeof(m_pLF->lfFaceName), pszFontName);
        m_pLF->lfHeight = lSize;
        m_pLF->lfWeight = lWeight;
        m_pLF->lfItalic = bItalic;
        m_pLF->lfUnderline = bUnderline;
        if (m_hFont)
            DeleteObject(m_hFont);
        m_hFont = CreateFontIndirectA(m_pLF);
        return (m_hFont != NULL);
    }
    return false;
}

void ttCShadeBtn::SetButtonStyle(UINT nStyle, BOOL bRedraw)
{
    m_IsPushLike = ((nStyle & BS_PUSHLIKE) != 0);
    m_flat = ((nStyle & BS_FLAT) != 0);

    if ((nStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
        //      ||((m_Style & BS_CHECKBOX)==BS_CHECKBOX))
        m_Style = BS_CHECKBOX;
    else if ((nStyle & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
        //          ||((m_Style & BS_RADIOBUTTON)==BS_RADIOBUTTON))
        m_Style = BS_RADIOBUTTON;
    else
    {
        m_Style = BS_PUSHBUTTON;
        m_IsPushLike = true;
        m_Checked = false;
    }

    // default radio & check-box has no border
    if (!m_IsPushLike)
        m_Border = false;

    if (bRedraw)
        InvalidateRect(*this, NULL,
                       TRUE);  // REVIEW: [randalphwa - 1/26/2019] Can we get away with setting FALSE for bErase?
}

void ttCShadeBtn::SetTextAlign(UINT nTextAlign)
{
    //  see DrawText() styles...
    switch (nTextAlign)
    {
        case BS_RIGHT:
            m_TextAlign = DT_RIGHT;
            break;

        case BS_LEFT:
            m_TextAlign = DT_LEFT;
            break;

        default:
            m_TextAlign = DT_CENTER;
    }
    m_TextAlign |= (DT_SINGLELINE | DT_VCENTER);
}

void ttCShadeBtn::SetIcon(HICON hIcon, UINT nIconAlign, UINT nIconDown, UINT nIconHighLight)
{
    if (hIcon)
    {
        if (m_hIconDown != m_hIcon && m_hIconDown)
            DestroyIcon(m_hIconDown);
        if (m_hIconHighLight != m_hIcon && m_hIconHighLight)
            DestroyIcon(m_hIconHighLight);
        if (m_hIcon)
            DestroyIcon(m_hIcon);

        ICONINFO iinfo;  // get icon info
        m_hIcon = hIcon;
        GetIconInfo(m_hIcon, &iinfo);
        m_rcIconBox.left = m_rcIconBox.top = 0;
        m_rcIconBox.right = iinfo.xHotspot * 2;
        m_rcIconBox.bottom = iinfo.yHotspot * 2;

        long x, y;
        RECT rect;
        GetWindowRect(*this, &rect);
        y = abs(rect.bottom - rect.top);
        x = abs(rect.right - rect.left);
        switch (nIconAlign)  // set the icon location
        {
            case BS_RIGHT:
                m_hIconAlign = BS_RIGHT;
                OffsetRect(&m_rcIconBox, x - iinfo.xHotspot * 2 - m_FocusRectMargin,
                           max(0, (long) (y / 2 - iinfo.yHotspot)));
                break;
            case BS_LEFT:
                m_hIconAlign = BS_LEFT;
                OffsetRect(&m_rcIconBox, m_FocusRectMargin + 8, max(0, (long) (y / 2 - iinfo.yHotspot)));
                break;
            default:
                m_hIconAlign = BS_CENTER;
                OffsetRect(&m_rcIconBox, max(0, (long) (x / 2 - iinfo.xHotspot)),
                           max(0, (long) (y / 2 - 2 * iinfo.yHotspot)));
        }

        // release icon mask bitmaps (Orioli Alessandro <aorioli@temaweb.it>)
        if (iinfo.hbmColor)
            DeleteObject(iinfo.hbmColor);
        if (iinfo.hbmMask)
            DeleteObject(iinfo.hbmMask);

        if (nIconDown > 0)  // load down icon
        {
            m_hIconDown =
                (HICON)::LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCEA(nIconDown), IMAGE_ICON, 0, 0, 0);
            if (m_hIconDown == NULL)
                m_hIconDown = m_hIcon;
        }
        else
        {
            m_hIconDown = m_hIcon;  // reuse resource handle
        }

        if (nIconHighLight > 0)  // load highlighted icon
        {
            m_hIconHighLight =
                (HICON)::LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCEA(nIconHighLight), IMAGE_ICON, 0, 0, 0);
            if (m_hIconHighLight == NULL)
                m_hIconHighLight = m_hIcon;
        }
        else
        {
            m_hIconHighLight = m_hIcon;  // reuse resource handle
        }
    }
}

void ttCShadeBtn::SetIcon(UINT nIcon, UINT nIconAlign, UINT nIconDown, UINT nIconHighLight)
{
    if (m_hIconDown != m_hIcon && m_hIconDown)
        DestroyIcon(m_hIconDown);
    if (m_hIconHighLight != m_hIcon && m_hIconHighLight)
        DestroyIcon(m_hIconHighLight);
    if (m_hIcon)
        DestroyIcon(m_hIcon);

    HICON hIcon = (HICON)::LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(nIcon), IMAGE_ICON, 0, 0, 0);
    ttASSERT_MSG(hIcon, "Unable to load icon");
    if (hIcon)
        SetIcon(hIcon, nIconAlign, nIconDown, nIconHighLight);
}

void ttCShadeBtn::SetIcon(const char* pszIconName, UINT nIconAlign, UINT nIconDown, UINT nIconHighLight)
{
    if (m_hIconDown != m_hIcon && m_hIconDown)
        DestroyIcon(m_hIconDown);
    if (m_hIconHighLight != m_hIcon && m_hIconHighLight)
        DestroyIcon(m_hIconHighLight);
    if (m_hIcon)
        DestroyIcon(m_hIcon);

    HICON hIcon = (HICON)::LoadImageA(GetModuleHandle(NULL), pszIconName, IMAGE_ICON, 0, 0, 0);
    ttASSERT_MSG(hIcon, "Unable to load icon");
    if (hIcon)
        SetIcon(hIcon, nIconAlign, nIconDown, nIconHighLight);
}

void ttCShadeBtn::SetShade(BTN_SHADE shadeID, BYTE granularity, BYTE highlight, BYTE coloring, COLORREF color)
{
    long sXSize, sYSize, bytes, j, i, k, h;
    BYTE *iDst, *posDst;

    RECT rect;
    GetWindowRect(*this, &rect);
    sYSize = abs(rect.bottom - rect.top);
    sXSize = abs(rect.right - rect.left);

    m_dh.Create(max(1, sXSize - 2 * m_FocusRectMargin - 1), 1, 8);  // create the horizontal focus bitmap
    m_dv.Create(1, max(1, sYSize - 2 * m_FocusRectMargin), 8);      // create the vertical focus bitmap

    m_dNormal.Create(sXSize, sYSize, 8);  // create the default bitmap

    COLORREF hicr = GetSysColor(COLOR_BTNHIGHLIGHT);  // get the button base colors
    COLORREF midcr = GetSysColor(COLOR_BTNFACE);
    COLORREF locr = GetSysColor(COLOR_BTNSHADOW);
    long r, g, b;  // build the shaded palette
    for (i = 0; i < 129; i++)
    {
        r = ((128 - i) * GetRValue(locr & 0xFF) + i * GetRValue(midcr & 0xFF)) / 128;
        g = ((128 - i) * GetGValue(locr & 0xFFFF) + i * GetGValue(midcr & 0xFFFF)) / 128;
        b = ((128 - i) * GetBValue(locr & 0xFFFFFF) + i * GetBValue(midcr & 0xFFFFFF)) / 128;
        m_dNormal.SetPaletteIndex((BYTE) i, (BYTE) r, (BYTE) g, (BYTE) b);
        m_dh.SetPaletteIndex((BYTE) i, (BYTE) r, (BYTE) g, (BYTE) b);
        m_dv.SetPaletteIndex((BYTE) i, (BYTE) r, (BYTE) g, (BYTE) b);
    }
    for (i = 1; i < 129; i++)
    {
        r = ((128 - i) * GetRValue(midcr & 0xFF) + i * GetRValue(hicr & 0xFF)) / 128;
        g = ((128 - i) * GetGValue(midcr & 0xFFFF) + i * GetGValue(hicr & 0xFFFF)) / 128;
        b = ((128 - i) * GetBValue(midcr & 0xFFFFFF) + i * GetBValue(hicr & 0xFFFFFF)) / 128;
        m_dNormal.SetPaletteIndex((BYTE)(i + 127), (BYTE) r, (BYTE) g, (BYTE) b);
        m_dh.SetPaletteIndex((BYTE)(i + 127), (BYTE) r, (BYTE) g, (BYTE) b);
        m_dv.SetPaletteIndex((BYTE)(i + 127), (BYTE) r, (BYTE) g, (BYTE) b);
    }

    m_dNormal.BlendPalette(color, coloring);  // color the palette

    iDst = m_dh.GetBits();  // build the horiz. dotted focus bitmap
    j = (long) m_dh.GetWidth();
    for (i = 0; i < j; i++)
    {
        //      iDst[i]=64+127*(i%2);   //soft
        iDst[i] = (BYTE)(255 * (i % 2));  // hard
    }

    iDst = m_dv.GetBits();  // build the vert. dotted focus bitmap
    j = (long) m_dv.GetHeight();
    for (i = 0; i < j; i++)
    {
        //      *iDst=64+127*(i%2);     //soft
        *iDst = (BYTE)(255 * (i % 2));  // hard
        iDst += 4;
    }

    bytes = m_dNormal.GetLineWidth();
    iDst = m_dNormal.GetBits();
    posDst = iDst;
    long a, x, y, d, xs, idxmax, idxmin;

    int grainx2 = RAND_MAX / (max(1, 2 * granularity));
    idxmax = 255 - granularity;
    idxmin = granularity;

    // REVIEW: [ralphw - 06-29-2003] use enumerated type for the shades, not numbers

    switch (shadeID)
    {
            //----------------------------------------------------
        case SHS_METAL:  // SHS_METAL
            m_dNormal.Clear();
            // create the strokes
            k = 40;  // stroke granularity
            for (a = 0; a < 200; a++)
            {
                x = rand() / (RAND_MAX / sXSize);                    // stroke postion
                y = rand() / (RAND_MAX / sYSize);                    // stroke position
                xs = rand() / (RAND_MAX / min(sXSize, sYSize)) / 2;  // stroke lenght
                d = rand() / (RAND_MAX / k);                         // stroke color
                for (i = 0; i < xs; i++)
                {
                    if (((x - i) > 0) && ((y + i) < sYSize))
                        m_dNormal.SetPixelIndex(x - i, y + i, (BYTE) d);
                    if (((x + i) < sXSize) && ((y - i) > 0))
                        m_dNormal.SetPixelIndex(sXSize - x + i, y - i, (BYTE) d);
                }
            }
            // blend strokes with SHS_DIAGONAL
            posDst = iDst;
            a = (idxmax - idxmin - k) / 2;
            for (i = 0; i < sYSize; i++)
            {
                for (j = 0; j < sXSize; j++)
                {
                    d = posDst[j] + ((a * i) / sYSize + (a * (sXSize - j)) / sXSize);
                    posDst[j] = (BYTE) d;
                    posDst[j] = (posDst[j] + (rand() / grainx2)) & 0xFF;
                }
                posDst += bytes;
            }

            break;
            //----------------------------------------------------
        case SHS_HARDBUMP:  // SHS_HARDBUMP
            // set horizontal bump
            for (i = 0; i < sYSize; i++)
            {
                k = (255 * i / sYSize) - 127;
                k = (k * (k * k) / 128) / 128;
                k = (k * (128 - granularity * 2)) / 128 + 128;
                for (j = 0; j < sXSize; j++)
                {
                    posDst[j] = (BYTE) k;
                    posDst[j] = (posDst[j] + (rand() / grainx2 - granularity)) & 0xFF;
                }
                posDst += bytes;
            }
            // set vertical bump
            d = min(16, sXSize / 6);  // max edge=16
            a = sYSize * sYSize / 4;
            posDst = iDst;
            for (i = 0; i < sYSize; i++)
            {
                y = i - sYSize / 2;
                for (j = 0; j < sXSize; j++)
                {
                    x = j - sXSize / 2;
                    xs = sXSize / 2 - d + (y * y * d) / a;
                    if (x > xs)
                        posDst[j] = (BYTE)(idxmin + (BYTE)(((sXSize - j) * 128) / d));
                    if ((x + xs) < 0)
                        posDst[j] = (BYTE)(idxmax - (BYTE)((j * 128) / d));
                    posDst[j] = (posDst[j] + (rand() / grainx2 - granularity)) & 0xFF;
                }
                posDst += bytes;
            }
            break;
            //----------------------------------------------------
        case SHS_SOFTBUMP:  // SHS_SOFTBUMP
            for (i = 0; i < sYSize; i++)
            {
                h = (255 * i / sYSize) - 127;
                for (j = 0; j < sXSize; j++)
                {
                    k = (255 * (sXSize - j) / sXSize) - 127;
                    k = (h * (h * h) / 128) / 128 + (k * (k * k) / 128) / 128;
                    k = k * (128 - granularity) / 128 + 128;
                    if (k < idxmin)
                        k = idxmin;
                    if (k > idxmax)
                        k = idxmax;
                    posDst[j] = (BYTE) k;
                    posDst[j] = (posDst[j] + (rand() / grainx2 - granularity)) & 0xFF;
                }
                posDst += bytes;
            }
            break;
            //----------------------------------------------------
        case SHS_VBUMP:  // SHS_VBUMP
            for (j = 0; j < sXSize; j++)
            {
                k = (255 * (sXSize - j) / sXSize) - 127;
                k = (k * (k * k) / 128) / 128;
                k = (k * (128 - granularity)) / 128 + 128;
                for (i = 0; i < sYSize; i++)
                {
                    posDst[j + i * bytes] = (BYTE) k;
                    posDst[j + i * bytes] = (posDst[j + i * bytes] + (rand() / grainx2 - granularity)) & 0xFF;
                }
            }
            break;
            //----------------------------------------------------
        case SHS_HBUMP:  // SHS_HBUMP
            for (i = 0; i < sYSize; i++)
            {
                k = (255 * i / sYSize) - 127;
                k = (k * (k * k) / 128) / 128;
                k = (k * (128 - granularity)) / 128 + 128;
                for (j = 0; j < sXSize; j++)
                {
                    posDst[j] = (BYTE) k;
                    posDst[j] = (posDst[j] + (rand() / grainx2 - granularity)) & 0xFF;
                }
                posDst += bytes;
            }
            break;
            //----------------------------------------------------
        case SHS_DIAGSHADE:  // SHS_DIAGSHADE
            a = (idxmax - idxmin) / 2;
            for (i = 0; i < sYSize; i++)
            {
                for (j = 0; j < sXSize; j++)
                {
                    posDst[j] = (BYTE)(idxmin + a * i / sYSize + a * (sXSize - j) / sXSize);
                    posDst[j] = (posDst[j] + (rand() / grainx2 - granularity)) & 0xFF;
                }
                posDst += bytes;
            }
            break;
            //----------------------------------------------------
        case SHS_HSHADE:  // SHS_HSHADE
            a = idxmax - idxmin;
            for (i = 0; i < sYSize; i++)
            {
                k = a * i / sYSize + idxmin;
                for (j = 0; j < sXSize; j++)
                {
                    posDst[j] = (BYTE) k;
                    posDst[j] = (posDst[j] + (rand() / grainx2 - granularity)) & 0xFF;
                }
                posDst += bytes;
            }
            break;
            //----------------------------------------------------
        case SHS_VSHADE:  // SHS_VSHADE:
            a = idxmax - idxmin;
            for (j = 0; j < sXSize; j++)
            {
                k = a * (sXSize - j) / sXSize + idxmin;
                for (i = 0; i < sYSize; i++)
                {
                    posDst[j + i * bytes] = (BYTE) k;
                    posDst[j + i * bytes] = (posDst[j + i * bytes] + (rand() / grainx2 - granularity)) & 0xFF;
                }
            }
            break;
            //----------------------------------------------------
        case SHS_NOISE:  // SHS_NOISE
            for (i = 0; i < sYSize; i++)
            {
                for (j = 0; j < sXSize; j++)
                    posDst[j] = (128 + rand() / grainx2 - granularity) & 0xFF;
                posDst += bytes;
            }
            break;
    }

    m_dDisabled.Clone(&m_dNormal);  // build the other bitmaps
    m_dOver.Clone(&m_dNormal);
    m_dOver.BlendPalette(hicr, highlight);
    m_dDown.Clone(&m_dOver);
}

COLORREF ttCShadeBtn::SetTextColor(COLORREF new_color)
{
    COLORREF tmp_color = m_TextColor;
    m_TextColor = new_color;
    return tmp_color;  // returns the previous color
}

void ttCShadeBtn::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdcPaint = BeginPaint(*this, &ps);

    ttCStr cszCaption;
    RECT rcClient;
    GetClientRect(*this, &rcClient);

    int cx = abs(rcClient.right - rcClient.left);
    int cy = abs(rcClient.bottom - rcClient.top);
    // get text box position
    RECT tr = { rcClient.left + m_FocusRectMargin + 2, rcClient.top, rcClient.right - m_FocusRectMargin - 2,
                rcClient.bottom };

    HDC hdcMem;  // create a memory DC to avoid flicker
    hdcMem = CreateCompatibleDC(hdcPaint);
    HANDLE hBitmap = CreateCompatibleBitmap(hdcPaint, cx, cy);
    HBITMAP hOldBitmap = (HBITMAP) SelectObject(hdcMem, hBitmap);  // select the destination for MemDC

    cszCaption.GetWndText(*this);  // get button text
    SetBkMode(hdcMem, TRANSPARENT);
    // with MemDC we need to select the font...

    // get text font
    HFONT hOldFont = NULL;
    if (m_hFont)
        hOldFont = (HFONT) SelectObject(hdcMem, m_hFont);
    else
    {
        // [randalphwa - 09-17-2018] This should never be called -- m_hFont is created in the destructor.
        // However if the caller tried to create an invalid font, then m_hFont will be null. This should be
        // rare enough that it's probably not worth recreating our default font that the caller deleted when
        // they tried to create their own font.

        SelectObject(hdcMem, GetStockObject(DEFAULT_GUI_FONT));
    }

    // accommodate text location
    if (m_hIcon)
    {
        switch (m_hIconAlign)
        {
            case BS_LEFT:
                tr.left += abs(m_rcIconBox.right - m_rcIconBox.left);  // shift left
                break;
            case BS_RIGHT:
                tr.right -= abs(m_rcIconBox.right - m_rcIconBox.left);  // shift right
                break;
            case BS_CENTER:
                tr.top += m_rcIconBox.bottom;  // shift text under the icon
                tr.bottom -= m_FocusRectMargin + 1;
        }
    }
    // Select the correct skin
    if (!IsWindowEnabled(*this))  // DISABLED BUTTON
    {
        // if (GetStyle() & WS_DISABLED) {
        if (m_dDisabled.IsValid())  // paint the skin
            m_dDisabled.Draw(hdcMem, 0, 0);
        else  // no skin selected for disabled state -> standard button
            ::FillRect(hdcMem, &rcClient, (HBRUSH)(ULONG_PTR) GetSysColor(COLOR_BTNFACE));

        if (m_hIcon)  // draw the icon
        {
            ::DrawState(hdcMem, NULL, NULL, (LPARAM) m_hIcon, NULL, m_rcIconBox.left, m_rcIconBox.top,
                        abs(m_rcIconBox.right - m_rcIconBox.left), abs(m_rcIconBox.bottom - m_rcIconBox.top),
                        DST_ICON | DSS_DISABLED);
        }
        // if needed, draw the standard 3D rectangular border
        if ((m_Border) && (m_flat == FALSE))
            DrawEdge(hdcMem, &rcClient, EDGE_RAISED, BF_RECT);
        // paint the etched button text
        ::SetTextColor(hdcMem, GetSysColor(COLOR_3DHILIGHT));
        ::DrawTextA(hdcMem, (const char*) cszCaption, -1, &tr, m_TextAlign);
        ::SetTextColor(hdcMem, GetSysColor(COLOR_GRAYTEXT));
        OffsetRect(&tr, -1, -1);
        ::DrawTextA(hdcMem, (const char*) cszCaption, -1, &tr, m_TextAlign);
    }
    else
    {
        if (SendMessage(BM_GETSTATE) & BST_PUSHED)  // SELECTED (DOWN) BUTTON
        {
            if (m_dDown.IsValid())  // paint the skin
                m_dDown.Draw(hdcMem, m_Border, m_Border);
            else  // no skin selected for selected state -> standard button
                ::FillRect(hdcMem, &rcClient, (HBRUSH)(ULONG_PTR) GetSysColor(COLOR_BTNFACE));

            if (m_hIconDown)  // draw the pushed icon
            {
                if (m_IsPushLike)
                    OffsetRect(&m_rcIconBox, 1, 1);
                ::DrawState(hdcMem, NULL, NULL, (LPARAM) m_hIcon, NULL, m_rcIconBox.left, m_rcIconBox.top,
                            abs(m_rcIconBox.right - m_rcIconBox.left), abs(m_rcIconBox.bottom - m_rcIconBox.top),
                            DST_ICON | DSS_NORMAL);
                if (m_IsPushLike)
                    OffsetRect(&m_rcIconBox, -1, -1);
            }
            // shift text
            if (m_IsPushLike)
                OffsetRect(&tr, 1, 1);
            // if needed, draw the standard 3D rectangular border
            if (m_Border)
            {
                if (m_flat)
                    Draw3dRect(hdcMem, &rcClient, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
                else
                    ::DrawEdge(hdcMem, &rcClient, EDGE_SUNKEN, BF_RECT);
            }
        }
        else
        {
            if (m_dNormal.IsValid())  // paint the skin    // DEFAULT BUTTON
                m_dNormal.Draw(hdcMem, 0, 0);
            else  // no skin selected for normal state -> standard button
                ::FillRect(hdcMem, &rcClient, (HBRUSH)(ULONG_PTR) GetSysColor(COLOR_BTNFACE));

            if (m_hIcon)  // draw the icon
                ::DrawState(hdcMem, NULL, NULL, (LPARAM) m_hIcon, NULL, m_rcIconBox.left, m_rcIconBox.top,
                            abs(m_rcIconBox.right - m_rcIconBox.left), abs(m_rcIconBox.bottom - m_rcIconBox.top),
                            DST_ICON | DSS_NORMAL);
            // if needed, draw the standard 3D rectangular border
            if (m_Border && (m_flat == FALSE))
            {
                if (m_Style & BS_DEFPUSHBUTTON)
                {
                    DrawEdge(hdcMem, &rcClient, EDGE_SUNKEN, BF_RECT);
                    InflateRect(&rcClient, -1, -1);
                    DrawEdge(hdcMem, &rcClient, EDGE_RAISED, BF_RECT);
                }
                else
                {
                    if (m_flat)
                        Draw3dRect(hdcMem, &rcClient, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
                    else
                        DrawEdge(hdcMem, &rcClient, EDGE_RAISED, BF_RECT);
                }
            }
        }
        // paint the focus rect
        if ((SendMessage(BM_GETSTATE) & BST_FOCUS) && m_FocusRectMargin > 0)
        {
            InflateRect(&rcClient, -m_FocusRectMargin, -m_FocusRectMargin);
            m_dh.Draw(hdcMem, 1 + rcClient.left, rcClient.top);
            m_dh.Draw(hdcMem, 1 + rcClient.left, rcClient.bottom);
            m_dv.Draw(hdcMem, rcClient.left, 1 + rcClient.top);
            m_dv.Draw(hdcMem, rcClient.right, 1 + rcClient.top);
        }
        // paint the enabled button text
        ::SetTextColor(hdcMem, m_TextColor);
        ::DrawTextA(hdcMem, (const char*) cszCaption, -1, &tr, m_TextAlign);
    }

    if (hOldFont)
        SelectObject(hdcMem, hOldFont);  // restore the old font object

    BitBlt(hdcPaint, 0, 0, cx, cy, hdcMem, 0, 0, SRCCOPY);  // copy in the real world
    // restore the old objects to avoid memory leakage <David Scambler>
    if (hOldBitmap)
        SelectObject(hdcMem, hOldBitmap);

    DeleteDC(hdcMem);
    DeleteObject(hBitmap);
    EndPaint(*this, &ps);
}

void ttCShadeBtn::Draw3dRect(HDC hdc, RECT* prc, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
    int x = prc->left;
    int y = prc->top;
    int cx = abs(prc->right - prc->left);
    int cy = abs(prc->bottom - prc->top);
    Draw3dRect(hdc, x, y, cx, cy, clrTopLeft, clrBottomRight);
}

void ttCShadeBtn::Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
    FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
    FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
    FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
    FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void ttCShadeBtn::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
{
    RECT rect = { x, y, x + cx, y + cy };
    ::FillRect(hdc, &rect, (HBRUSH)(ULONG_PTR) clr);
}
