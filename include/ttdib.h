/////////////////////////////////////////////////////////////////////////////
// Name:		ttDib
// Purpose:		Class for handling Device Independent Bitmap
// Author:		Davide Pizzolato
// Copyright:	Copyright (c) [2001] Davide Pizzolato
// Licence:		The Code Project Open License (see CPOL.htm)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

// REVIEW: [randalphwa - 1/10/2019] This is a bug waiting to happen. Handles in Windows are freed with DeleteObject().
// HDIB looks like a GDI Object, and if it really was, it would be handed to DeleteObject(). A HANDLE in Windows is a
// pointer, but you do NOT use standard memory routines to allocate/free it the way HDIB is being used here. The HDIB
// should be replaced with a void* since it gets cast to different structure pointers.

#define HDIB HANDLE

class ttDib
{
public:
	ttDib();
	~ttDib();

	// Class functions

	HDIB	Create(DWORD dwWidth, DWORD dwHeight, WORD wBitCount);
	void	Clear(BYTE bval=0);
	void	Clone(ttDib *src);
	BOOL	IsValid();
	long	GetSize();
	BYTE*	GetBits();
	DWORD	GetHeight() {return m_bi.biHeight;};
	DWORD	GetWidth() {return m_bi.biWidth;};
	DWORD	GetLineWidth() {return m_LineWidth;};
	WORD	GetNumColors() {return m_nColors;};
	WORD	GetBitCount() {return m_bi.biBitCount;};
	WORD	GetPaletteSize();
	BYTE	GetPixelIndex(long x,long y);
	RGBQUAD GetPaletteIndex(BYTE idx);
	RGBQUAD GetPixelColor(long x,long y);
	BYTE	GetNearestIndex(RGBQUAD c);
	long	Draw(HDC pDC, long xoffset, long yoffset);
	long	Stretch(HDC pDC, long xoffset, long yoffset, long xsize, long ysize);
	void	BlendPalette(COLORREF cr,long perc);
	void	SetPaletteIndex(BYTE idx, BYTE r, BYTE g, BYTE b);
	void	SetPaletteIndex(BYTE idx, RGBQUAD c);
	void	SetPaletteIndex(BYTE idx, COLORREF cr);
	void	SetPixelColor(long x,long y,RGBQUAD c);
	void	SetPixelIndex(long x,long y,BYTE i);
	void	SetPixelColor(long x,long y,COLORREF cr);
	void	SetGrayPalette();
	RGBQUAD HSLtoRGB(COLORREF cHSLColor);
	RGBQUAD RGB2RGBQUAD(COLORREF cr);
	COLORREF RGBQUAD2RGB (RGBQUAD c);
	RGBQUAD RGBtoHSL(RGBQUAD lRGBColor);
	RGBQUAD HSLtoRGB(RGBQUAD lHSLColor);

protected:
	bool IsWin30Dib();
	WORD HueToRGB(WORD n1,WORD n2,WORD hue);

	// Class members

	HDIB hDib;
	BITMAPINFOHEADER m_bi;
	DWORD m_LineWidth;
	WORD  m_nColors;
};
