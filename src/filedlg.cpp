/////////////////////////////////////////////////////////////////////////////
// Name:		CFileDlg
// Purpose:		Class for displaying Windows File Open dialog
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 2002-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include "../include/filedlg.h"

UINT_PTR CALLBACK _OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam);

CFileDlg::CFileDlg(HWND hwndParent)
{
	int cbStruct = sizeof(OPENFILENAME);

	// If we are running on Windows XP or higher, then make room for pvReserved, dwReserved, and FlagsEx.
	// That's why we allocate OPENFILENAME rather then just declaring it in our class

#if (_WIN32_WINNT < 0x0500)
	if (_osv.dwMajorVersion >= 5)
		cbStruct += sizeof(void*) + sizeof(DWORD) + sizeof(DWORD);
#endif
	m_pofn = (OPENFILENAMEA*) tt::calloc(cbStruct);

	m_cszFileName.Enlarge(MAX_PATH);

	m_pofn->lStructSize = cbStruct;
	m_pofn->hwndOwner = hwndParent ? hwndParent : GetActiveWindow();
	*(char*) m_cszFileName = 0;
	m_pofn->lpstrFile = m_cszFileName;
	m_pofn->nMaxFile = MAX_PATH;
	m_pofn->lpfnHook = _OFNHookProc;
	m_pofn->Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT;
	m_pofn->lCustData = (LPARAM) this;

	m_bRepositionWindow = false;
	memset(&m_rcPosition, 0, sizeof(m_rcPosition));
}

CFileDlg::~CFileDlg()
{
	tt::free(m_pofn);
}

bool CFileDlg::GetOpenFileName()
{
	if (!::GetOpenFileNameA(m_pofn)) {
#ifdef _DEBUG
		DWORD error = CommDlgExtendedError();	// Will be 0 if the user cancelled, else it's an actual error
		ASSERT_COMMENT(error == 0, "An error occurred in the Open dialog box");
#endif
		return false;
	}

	if (m_pofn->Flags & OFN_FILEMUSTEXIST)
		FixExtension();
	return true;
}

bool CFileDlg::GetSaveFileName()
{
	m_pofn->Flags &= ~OFN_FILEMUSTEXIST;
	m_pofn->Flags |= OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	if (!::GetSaveFileNameA(m_pofn)) {
#ifdef _DEBUG
		DWORD error = CommDlgExtendedError();	// Will be 0 if the user cancelled, else it's an actual error
		ASSERT_COMMENT(error == 0, "An error occurred in the Open dialog box");
#endif
		return false;
	}
	FixExtension();
	return true;
}

void CFileDlg::FixExtension()
{
	if (kstrchr(m_cszFileName, '.'))
		return;	// we have an extension, return

	const char* psz = m_pofn->lpstrFilter;
	for (DWORD i = 1; i < m_pofn->nFilterIndex; i++) {
		psz = psz + strlen(psz) + 1;
		psz = psz + strlen(psz) + 1;
	}
	psz = psz + kstrlen(psz) + 1;
	ASSERT(psz);
	char* pszTmp = kstrchr(psz, ';');
	if (pszTmp)
		*pszTmp = '\0';
	m_cszFileName.ChangeExtension(psz + 1);
}

void CFileDlg::SetFilter(const char* pszFilters)
{
	ASSERT(pszFilters);
	if (!pszFilters)
		return;

	m_cszFilter = pszFilters;
	char* psz = kstrchr(m_cszFilter, '|');
	while (psz) {
		*psz = '\0';
		psz = kstrchr(psz + 1, '|');
	}
	m_pofn->lpstrFilter = m_cszFilter.getptr();
}

void CFileDlg::SetFilter(int idResource)
{
	m_cszFilter.GetResString(idResource);
	char* psz = kstrchr(m_cszFilter, '|');
	while (psz) {
		*psz = '\0';
		psz = kstrchr(psz + 1, '|');
	}
	m_pofn->lpstrFilter = m_cszFilter.getptr();
}

UINT_PTR CALLBACK _OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG) {
		SetWindowLongPtr(hdlg, GWLP_USERDATA, ((OPENFILENAME*) lParam)->lCustData);
		CFileDlg* pThis = (CFileDlg*) ((OPENFILENAME*) lParam)->lCustData;

#if 0
		pThis->m_ShadedBtns.Initialize(GetParent(hdlg));
		if (pThis->m_idOpenIcon != -1)
			pThis->m_ShadedBtns.SetIcon(IDOK, pThis->m_idOpenIcon);
		if (pThis->m_idCancelIcon != -1)
			pThis->m_ShadedBtns.SetIcon(IDCANCEL, pThis->m_idCancelIcon);
#endif

		if (!IsRectEmpty(&pThis->m_rcPosition))
			pThis->m_bRepositionWindow = true;
#if 0	// REVIEW: [randalphwa - 09-03-2018] enable this if we add CenterWindow()
		else
			CenterWindow(GetParent(GetParent(hdlg)), GetParent(hdlg));
#endif
		return TRUE;
	}

	if (uMsg == WM_NOTIFY) {
		OFNOTIFY* ofn = (OFNOTIFY*) lParam;
		switch (ofn->hdr.code) {

			// We cannot reposition the window during the CDN_INITDONE or WM_INITDIALOG messages because the
			// dialog will not be drawn correctly, and cannot be resized (system determines dialog
			// size/layout after this message is processed). It also doesn't work to hide the window and
			// display it later because the system will show the window after we tell it to be hidden. So
			// instead, we track the last notification message we receive before the dialog is displayed, and
			// set the dialog size/position there.

			case CDN_FOLDERCHANGE:
				{
					CFileDlg* pThis = (CFileDlg*) GetWindowLongPtr(hdlg, GWLP_USERDATA);
					if (pThis->m_bRepositionWindow)	{
						pThis->m_bRepositionWindow = false;
						MoveWindow(GetParent(hdlg), pThis->m_rcPosition.left, pThis->m_rcPosition.top,
							RECT_WIDTH(pThis->m_rcPosition), RECT_HEIGHT(pThis->m_rcPosition), FALSE);
					}
				}
				break;
		}
	}
	else if (uMsg == WM_DESTROY) {
		CFileDlg* pThis = (CFileDlg*) GetWindowLongPtr(hdlg, GWLP_USERDATA);
		GetWindowRect(GetParent(hdlg), &pThis->m_rcPosition);
	}
	return 0;
}
