/////////////////////////////////////////////////////////////////////////////
// Name:      ttdib.h
// Purpose:   Class for handling Device Independent Bitmap
// License:   CPOL License (see ../license_code_project.htm )
/////////////////////////////////////////////////////////////////////////////

//////////////// Derivative work ////////////////////////////////////////////
// Changes:   Derived from CxDib and CxShadeButton, changed to remove MFC requirement
// Author:    Ralph Walden
// Copyright: Copyright (c) 2001-2020 KeyWorks Software (Ralph Walden)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <cstddef>

#include <wtypes.h>

class ttCDib
{
public:
    ttCDib();
    ~ttCDib();

    // Class functions

    void BlendPalette(COLORREF cr, long perc);
    void Clear(BYTE bval = 0);
    void Clone(ttCDib* src);
    void* Create(DWORD dwWidth, DWORD dwHeight, WORD wBitCount);
    long Draw(HDC pDC, long xoffset, long yoffset);
    WORD GetBitCount() { return m_bi.biBitCount; };
    BYTE* GetBits();
    DWORD GetHeight() { return m_bi.biHeight; };
    DWORD GetLineWidth() { return m_LineWidth; };
    BYTE GetNearestIndex(RGBQUAD c);
    WORD GetNumColors() { return m_nColors; };
    RGBQUAD GetPaletteIndex(BYTE idx);
    WORD GetPaletteSize();
    RGBQUAD GetPixelColor(long x, long y);
    BYTE GetPixelIndex(long x, long y);
    long GetSize();
    DWORD GetWidth() { return m_bi.biWidth; };
    RGBQUAD HSLtoRGB(COLORREF cHSLColor);
    RGBQUAD HSLtoRGB(RGBQUAD lHSLColor);
    bool IsValid() { return (m_dib); }
    RGBQUAD RGB2RGBQUAD(COLORREF cr);
    COLORREF RGBQUAD2RGB(RGBQUAD c);
    RGBQUAD RGBtoHSL(RGBQUAD lRGBColor);
    void SetGrayPalette();
    void SetPaletteIndex(BYTE idx, BYTE r, BYTE g, BYTE b);
    void SetPaletteIndex(BYTE idx, COLORREF cr);
    void SetPaletteIndex(BYTE idx, RGBQUAD c);
    void SetPixelColor(long x, long y, COLORREF cr);
    void SetPixelColor(long x, long y, RGBQUAD c);
    void SetPixelIndex(long x, long y, BYTE i);
    long Stretch(HDC pDC, long xoffset, long yoffset, long xsize, long ysize);

protected:
    WORD HueToRGB(WORD n1, WORD n2, WORD hue);

private:
    // Class members

    std::byte* m_dib { nullptr };
    BITMAPINFOHEADER m_bi;
    DWORD m_LineWidth;
    WORD m_nColors;
};
