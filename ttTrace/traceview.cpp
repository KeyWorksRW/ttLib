/////////////////////////////////////////////////////////////////////////////
// Name:        CTraceView
// Purpose:     Class for
// Author:      Ralph Walden
// Copyright:   Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"

#include "traceview.h"  // CTraceView

BOOL CTraceView::PreTranslateMessage(MSG* /* pMsg */)
{
    return FALSE;
}

LRESULT CTraceView::OnCreate(CREATESTRUCT* /* pcs */)
{
    HDC hdc = CreateCompatibleDC(NULL);
    SetMapMode(hdc, MM_TEXT);

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(LOGFONT));

    int ratio = MulDiv(GetDeviceCaps(hdc, LOGPIXELSY), 100, 72);
    lf.lfHeight = MulDiv(10, ratio, 100);
    if ((10 * ratio) % 100 >= 50)
        lf.lfHeight++;

    lf.lfHeight = -lf.lfHeight;
    ttStrCpy(lf.lfFaceName, LF_FACESIZE, "MS Shell Dlg");

    HFONT hfont = CreateFontIndirect(&lf);
    DeleteDC(hdc);

    PostMessage(WM_SETFONT, (WPARAM) hfont);

    return 0;
}
