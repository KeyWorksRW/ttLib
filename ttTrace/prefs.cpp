/////////////////////////////////////////////////////////////////////////////
// Name:		CPreferences
// Purpose:		Class for setting what message to accept/ignore
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "prefs.h"

void CPreferences::OnBegin(void)
{
    SetCheck(DLG_ID(IDCHECK_ON_TOP), uprof.fKeepOnTop);
    SetCheck(DLG_ID(IDCHECK_KEYHELP_EVENTS), uprof.fEventMessages);
    SetCheck(DLG_ID(IDCHECK_KEYHELP_PROPS), uprof.fPropertyMessages);
    SetCheck(DLG_ID(IDCHECK_KEYHELP_SCRIPTS), uprof.fScriptMessages);
}

void CPreferences::OnOK(void)
{
    uprof.fKeepOnTop = GetCheck(IDCHECK_ON_TOP);
    uprof.fEventMessages = GetCheck(IDCHECK_KEYHELP_EVENTS);
    uprof.fPropertyMessages = GetCheck(IDCHECK_KEYHELP_PROPS);
    uprof.fScriptMessages = GetCheck(IDCHECK_KEYHELP_SCRIPTS);
}
