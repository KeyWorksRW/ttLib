/////////////////////////////////////////////////////////////////////////////
// Name:		ttCFileDlg
// Purpose:		Wrapper around Windows GetOpenFileName() API
// Author:		Ralph Walden
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

#include "ttdebug.h"	// for ttASSERTS
#include "ttstr.h"		// ttCStr
#include "ttmultibtn.h"	// ttCMultiBtn

#ifndef OFN_DONTADDTORECENT
	#define OFN_DONTADDTORECENT			 0x02000000
#endif

namespace ttpriv {
	UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam);
}

class ttCFileDlg
{
public:
	ttCFileDlg(HWND hwndParent = NULL);
	~ttCFileDlg();

	// Class functions

	bool	GetOpenFileName();	// call this or the following to launch the actual dialog box
	bool	GetSaveFileName();

	char*	GetFileName() { return m_cszFileName; }	// call this after one of the above two functions has been called

	void	AddToRecent() { m_pofn->Flags &= ~OFN_DONTADDTORECENT; }
	void	SetFilter(int idResource);
	void	SetFilter(const char* pszFilters);	// separate filters with '|' character
	void	SetInitialDir(const char* pszFolder);
	void	SetInitialFileName(const char* psz) { ttASSERT(ttStrLen(psz) < MAX_PATH); m_cszFileName = psz; }
	void	ShowCreatePrompt() { m_pofn->Flags &= ~OFN_FILEMUSTEXIST; m_pofn->Flags |= OFN_CREATEPROMPT; }
	void	ShowReadOnlyBox() { m_pofn->Flags &= ~OFN_HIDEREADONLY; }
	void	UseCurrentDirectory() { m_cszCurDir.GetCWD(); m_pofn->lpstrInitialDir = m_cszCurDir; }
	void	RestoreDirectory() { m_pofn->Flags |= OFN_NOCHANGEDIR; }

	void	SetWindowRect(const RECT* prc) { ttASSERT(!IsRectEmpty(prc)); CopyRect(&m_rcPosition, prc); }
	RECT*	GetWindowRect() { return &m_rcPosition; }

	void	EnableShadeBtns(bool bEnable = true) { m_bShadeBtns = bEnable; }
	void	SetOpenIcon(UINT idIcon) { m_idOpenIcon = idIcon; }
	void	SetCancelIcon(UINT idIcon) { m_idCancelIcon = idIcon; }

	// By default, the file must exist

	void	SetFileMustExist(bool bMustExist = true) { if (bMustExist) m_pofn->Flags |= OFN_FILEMUSTEXIST; else m_pofn->Flags &= ~OFN_FILEMUSTEXIST; }
	void	AddFlags(DWORD flags) { m_pofn->Flags |= flags; }	// Adds one or more of the OFN_ flags

	OPENFILENAMEA* GetOF() { return m_pofn; }
	operator OPENFILENAMEA*() const { return m_pofn; }
	operator char*() const { return (char*) m_cszFileName; }

private:
	void FixExtension();

	friend UINT_PTR CALLBACK ttpriv::OFNHookProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Class members

protected:
	ttCStr  m_cszFileName;
	ttCStr  m_cszCurDir;
	ttCStr  m_cszFilter;
	ttCStr  m_cszSetDir;

	RECT	  m_rcPosition;

	// This is Malloc'd because the size changes depending on the version of Windows
	OPENFILENAMEA* m_pofn;

	ttCMultiBtn	m_ShadedBtns;
	UINT		m_idOpenIcon;
	UINT		m_idCancelIcon;

	bool m_bRepositionWindow;
	bool m_bShadeBtns;
};

#endif	// __TTLIB_FILEDLG_H__
