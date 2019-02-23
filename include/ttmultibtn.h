/////////////////////////////////////////////////////////////////////////////
// Name:		ttCMultiBtn
// Purpose:		Class for applying CShadeBtn to every button in a dialog
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include "../include/ttarray.h" 	// CTTArray
#include "../include/ttshadebtn.h"	// CShadeBtn

namespace ttpriv {
	BOOL WINAPI EnumBtnProc(HWND hwnd, LPARAM lval);
}

class ttCMultiBtn
{
public:
	ttCMultiBtn() { }
	~ttCMultiBtn();

	void SetIcon(int idBtn, int idIcon, UINT nIconAlign = BS_LEFT);
	ttCShadeBtn* FindShadeBtn(int id);

	// Class functions

	void Initialize(HWND hwndParent, ttCShadeBtn::BTN_SHADE shade = ttCShadeBtn::SHS_HARDBUMP);

private:
	friend BOOL WINAPI ttpriv::EnumBtnProc(HWND hwnd, LPARAM lval);

	// Class members

	ttCArray<ttCShadeBtn*> m_aBtns;
	ttCShadeBtn::BTN_SHADE m_btnShade;
};
