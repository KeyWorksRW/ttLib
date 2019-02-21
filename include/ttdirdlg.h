/////////////////////////////////////////////////////////////////////////////
// Name:		ttDirDlg
// Purpose:		Class for displaying a dialog to select a directory
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <shlobj.h>

#include "../../ttLib/include/ttwstring.h"	// ttWString

class ttDirDlg
{
public:
	ttDirDlg();

	// Class functions

	void SetTitle(const char* pszTitle) { m_cwszTitle = pszTitle; }
	void SetStartingDir(const char* pszDir) { m_cwszStartingDir = pszDir; }

	bool GetFolderName(HWND hwndParent = NULL);

	operator char*() const { return (char*) m_cszDirName; }

protected:
	// Class members

	ttWString m_cwszTitle;	// defaults to "Select a Folder"
	ttWString m_cwszStartingDir;
	ttString  m_cszDirName;
};
