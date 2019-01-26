/////////////////////////////////////////////////////////////////////////////
// Name:		shadebtn.h
// Purpose:		Class for implementing an owner-draw button
// Author:		Davide Pizzolato
// Copyright:   Copyright (c) [2001] Davide Pizzolato
// Licence:     The Code Project Open License (see ../CPOL.md)
/////////////////////////////////////////////////////////////////////////////

// Changes: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)

// This class is derived from CxShadeButton, changed to use WTL instead of MFC

// For the original, see article at https://www.codeproject.com/articles/1121/CShadeBtn

#pragma once

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#ifndef __ATLBASE_H__
	#include <atlbase.h>
#endif

#ifndef __ATLAPP_H__
	#include <atlapp.h>
#endif

#ifndef __ATLCTRLS_H__
	#include <atlctrls.h>
#endif

#ifndef _INC_SHELLAPI
	#include <shellapi.h>	// required by "atlctrlx.h"
#endif

#ifndef __ATLCTRLX_H__
	#include <atlctrlx.h>
#endif

#include "ttdib.h"

#include "ttmsgs.h"	// WTL-like messages

namespace ShadeBtn {
	// #define WM_CXSHADE_RADIO WM_USER + 0x100		// [randalphwa - 01-04-2019] Never used

	#define MSG_BM_SETSTATE(func) \
		if (uMsg == BM_SETSTATE) \
		{ \
			func(); \
			return FALSE; \
		}
}

class ttShadeBtn : public CBitmapButton
{
public:
	ttShadeBtn();
	~ttShadeBtn();

	typedef enum {
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

	BEGIN_MSG_MAP(ttShadeBtn)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ENABLE(OnEnable)
		MSG_BM_SETSTATE(OnSetState)
	END_MSG_MAP()

	BOOL SubclassWindow(HWND hWnd) {
		m_Style = ::GetWindowLong(hWnd, GWL_STYLE);	// save style now before it gets changed to owner-draw
		return CBitmapButton::SubclassWindow(hWnd);
	}

	void DoPaint(HDC dc);
	void OnEnable(WPARAM /* bEnabled */) {
		Invalidate();
		// UpdateWindow();	// [ralphw - 02-16-2010] I can't think of any reason why button needs to be redrawn immediately
	}

	// [ralphw - 02-16-2010] We have to process BM_SETSTATE ourselves because if the application consists
	// only of a Modal dialog box, the button control never gets it's window invalidated. In a dialog from a
	// main window, this does result in one extra WM_PAINT being sent when the button is first clicked.

	void OnSetState() {
		Invalidate();
		// UpdateWindow();
	}

	// Class functions

	void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);
	void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
	void Draw3dRect(HDC hdc, RECT* pRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	void OnPaint();
	LOGFONT* GetFont() { return m_pLF; }
	bool SetFont(LOGFONT* pNewStyle);
	bool SetFont(const char* pszFontName, long lSize = 0, long lWeight = 400, BYTE bItalic = 0, BYTE bUnderline = 0);
	void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE);
	void SetFlat(bool bFlag) { m_flat=bFlag; }
	void SetTextAlign(UINT nTextAlign);
	void SetIcon(UINT nIcon, UINT nIconAlign = BS_CENTER, UINT nIconDown = 0, UINT nIconHighLight = 0);
	void SetShade(BTN_SHADE shadeID = SHS_NOISE, BYTE granularity = 8, BYTE highlight = 10, BYTE coloring = 0, COLORREF color = 0);
	COLORREF SetTextColor(COLORREF new_color);

	bool operator==(ttShadeBtn* pShade) { return m_hWnd == pShade->m_hWnd; }

protected:
	// Class members

	HFONT	m_hFont;		// font object
	LOGFONT* m_pLF; 		// font structure
	bool	m_Checked;		// radio & check buttons
	bool	m_IsPushLike;	// radio & check buttons
	DWORD	m_Style;
	ttDib	m_dNormal, m_dDown, m_dDisabled, m_dOver, m_dh, m_dv;
	short	m_FocusRectMargin;	// dotted margin offset
	COLORREF m_TextColor;		// button text color
	BOOL	m_Border;			// 0=flat; 1=3D;
	HICON	m_Icon, m_IconDown, m_IconHighLight;
	RECT	m_rcIconBox;
	UINT	m_TextAlign;
	UINT	m_IconAlign;
	bool	m_flat;
};
