/////////////////////////////////////////////////////////////////////////////
// Name:		Prefs.h
// Purpose:		Class for setting what message to accept/ignore
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef IDDLG_PREFERENCES
#include "resource.h"
#endif

#include <ttdlg.h>  // ttCDlg, ttCComboBox, ttCListBox, ttCListView

class CPreferences : public ttCDlg
{
public:
    CPreferences()
        : ttCDlg(IDDLG_PREFERENCES)
    {
    }

    BEGIN_TTMSG_MAP()
    END_TTMSG_MAP()

    // Class functions

    void OnBegin(void);
    void OnOK(void);

protected:
};
