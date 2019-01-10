/////////////////////////////////////////////////////////////////////////////
// Name:		ttDib
// Purpose:		Class for handling Device Independent Bitmap
// Author:		Davide Pizzolato
// Copyright:   Copyright (c) [2001] Davide Pizzolato
// Licence:     The Code Project Open License (see ../CPOL.md)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include "../include/ttdib.h"

#ifndef	WIDTHBYTES
	#define WIDTHBYTES(bits)	(((bits) + 31) / 32 * 4)
#endif

#if 0	// [randalphwa - 1/10/2019] These are presumably used to indentify type, but nothing uses them
	#define BFT_ICON   0x4349	// 'IC'
	#define BFT_BITMAP 0x4d42	// 'BM'
	#define BFT_CURSOR 0x5450	// 'PT'
#endif

#define IS_WIN30_DIB(lpbi)	((*(LPDWORD)(lpbi))==sizeof(BITMAPINFOHEADER))

ttDib::ttDib()
{
	hDib = NULL;

	memset(&m_bi, 0, sizeof(BITMAPINFOHEADER));
	m_bi.biSize = sizeof(BITMAPINFOHEADER);

	m_LineWidth = 0;
	m_nColors = 0;
}

ttDib::~ttDib()
{
	if (hDib)
		tt::free(hDib);
}

bool ttDib::IsWin30Dib() {
	return ((*(LPDWORD)(hDib)) == sizeof(BITMAPINFOHEADER));
}

WORD ttDib::GetPaletteSize()
{
//	  if (IsWin30Dib())
		return (m_nColors * sizeof(RGBQUAD));
//	  else return (m_nColors * sizeof(RGBTRIPLE));
}

BYTE* ttDib::GetBits()
{
	if (hDib)
		return ((BYTE*) hDib + *(LPDWORD) hDib + GetPaletteSize());
	return nullptr;
}

