// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

// Platform testing can be done in one of two ways when building on Windows. You can uncomment the PTEST line below, and
// this will cause some functionality to switch to non-Windows. You can also uncomment the wxGUI line which should turn
// even more (if not all) _WIN32 functionality, using the wxWidgets versions instead.

// #define PTEST
// #define wxGUI 1

// clang-format off
#if defined(_WIN32) && !defined(wxGUI)

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

    // Minimum OS version is Windows 7. Any app linking to this library is expected to work normally on Windows 7 and up.

    #define WINVER       0x0601  // Windows 7
    #define _WIN32_WINNT 0x0600
    #define _WIN32_IE    0x0700

    #include <windows.h>
    #include <stdlib.h>

#else

    #define wxUSE_UNICODE     1
    #define wxUSE_NO_MANIFEST 1

    // We could just include <wx/wxprec.h>, however that's a bit overkill since we only need a fraction of the header files
    // this header will pull in. Instead, we pull in just the required header files and then include individual header files
    // as needed in the actual source files. This greatly speeds up generation of the precompiled header file.

    #include "wx/defs.h"
    #include "wx/chartype.h"

    #if defined(__WINDOWS__)
        #include "wx/msw/wrapwin.h"
        #include "wx/msw/private.h"
    #endif

    #if defined(__WXMSW__)
        #include "wx/msw/wrapcctl.h"
        #include "wx/msw/wrapcdlg.h"
        #include "wx/msw/missing.h"
    #endif

    // The following are common enough that we include them here rather than in every (or most) source files

    #include <wx/string.h>

    #ifndef max
        #define max(a ,b) (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
        #define min(a, b) (((a) < (b)) ? (a) : (b))
    #endif

#endif  // defined(_WIN32)
// clang-format on

#include "../include/ttlib.h"  // Master header file for ttLibwx
