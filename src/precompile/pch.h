// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

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

// Minimum OS version is Windows 7. Any app linking to this library is expected to work normally on Windows 7 and
// up.

    #define WINVER       0x0601  // Windows 7
    #define _WIN32_WINNT 0x0600
    #define _WIN32_IE    0x0700

    #include <windows.h>

#endif

#include <stdlib.h>

#include <cassert>
#include <string>
#include <string_view>

#include "ttlibspace.h"
