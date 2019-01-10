// This header file is used to create a pre-compiled header for use in the entire project
// This header file should only be use to build a library compatible with Microsoft Windows

#pragma once

// Okay to #define version numbers to require Vista on, but don't #define anything else -- need to be as
// clean as we can to detect any conflicts with Windows header files.

#define WINVER 		 	0x0600		// Windows Vista and Windows Server 2008 (use 0x0601 for Windows 7).
#define _WIN32_WINNT 	0x0600
#define _WIN32_IE 	 	0x0700

#include <windows.h>
#include <stdint.h>
#include <inttypes.h>

#include "../include/ttlib.h"

namespace ttpriv {
	const char CH_OPEN_PAREN =  '(';
	const char CH_CLOSE_PAREN = ')';
	const char CH_COLON =		':';
	const char CH_SEMICOLON =	';';
	const char CH_START_QUOTE = '`';
	const char CH_SQUOTE =		'\'';
	const char CH_END_QUOTE =	'\'';
	const char CH_QUOTE =		'"';
	const char CH_EQUAL =		'=';
	const char CH_SPACE =		' ';
	const char CH_COMMA =		',';
	const char CH_LEFT_BRACKET = '[';
	const char CH_RIGHT_BRACKET = ']';
	const char CH_TAB =			'\t';
	const char CH_BACKSLASH =	 '\\';
	const char CH_FORWARDSLASH = '/';
} // end of ttpriv namespace
