/////////////////////////////////////////////////////////////////////////////
// Name:      temp.cpp
// Purpose:
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttfiledlg.h"  // ttCFileDlg -- Wrapper around Windows GetOpenFileName() API
#include "../include/ttstr.h"

ttCFileDlg foo;

foo()
{
    ttCStr csz;
    csz.GetCWD();
}
