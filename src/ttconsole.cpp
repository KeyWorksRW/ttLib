/////////////////////////////////////////////////////////////////////////////
// Name:      color.cpp
// Purpose:   Sets/restores console foreground color
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <cstdio>  // for printf

#include "../include/ttconsole.h"  // ttConsoleColor

#ifdef __clang__
    #pragma GCC diagnostic ignored \
        "-Wformat-security"  // format string is not a string literal (potentially insecure)
#endif

ttConsoleColor::ttConsoleColor(int clr)
{
#if defined(_WIN32) && !defined(PTEST)
    // save the current color on Windows. Not needed for ansi-enabled consoles which use reset
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    m_defAttributes = (uint16_t) csbi.wAttributes;
#endif

    SetColor(clr);
}

ttConsoleColor::~ttConsoleColor()
{
    ResetColor();
};

void ttConsoleColor::SetColor(int clr)
{
#if defined(_WIN32) && !defined(PTEST)
    auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SetConsoleTextAttribute(hConsole, (csbi.wAttributes & 0xFFF0) |
                                          (WORD) clr);  // Foreground colors take up the least significant byte

#else   // following section uses ANSI escape codes
    const char* pszClr;

    switch (clr)
    {
        case BLACK:
            pszClr = "\033[22;30m";
            break;
        case BLUE:
            pszClr = "\033[22;34m";
            break;
        case GREEN:
            pszClr = "\033[22;32m";
            break;
        case CYAN:
            pszClr = "\033[22;36m";
            break;
        case RED:
            pszClr = "\033[22;31m";
            break;
        case MAGENTA:
            pszClr = "\033[22;35m";
            break;
        case BROWN:
            pszClr = "\033[22;33m";
            break;
        case GREY:
            pszClr = "\033[22;37m";
            break;
        case DARKGREY:
            pszClr = "\033[01;30m";
            break;
        case LIGHTBLUE:
            pszClr = "\033[01;34m";
            break;
        case LIGHTGREEN:
            pszClr = "\033[01;32m";
            break;
        case LIGHTCYAN:
            pszClr = "\033[01;36m";
            break;
        case LIGHTRED:
            pszClr = "\033[01;31m";
            break;
        case LIGHTMAGENTA:
            pszClr = "\033[01;35m";
            break;
        case YELLOW:
            pszClr = "\033[01;33m";
            break;
        case WHITE:
            pszClr = "\033[01;37m";
            break;
        default:
            pszClr = "";
            break;
    }

    printf(pszClr);
#endif  // defined(_WIN32) && !defined(PTEST)
}

void ttConsoleColor::ResetColor()
{
#if defined(_WIN32) && !defined(PTEST)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD) m_defAttributes);
#else
    printf("\033[0m");
#endif
}
