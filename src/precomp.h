// This header file is used to create a pre-compiled header for use in the entire project
// This header file should only be use to build a library compatible with Microsoft Windows

#pragma once

// Okay to #define version numbers to require Vista on, but don't #define anything else -- need to be as
// clean as we can to detect any conflicts with Windows header files.

#define WINVER 		 	0x0600		// Windows Vista and Windows Server 2008 (use 0x0601 for Windows 7).
#define _WIN32_WINNT 	0x0600
#define _WIN32_IE 	 	0x0700

#define _RICHEDIT_VER	0x0300

#include <windows.h>
#include <stdint.h>
#include <inttypes.h>

#include "../include/ttlib.h"

// Don't include any other TTLib headers -- need to verify that each ttLib source module #includes any header files it needs