HDIB  ttDib::Create(DWORD dwWidth, DWORD dwHeight, WORD wBitCount)
{
	LPBITMAPINFOHEADER	lpbi;	// pointer to BITMAPINFOHEADER
	DWORD				dwLen;	// size of memory block

	if (hDib)
		tt::free(hDib);
	hDib = nullptr;

	// Make sure bits per pixel is valid
	if (wBitCount <= 1)
		wBitCount = 1;
	else if (wBitCount <= 4)
		wBitCount = 4;
	else if (wBitCount <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	switch (wBitCount){
		case 1:
			m_nColors = 2;
			break;
		case 4:
			m_nColors = 16;
			break;
		case 8:
			m_nColors = 256;
			break;
		default:
			m_nColors = 0;
	}

	m_LineWidth = WIDTHBYTES(wBitCount * dwWidth);

	// initialize BITMAPINFOHEADER
	m_bi.biSize = sizeof(BITMAPINFOHEADER);
	m_bi.biWidth = dwWidth;			// fill in width from parameter
	m_bi.biHeight = dwHeight;		// fill in height from parameter
	m_bi.biPlanes = 1;				// must be 1
	m_bi.biBitCount = wBitCount;	// from parameter
	m_bi.biCompression = BI_RGB;
	m_bi.biSizeImage = m_LineWidth * dwHeight;
	m_bi.biXPelsPerMeter = 0;
	m_bi.biYPelsPerMeter = 0;
	m_bi.biClrUsed = 0;
	m_bi.biClrImportant = 0;

	// calculate size of memory block required to store the DIB.  This
	// block should be big enough to hold the BITMAPINFOHEADER, the color
	// table, and the bits
	dwLen = GetSize();

	hDib = tt::malloc(dwLen); // alloc memory block to store our bitmap
	if (!hDib)
		return nullptr;

	// use our bitmap info structure to fill in first part of
	// our DIB with the BITMAPINFOHEADER
	lpbi = (LPBITMAPINFOHEADER)(hDib);
	*lpbi = m_bi;

	return hDib; //return handle to the DIB
}

long ttDib::Draw(HDC pDC, long xoffset, long yoffset)
{
	if ((hDib) && (pDC))  {
		//palette must be correctly filled
		char* lpDIB = (char*)hDib;	//set image to hdc...
		SetStretchBltMode(pDC,COLORONCOLOR);
		SetDIBitsToDevice(pDC, xoffset, yoffset, m_bi.biWidth, m_bi.biHeight, 0, 0, 0,
			m_bi.biHeight, GetBits(), (BITMAPINFO*)lpDIB, DIB_RGB_COLORS);
		return 1;
	}
	return 0;
}

long ttDib::Stretch(HDC pDC, long xoffset, long yoffset, long xsize, long ysize)
{
	if ((hDib) && (pDC)) {
		//palette must be correctly filled
		char* lpDIB = (char*)hDib;	//set image to hdc...
		SetStretchBltMode(pDC,COLORONCOLOR);
		StretchDIBits(pDC, xoffset, yoffset, xsize, ysize, 0, 0, m_bi.biWidth, m_bi.biHeight,
			GetBits(),(BITMAPINFO*) lpDIB, DIB_RGB_COLORS, SRCCOPY);
		return 1;
	}
	return 0;
}

void ttDib::SetPaletteIndex(BYTE idx, BYTE r, BYTE g, BYTE b)
{
	if ((hDib) && (m_nColors)){
		BYTE* iDst = (BYTE*)(hDib) + sizeof(BITMAPINFOHEADER);
		if (idx < m_nColors) {
			long ldx = idx * sizeof(RGBQUAD);
			iDst[ldx++] = (BYTE) b;
			iDst[ldx++] = (BYTE) g;
			iDst[ldx++] = (BYTE) r;
			iDst[ldx] = (BYTE) 0;
		}
	}
}

void ttDib::SetPaletteIndex(BYTE idx, RGBQUAD c)
{
	if ((hDib) && (m_nColors)){
		BYTE* iDst = (BYTE*)(hDib) + sizeof(BITMAPINFOHEADER);
		if (idx < m_nColors) {
			long ldx=idx*sizeof(RGBQUAD);
			iDst[ldx++] = (BYTE) c.rgbBlue;
			iDst[ldx++] = (BYTE) c.rgbGreen;
			iDst[ldx++] = (BYTE) c.rgbRed;
			iDst[ldx] = (BYTE) 0;
		}
	}
}

void ttDib::SetPaletteIndex(BYTE idx, COLORREF cr)
{
	if ((hDib) && (m_nColors)){
		BYTE* iDst = (BYTE*)(hDib) + sizeof(BITMAPINFOHEADER);
		if (idx < m_nColors) {
			long ldx = idx * sizeof(RGBQUAD);
			iDst[ldx++] = (BYTE) GetBValue(cr & 0xFFFFFF);
			iDst[ldx++] = (BYTE) GetGValue(cr & 0xFFFF);
			iDst[ldx++] = (BYTE) GetRValue(cr & 0xFF);
			iDst[ldx] = (BYTE) 0;
		}
	}
}

RGBQUAD ttDib::GetPaletteIndex(BYTE idx)
{
	RGBQUAD rgb = {0,0,0,0};
	if ((hDib) && (m_nColors)){
		BYTE* iDst = (BYTE*)(hDib) + sizeof(BITMAPINFOHEADER);
		if (idx < m_nColors) {
			long ldx = idx * sizeof(RGBQUAD);
			rgb.rgbBlue = iDst[ldx++];
			rgb.rgbGreen=iDst[ldx++];
			rgb.rgbRed =iDst[ldx];
			rgb.rgbReserved = 0;
		}
	}
	return rgb;
}

BYTE ttDib::GetPixelIndex(long x, long y)
{
	if ((hDib == NULL)	|| (m_nColors == 0)  || (x < 0)  || (y < 0)  || (x >= m_bi.biWidth)  || (y >= m_bi.biHeight))
		return 0;
	BYTE* iDst = GetBits();
	return iDst[(m_bi.biHeight - y - 1) * m_LineWidth + x];
}

RGBQUAD ttDib::GetPixelColor(long x, long y)
{
	RGBQUAD rgb = {0, 0, 0, 0};
	if ((hDib == NULL)	|| (x < 0)	|| (y < 0)	|| (x >= m_bi.biWidth) || (y >= m_bi.biHeight))
		return rgb;
	if (m_nColors)
		return GetPaletteIndex(GetPixelIndex(x,y));
	else {
		BYTE* iDst = GetBits()+(m_bi.biHeight - y - 1) * m_LineWidth + x * sizeof(RGBQUAD);
		rgb.rgbBlue = *iDst++;
		rgb.rgbGreen= *iDst++;
		rgb.rgbRed =*iDst;
		return rgb;
	}
}

void ttDib::SetPixelIndex(long x, long y, BYTE i)
{
	if ((hDib == NULL) || (m_nColors == 0) || (x < 0) || (y < 0) || (x >= m_bi.biWidth) || (y >= m_bi.biHeight))
		return;
	BYTE* iDst = GetBits();
	iDst[(m_bi.biHeight - y - 1) * m_LineWidth + x] = i;
}

void ttDib::SetPixelColor(long x, long y, COLORREF cr)
{
	SetPixelColor(x,y,RGB2RGBQUAD(cr));
}

void ttDib::SetPixelColor(long x,long y,RGBQUAD c)
{
	if ((hDib==NULL)||(x<0)||(y<0)|| (x>=m_bi.biWidth)||(y>=m_bi.biHeight))
		return;
	if (m_nColors)
		SetPixelIndex(x,y,GetNearestIndex(c));
	else {
		BYTE* iDst = GetBits() +(m_bi.biHeight - y - 1) * m_LineWidth + x * sizeof(RGBQUAD);
		*(RGBQUAD*)iDst = c;
	}
}

BYTE ttDib::GetNearestIndex(RGBQUAD c)
{
	if ((hDib == NULL)	|| (m_nColors == 0))
		return 0;
	BYTE* iDst = (BYTE*)(hDib) + sizeof(BITMAPINFOHEADER);
	long distance = 200000;
	BYTE i,j = 0;
	long k, l;
	for(i = 0, l = 0;i<m_nColors;i++, l += sizeof(RGBQUAD)) {
		k = (iDst[l]-c.rgbBlue) *(iDst[l]-c.rgbBlue) +
			(iDst[l+1]-c.rgbGreen) *(iDst[l+1]-c.rgbGreen) +
			(iDst[l+2]-c.rgbRed) *(iDst[l+2]-c.rgbRed);
		if (k == 0) {
			j = i;
			break;
		}
		if (k<distance) {
			distance = k;
			j = i;
		}
	}
	return j;
}

#define	 HSLMAX	  240	/* H,L, and S vary over 0-HSLMAX */
#define	 RGBMAX	  255	/* R,G, and B vary over 0-RGBMAX */
						/* HSLMAX BEST IF DIVISIBLE BY 6 */
						/* RGBMAX, HSLMAX must each fit in a byte. */
/* Hue is undefined if Saturation is 0 (grey-scale) */
/* This value determines where the Hue scrollbar is */
/* initially set for achromatic colors */
#define UNDEFINED (HSLMAX*2/3)

RGBQUAD ttDib::RGBtoHSL(RGBQUAD lRGBColor)
{
	BYTE R, G, B;				  // input RGB values
	BYTE H, L, S;				  // output HSL values
	BYTE cMax, cMin;			  // max and min RGB values
	WORD Rdelta, Gdelta, Bdelta;  // intermediate value: % of spread from max

	R = lRGBColor.rgbRed;		  // get R, G, and B out of DWORD
	G = lRGBColor.rgbGreen;
	B = lRGBColor.rgbBlue;

	cMax = max(max(R, G), B);	  // calculate lightness
	cMin = min(min(R, G), B);
	L = ((((cMax + cMin)*HSLMAX) + RGBMAX) / (2 *RGBMAX)) & 0xFF;

	if (cMax == cMin) { 		   // r=g=b --> achromatic case
		S = 0;					   // saturation
		H = UNDEFINED;			   // hue
	} else {					   // chromatic case
		if (L <= (HSLMAX / 2))	   // saturation
			S = (((cMax - cMin) * HSLMAX) + ((cMax + cMin) / 2)) / (cMax + cMin);
		else
			S = ((((cMax - cMin)*HSLMAX) + ((2 *RGBMAX - cMax - cMin) / 2)) / (2 *RGBMAX - cMax - cMin)) & 0xFF;
		// hue
		Rdelta = (((cMax - R)*(HSLMAX / 6)) + ((cMax - cMin) / 2)) / (cMax - cMin);
		Gdelta = (((cMax - G)*(HSLMAX / 6)) + ((cMax - cMin) / 2)) / (cMax - cMin);
		Bdelta = (((cMax - B)*(HSLMAX / 6)) + ((cMax - cMin) / 2)) / (cMax - cMin);

		if (R == cMax)
			H = (BYTE) (Bdelta - Gdelta);
		else if (G == cMax)
			H = (BYTE) ((HSLMAX / 3) + Rdelta - Bdelta);
		else // B == cMax
			H = (BYTE) (((2 * HSLMAX) / 3) + Gdelta - Rdelta);

		if (H < 0)
			H += HSLMAX;
		if (H > HSLMAX)
			H -= HSLMAX;
	}
	RGBQUAD hsl = {L, S, H, 0};
	return hsl;
}

WORD ttDib::HueToRGB(WORD n1, WORD n2, WORD hue)
{
// range check: note values passed add/subtract thirds of range
	if (hue < 0) hue += HSLMAX;
	if (hue > HSLMAX) hue -= HSLMAX;

// return r,g, or b value from this tridrant
	if (hue < (HSLMAX/6))
		return (n1 + (((n2 - n1) * hue + (HSLMAX / 12)) / (HSLMAX / 6)));
	if (hue < (HSLMAX / 2))
		return (n2);
	if (hue < ((HSLMAX * 2) / 3))
		return (n1 + (((n2 - n1)*(((HSLMAX * 2) / 3) - hue) + (HSLMAX / 12)) / (HSLMAX / 6)));
	else
		return (n1);
}

RGBQUAD ttDib::HSLtoRGB(COLORREF cHSLColor)
{
	return HSLtoRGB(RGB2RGBQUAD(cHSLColor));
}

RGBQUAD ttDib::HSLtoRGB(RGBQUAD lHSLColor)
{
	WORD hue, lum, sat;
	BYTE R, G, B;				  // RGB component values
	WORD Magic1, Magic2;		  // calculated magic numbers (really!)

	hue = lHSLColor.rgbRed; 	  // get H, S, and L out of DWORD
	sat = lHSLColor.rgbGreen;
	lum = lHSLColor.rgbBlue;

	if (sat == 0) { 			  // achromatic case
		R = G = B = (BYTE) ((lum * RGBMAX) / HSLMAX);
	} else {					  // chromatic case
		// set up magic numbers
		if (lum <= (HSLMAX / 2))
			Magic2 = (lum*(HSLMAX + sat) + (HSLMAX / 2)) / HSLMAX;
		else
			Magic2 = lum + sat - ((lum * sat) + (HSLMAX / 2)) / HSLMAX;
		Magic1 = 2 * lum - Magic2;

		/* get RGB, change units from HSLMAX to RGBMAX */
		R = (BYTE) (HueToRGB(Magic1, Magic2,(WORD) (hue+(HSLMAX / 3))) * RGBMAX + (HSLMAX / 2)) / HSLMAX;
		G = (BYTE) (HueToRGB(Magic1, Magic2, hue) *RGBMAX + (HSLMAX / 2)) / HSLMAX;
		B = (BYTE) (HueToRGB(Magic1, Magic2,(WORD) (hue-(HSLMAX / 3))) * RGBMAX + (HSLMAX / 2)) / HSLMAX;
	}
	RGBQUAD rgb = {B, G, R, 0};
	return rgb;
}

RGBQUAD ttDib::RGB2RGBQUAD(COLORREF cr)
{
	RGBQUAD c;
	c.rgbRed = GetRValue(cr & 0xFF);	/* get R, G, and B out of DWORD */
	c.rgbGreen = GetGValue(cr & 0xFFFF);
	c.rgbBlue = GetBValue(cr & 0xFFFFFF);
	c.rgbReserved = 0;
	return c;
}

COLORREF ttDib::RGBQUAD2RGB(RGBQUAD c)
{
	return RGB(c.rgbRed, c.rgbGreen, c.rgbBlue);
}

void ttDib::SetGrayPalette()
{
	if ((hDib == NULL) || (m_nColors == 0))
		return;
	RGBQUAD pal[256];
	RGBQUAD* ppal;
	BYTE* iDst;
	int ni;
	ppal = (RGBQUAD*) &pal[0];
	iDst = (BYTE*)(hDib) + sizeof(BITMAPINFOHEADER);
	for (ni = 0; ni < m_nColors; ni++) {
		pal[ni] = RGB2RGBQUAD(RGB(ni, ni, ni));
	}
	pal[0] = RGB2RGBQUAD(RGB(0, 0, 0));
	pal[m_nColors - 1] = RGB2RGBQUAD(RGB(255, 255, 255));
	memcpy(iDst, ppal, GetPaletteSize());
}

void ttDib::BlendPalette(COLORREF cr, long perc)
{
	if ((hDib == NULL) || (m_nColors == 0))
		return;
	BYTE* iDst = (BYTE*)(hDib) + sizeof(BITMAPINFOHEADER);
	long i, r, g, b;
	RGBQUAD* pPal = (RGBQUAD*) iDst;
	r = GetRValue(cr & 0xFF);
	g = GetGValue(cr & 0xFFFF);
	b = GetBValue(cr & 0xFFFFFF);
	if (perc > 100)
		perc = 100;
	for (i = 0; i < m_nColors; i++) {
		pPal[i].rgbBlue = (BYTE)((pPal[i].rgbBlue*(100 - perc) + b * perc) / 100);
		pPal[i].rgbGreen = (BYTE)((pPal[i].rgbGreen*(100 - perc) + g * perc) / 100);
		pPal[i].rgbRed = (BYTE)((pPal[i].rgbRed*(100 - perc) + r * perc) / 100);
	}
}

long ttDib::GetSize()
{
	return m_bi.biSize + m_bi.biSizeImage + GetPaletteSize();
}

BOOL ttDib::IsValid()
{
	return (hDib!=NULL);
}

void ttDib::Clone(ttDib *src)
{
	Create(src->GetWidth(), src->GetHeight(), src->GetBitCount());
	if (hDib)
		memcpy(hDib, src->hDib, GetSize());
}

void ttDib::Clear(BYTE bval)
{
	if (hDib)
		memset(GetBits(), bval, m_bi.biSizeImage);
}
