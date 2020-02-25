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

    #define WINVER 0x0A00  // Windows 10

    #include <windows.h>
    #include <stdlib.h>
#endif  // defined(_WIN32)

// This prevents deprecated warnings -- we already know they are deprecated, the warning
// is for code that calls the library.
#define TTLIB_INTERNAL_BUILD

// #include "../include/ttlibwin.h"  // Master header file for ttLibwin.lib
