/////////////////////////////////////////////////////////////////////////////
// Name:		ttCDirDlg
// Purpose:		Class for displaying a dialog to select a directory
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <shlobj.h>

#include "../include/ttstring.h"
#include "../include/ttwstring.h"	// ttCWStr

class ttCDirDlg
{
public:
	ttCDirDlg();

	// Class functions

	void SetTitle(const char* pszTitle) { m_cwszTitle = pszTitle; }
	void SetStartingDir(const char* pszDir) { m_cwszStartingDir = pszDir; }

	bool GetFolderName(HWND hwndParent = NULL);

	operator char*() const { return (char*) m_cszDirName; }

protected:
	// Class members

	ttCWStr m_cwszTitle;	// defaults to "Select a Folder"
	ttCWStr m_cwszStartingDir;
	ttCStr  m_cszDirName;
};
