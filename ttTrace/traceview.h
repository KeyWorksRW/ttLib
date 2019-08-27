/////////////////////////////////////////////////////////////////////////////
// Name:		CRichEditCtrl
// Purpose:		Class for viewing a Rich Edit control
// Author:      Ralph Walden
// Copyright:   Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ttwtlmsgs.h>  // Macros for mapping Windows messages to functions

class CTraceView : public CWindowImpl<CTraceView, CRichEditCtrl>
{
public:
    DECLARE_WND_SUPERCLASS(NULL, CRichEditCtrl::GetWndClassName())

    BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(CTraceView)
        MSG_WM_CREATE(OnCreate)
    END_MSG_MAP()

    // Class functions

protected:
    // Msg handlers

    LRESULT OnCreate(CREATESTRUCT* /* pcs */);
};
