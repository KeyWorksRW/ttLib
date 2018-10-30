/////////////////////////////////////////////////////////////////////////////
// Name:		CFileDlg
// Purpose:		Wrapper around Windows GetOpenFileName() API
// Author:		Ralph Walden (randalphwa)
// Copyright:   Copyright (c) 2002-2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_FILEDLG_H__
#define __TTLIB_FILEDLG_H__

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

#include <commdlg.h>

#include "cstr.h"	// CStr

#ifndef OFN_DONTADDTORECENT
	#define OFN_DONTADDTORECENT			 0x02000000
#endif

class CFileDlg
{
public:
	CFileDlg(HWND hwndParent = NULL);
	~CFileDlg();

	// Class functions

	bool GetOpenFileName();	// call this or the following to launch the actual dialog box
	bool GetSaveFileName();

	char*	GetFileName() { return m_cszFileName; }	// call this after one of the above two functions has been called

	void	AddToRecent() { m_pofn->Flags &= ~OFN_DONTADDTORECENT; }
	void	SetFilter(int idResource);
	void	SetFilter(const char* pszFilters);	// separate filters with '|' character
	void	SetInitialDir(const char* pszFolder) { m_pofn->lpstrInitialDir = pszFolder; }
	void	SetInitialFileName(const char* psz) { ASSERT(kstrlen(psz) < MAX_PATH); m_cszFileName = psz; }
	void	ShowCreatePrompt() { m_pofn->Flags &= ~OFN_FILEMUSTEXIST; m_pofn->Flags |= OFN_CREATEPROMPT; }
	void	ShowReadOnlyBox() { m_pofn->Flags &= ~OFN_HIDEREADONLY; }
	void	UseCurrentDirectory() { m_cszCurDir.GetCWD(); m_pofn->lpstrInitialDir = m_cszCurDir; }

	void  SetWindowRect(const RECT* prc) { ASSERT(!IsRectEmpty(prc)); CopyRect(&m_rcPosition, prc); }
	RECT* GetWindowRect() { return &m_rcPosition; }

	// By default, the file must exist

	void  SetFileMustExist(bool bMustExist = true) { if (bMustExist) m_pofn->Flags |= OFN_FILEMUSTEXIST; else m_pofn->Flags &= ~OFN_FILEMUSTEXIST; }

	OPENFILENAMEA* GetOF() { return m_pofn; }
	operator OPENFILENAMEA*() const { return m_pofn; }

private:
	void FixExtension();

	friend UINT_PTR CALLBACK _OFNHookProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Class members

protected:
	bool  m_bRepositionWindow;
	CStr  m_cszFileName;
	CStr  m_cszCurDir;
	CStr  m_cszFilter;
	int	  m_idOpenIcon;
	int	  m_idCancelIcon;
	RECT  m_rcPosition;

	// This is malloc'd because it is a different size when running on XP then on Win 9x.
	OPENFILENAMEA* m_pofn;

	// CMultiBtn m_ShadedBtns;
};

#endif	// __TTLIB_FILEDLG_H__
