// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

// reduce the number of Windows header files pulled in
#define NOATOM
#define NOCOMM
#define NODRIVERS
#define NOENHMETAFILE
#define NOEXTDEVMODEPROPSHEET
#define NOIME
#define NOKANJI
#define NOLOGERROR
#define NOMCX
#define NOPROFILER
#define NOSCALABLEFONT
#define NOSERVICE
#define NOSOUND
#define NOWINDOWSX
#define OEMRESOURCE

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0A00  // Windows 10

#define wxUSE_UNICODE     1
#define wxUSE_NO_MANIFEST 1  // This is required for compiling using CLANG 8 and earlier

#include <wx/defs.h>

#define _WX_MISSING_H_  // prevent loading <wx/missing.h> which conflicts with <urlmon.h>
#include "wx/msw/wrapcctl.h"

#if wxUSE_COMMON_DIALOGS
    #include <commdlg.h>
#endif

// Ensure that _DEBUG is defined in non-release builds
#if !defined(NDEBUG) && !defined(_DEBUG)
    #define _DEBUG
#endif

#include <ttlibspace.h>

#undef TT_ASSERT
