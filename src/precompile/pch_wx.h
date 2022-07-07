// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

#if defined(_WIN32)

// Reduce the number of Windows files that get read

    #define NOATOM
    #define NOCOMM
    #define NODRIVERS
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
    #define NOENHMETAFILE

    #define OEMRESOURCE
    #define _CRT_SECURE_NO_WARNINGS

    #define STRICT
    #define WIN32_LEAN_AND_MEAN

#endif  // _WIN32

#define wxUSE_UNICODE     1
#define wxUSE_NO_MANIFEST 1  // This is required for compiling using CLANG 8 and earlier

// This *IS* a legitimate warning, however while wxWidgets 3.1.15 has made some progress, there are still header files that
// do this, and of course we can't assume the user is compiling with a version of wxWidgets where it has been fixed.

#if (wxMAJOR_VERSION < 4) && (wxMINOR_VERSION < 2) && (wxRELEASE_NUMBER < 16)
    #if (__cplusplus > 201703L || (defined(_MSVC_LANG) && _MSVC_LANG > 201703L))
        #ifdef _MSC_VER
            #pragma warning(disable : 5054)  // operator '|': deprecated between enumerations of different types
        #endif
    #endif  // C++20 or later
#endif

#ifdef _MSC_VER
    #pragma warning(push)
#endif

#define _WX_WXCRT_H_  // don't import <wx/wxcrt.h>

#include <wx/defs.h>

#if defined(__WINDOWS__)

    #define _WX_MISSING_H_  // prevent loading <wx/missing.h> which conflicts with <urlmon.h>
    #include "wx/msw/wrapcctl.h"

    #include <commdlg.h>
#endif

#include <wx/string.h>  // wxString class

// This is from <wx/wxcrt.h>
template <typename T>
inline int wxStrcmp(const wxString& s1, const T& s2)
{
    return s1.compare(s2);
}

#include <wx/msgdlg.h>

#ifdef _MSC_VER
    #pragma warning(pop)
#endif

// Without this, a huge number of #included wxWidgets header files will generate the warning
#pragma warning(disable : 4251)  // needs to have dll-interface to be used by clients of class

// Ensure that _DEBUG is defined in non-release builds
#if !defined(NDEBUG) && !defined(_DEBUG)
    #define _DEBUG
#endif

#include <stdexcept>
#include <string>
#include <string_view>

#include "ttlibspace.h"

#if defined(_WIN32)
    #include <ttdebug.h>  // ttASSERT macros
#endif

#include <ttcstr.h>   // Classes for handling zero-terminated char strings.
#include <ttcview.h>  // cview -- string_view functionality on a zero-terminated char string.
#include <ttstr.h>    // ttString -- wxString with additional methods similar to ttlib::cstr

#undef TT_ASSERT
