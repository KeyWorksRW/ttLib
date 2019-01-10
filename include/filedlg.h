/////////////////////////////////////////////////////////////////////////////
// Name:		ttFileDlg
// Purpose:		Wrapper around Windows GetOpenFileName() API
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_FILEDLG_H__
#define __TTLIB_FILEDLG_H__

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include <commdlg.h>

#include "ttstring.h"	// ttString

#ifndef OFN_DONTADDTORECENT
	#define OFN_DONTADDTORECENT			 0x02000000
#endif

namespace ttpriv {
	UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam);
}

class ttFileDlg
{
public:
	ttFileDlg(HWND hwndParent = NULL);
	~ttFileDlg();

	// Class functions

	bool GetOpenFileName();	// call this or the following to launch the actual dialog box
	bool GetSaveFileName();

	char*	GetFileName() { return m_cszFileName; }	// call this after one of the above two functions has been called

	void	AddToRecent() { m_pofn->Flags &= ~OFN_DONTADDTORECENT; }
	void	SetFilter(int idResource);
	void	SetFilter(const char* pszFilters);	// separate filters with '|' character
	void	SetInitialDir(const char* pszFolder) { m_pofn->lpstrInitialDir = pszFolder; }
	void	SetInitialFileName(const char* psz) { ttASSERT(tt::strlen(psz) < MAX_PATH); m_cszFileName = psz; }
	void	ShowCreatePrompt() { m_pofn->Flags &= ~OFN_FILEMUSTEXIST; m_pofn->Flags |= OFN_CREATEPROMPT; }
	void	ShowReadOnlyBox() { m_pofn->Flags &= ~OFN_HIDEREADONLY; }
	void	UseCurrentDirectory() { m_cszCurDir.GetCWD(); m_pofn->lpstrInitialDir = m_cszCurDir; }

	void  SetWindowRect(const RECT* prc) { ttASSERT(!IsRectEmpty(prc)); CopyRect(&m_rcPosition, prc); }
	RECT* GetWindowRect() { return &m_rcPosition; }

	// By default, the file must exist

	void  SetFileMustExist(bool bMustExist = true) { if (bMustExist) m_pofn->Flags |= OFN_FILEMUSTEXIST; else m_pofn->Flags &= ~OFN_FILEMUSTEXIST; }

	OPENFILENAMEA* GetOF() { return m_pofn; }
	operator OPENFILENAMEA*() const { return m_pofn; }

private:
	void FixExtension();

	friend UINT_PTR CALLBACK ttpriv::OFNHookProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Class members

protected:
	bool  m_bRepositionWindow;
	ttString  m_cszFileName;
	ttString  m_cszCurDir;
	ttString  m_cszFilter;
	int	  m_idOpenIcon;
	int	  m_idCancelIcon;
	RECT  m_rcPosition;

	// This is malloc'd because it is a different size when running on XP then on Win 9x.
	OPENFILENAMEA* m_pofn;

	// CMultiBtn m_ShadedBtns;
};

#endif	// __TTLIB_FILEDLG_H__
