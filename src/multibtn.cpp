/////////////////////////////////////////////////////////////////////////////
// Name:		ttMultiBtn
// Purpose:		Class for applying ttShadeBtn to every button in a dialog
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include "../include/ttmultibtn.h"	// ttMultiBtn

BOOL WINAPI ttpriv::EnumBtnProc(HWND hwnd, LPARAM lval)
{
	char szClass[MAX_PATH];
	if ((GetWindowLong(hwnd, GWL_STYLE) & 0x0f) < BS_CHECKBOX) {
		GetClassName(hwnd, szClass, sizeof(szClass));
		if (tt::samestri(szClass, "Button")) {
			ttMultiBtn* pMultiBtn = (ttMultiBtn*) lval;
			ttShadeBtn* pBtn = new ttShadeBtn;
			pBtn->SubClass(hwnd);
			pBtn->SetShade(pMultiBtn->m_btnShade);
			pMultiBtn->m_aBtns.Add(pBtn);
		}
	}
	return TRUE;
}

ttMultiBtn::~ttMultiBtn()
{
	for (size_t i = 0; i < m_aBtns.GetCount(); i++) {
		delete m_aBtns[i];
	}
}

void ttMultiBtn::Initialize(HWND hwndParent, ttShadeBtn::BTN_SHADE shade)
{
	m_btnShade = shade;
	EnumChildWindows(hwndParent, (WNDENUMPROC) ttpriv::EnumBtnProc, (LPARAM) this);
}

void ttMultiBtn::SetIcon(int idBtn, int idIcon, UINT nIconAlign)
{
	ttASSERT_MSG(m_aBtns.GetCount(), "Calling SetIcon without any buttons to set (Initialize not called? EnableShadeBtns not called?)");

	for (size_t i = 0; i < m_aBtns.GetCount(); i++) {
		if (IsWindow(*m_aBtns[i]) && GetDlgCtrlID(*m_aBtns[i]) == idBtn) {
			m_aBtns[i]->SetIcon(idIcon, nIconAlign);
			return;
		}
#ifdef _DEBUG
		if (IsWindow(*m_aBtns[i])) {
			int id = GetDlgCtrlID(*m_aBtns[i]);
			tt::KeyTrace("Non-tracked DlgId: %s\n", id);
		}
#endif
	}
}

ttShadeBtn* ttMultiBtn::FindShadeBtn(int id)
{
	ttASSERT_MSG(m_aBtns.GetCount(), "Calling FindShadeBtn without any buttons to set (Initialize not called? EnableShadeBtns not called?)");

	for (size_t i = 0; i < m_aBtns.GetCount(); i++) {
		if (GetDlgCtrlID(*m_aBtns[i]) == id) {
			return m_aBtns[i];
		}
	}
	return NULL;
}
