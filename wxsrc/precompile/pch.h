// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

#if defined(_WIN32)
    #define wxMSVC_VERSION_AUTO
#endif

#define wxUSE_UNICODE     1
#define wxUSE_NO_MANIFEST 1
#define wxUSE_INTL        1

#include "wx/defs.h"

#if defined(__WINDOWS__)
    #include "wx/msw/private.h"
#endif

#include <ttlib.h>
