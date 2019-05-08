/////////////////////////////////////////////////////////////////////////////
// Name:		AboutDlg.h
// Purpose:		Class for handling About Box
// Author:      Ralph Walden
// Copyright:   Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef IDD_ABOUTBOX
	#include "resource.h"
#endif

#include <ttdlg.h>	// ttCDlg, ttCComboBox, ttCListBox, ttCListView

class CAboutDlg : public ttCDlg
{
public:
	CAboutDlg() : ttCDlg(IDD_ABOUTBOX) {  }
};
