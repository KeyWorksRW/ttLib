// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

// clang-format off
#if defined(_WIN32)

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

    #define WINVER       0x0601     // Windows 7
    #define _WIN32_WINNT 0x0600
    #define _WIN32_IE    0x0700

    #include <windows.h>
    #include <stdlib.h>

#else    // not defined(_WIN32), so wxWidgets is required

    #define wxUSE_UNICODE     1
    #define wxGUI             1
    #define wxUSE_NO_MANIFEST 1

    // We could just include <wx/wxprec.h>, however that's a bit overkill since we only need a fraction of the header files
    // this header will pull in. Instead, we pull in just the required header files and then include individual header files
    // as needed in the actual source files. This greatly speeds up generation of the precompiled header file.

    #include "wx/defs.h"    // compiler detection; includes setup.h

    #include "wx/chartype.h"

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

// Uncomment the following to do non-Windows platform testing while building on Windows. Some, but not all conditionals will check
// for this and use alternate code.

// #define PTEST
