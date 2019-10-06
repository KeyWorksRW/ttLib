/////////////////////////////////////////////////////////////////////////////
// Name:      ttCDib
// Purpose:   Class for handling Device Independent Bitmap
// Author:    Ralph Walden
// Copyright: Copyright (c) 2001-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

//////////////// Derivative work ////////////////////////////////////////////
// Changes:     Derived from CxDib and CxShadeButton, changed to remove MFC requirement
// Author:      Ralph Walden
// Copyright:   Copyright (c) 2001-2019 KeyWorks Software (Ralph Walden)
// Notes:       The above Code Project License also applies to the derivative work
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(_WIN32)

    #include <wtypes.h>

// REVIEW: [randalphwa - 1/10/2019] This is a bug waiting to happen. Handles in Windows are FreeAllocd with
// DeleteObject(). HDIB looks like a GDI Object, and if it really was, it would be handed to DeleteObject(). A HANDLE
// in Windows is a pointer, but you do NOT use standard memory routines to allocate/FreeAlloc it the way HDIB is being
// used here. The HDIB should be replaced with a void* since it gets cast to different structure pointers.

    #define HDIB HANDLE

class ttCDib
{
public:
    ttCDib();
    ~ttCDib();

    // Class functions

    void     BlendPalette(COLORREF cr, long perc);
    void     Clear(BYTE bval = 0);
    void     Clone(ttCDib* src);
    HDIB     Create(DWORD dwWidth, DWORD dwHeight, WORD wBitCount);
    long     Draw(HDC pDC, long xoffset, long yoffset);
    WORD     GetBitCount() { return m_bi.biBitCount; };
    BYTE*    GetBits();
    DWORD    GetHeight() { return m_bi.biHeight; };
    DWORD    GetLineWidth() { return m_LineWidth; };
    BYTE     GetNearestIndex(RGBQUAD c);
    WORD     GetNumColors() { return m_nColors; };
    RGBQUAD  GetPaletteIndex(BYTE idx);
    WORD     GetPaletteSize();
    RGBQUAD  GetPixelColor(long x, long y);
    BYTE     GetPixelIndex(long x, long y);
    long     GetSize();
    DWORD    GetWidth() { return m_bi.biWidth; };
    RGBQUAD  HSLtoRGB(COLORREF cHSLColor);
    RGBQUAD  HSLtoRGB(RGBQUAD lHSLColor);
    bool     IsValid() { return (hDib != NULL); }
    RGBQUAD  RGB2RGBQUAD(COLORREF cr);
    COLORREF RGBQUAD2RGB(RGBQUAD c);
    RGBQUAD  RGBtoHSL(RGBQUAD lRGBColor);
    void     SetGrayPalette();
    void     SetPaletteIndex(BYTE idx, BYTE r, BYTE g, BYTE b);
    void     SetPaletteIndex(BYTE idx, COLORREF cr);
    void     SetPaletteIndex(BYTE idx, RGBQUAD c);
    void     SetPixelColor(long x, long y, COLORREF cr);
    void     SetPixelColor(long x, long y, RGBQUAD c);
    void     SetPixelIndex(long x, long y, BYTE i);
    long     Stretch(HDC pDC, long xoffset, long yoffset, long xsize, long ysize);

protected:
    bool IsWin30Dib();
    WORD HueToRGB(WORD n1, WORD n2, WORD hue);

    // Class members

    HDIB             hDib;
    BITMAPINFOHEADER m_bi;
    DWORD            m_LineWidth;
    WORD             m_nColors;
};

#endif  // defined(_WIN32)
