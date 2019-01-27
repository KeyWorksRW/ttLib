/////////////////////////////////////////////////////////////////////////////
// Name:		ttMultiBtn
// Purpose:		Class for applying ttShadeBtn to every button in a dialog
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include "../include/multibtn.h"	// ttMultiBtn

BOOL WINAPI ttpriv::EnumBtnProc(HWND hwnd, LPARAM lval)
{
	char szClass[MAX_PATH];
	if ((GetWindowLong(hwnd, GWL_STYLE) & 0x0f) < BS_CHECKBOX) {
		GetClassName(hwnd, szClass, sizeof(szClass));
		if (tt::samestri(szClass, "BUTTON")) {
			ttMultiBtn* pThis = (ttMultiBtn*) lval;
			ttShadeBtn* pBtn = new ttShadeBtn;
			pBtn->SubClass(hwnd);
			pBtn->SetShade(pThis->m_btnShade);
			pThis->m_aBtns.Add(pBtn);
		}
	}
	return TRUE;
}

void ttMultiBtn::Initialize(HWND hwndParent, ttShadeBtn::BTN_SHADE shade)
{
	m_btnShade = shade;
	EnumChildWindows(hwndParent, (WNDENUMPROC) ttpriv::EnumBtnProc, (LPARAM) this);
}

ttMultiBtn::~ttMultiBtn()
{
	for (size_t i = 0; i < m_aBtns.GetCount(); i++) {
		delete m_aBtns[i];
	}
}
