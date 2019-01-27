/////////////////////////////////////////////////////////////////////////////
// Name:		ttMultiBtn
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
#include "../include/shadebtn.h"	// CShadeBtn

namespace ttpriv {
	BOOL WINAPI EnumBtnProc(HWND hwnd, LPARAM lval);
}

class ttMultiBtn
{
public:
	ttMultiBtn() { }
	~ttMultiBtn();

	void SetIcon(int idBtn, int idIcon, UINT nIconAlign = BS_LEFT) {
		for (size_t i = 0; i < m_aBtns.GetCount(); i++) {
			if (IsWindow(*m_aBtns[i]) && GetDlgCtrlID(*m_aBtns[i]) == idBtn) {
				m_aBtns[i]->SetIcon(idIcon, nIconAlign);
				return;
			}
		}
	}
	ttShadeBtn* FindShadeBtn(int id) {
		for (size_t i = 0; i < m_aBtns.GetCount(); i++) {
			if (GetDlgCtrlID(*m_aBtns[i]) == id) {
				return m_aBtns[i];
			}
		}
		return NULL;
	}

	// Class functions

	void Initialize(HWND hwndParent, ttShadeBtn::BTN_SHADE shade = ttShadeBtn::SHS_HARDBUMP);

private:
	friend BOOL WINAPI ttpriv::EnumBtnProc(HWND hwnd, LPARAM lval);

	// Class members

	ttArray<ttShadeBtn*> m_aBtns;
	ttShadeBtn::BTN_SHADE m_btnShade;
};
