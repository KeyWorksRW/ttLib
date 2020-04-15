/////////////////////////////////////////////////////////////////////////////
// Name:      color.cpp
// Purpose:   Sets/restores console foreground color
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #include <cstdlib>
#endif

#include "ttconsole.h"  // concolor

using namespace ttlib;

concolor::concolor(int clr)
{
#if defined(_WIN32)
    // save the current color on Windows. Not needed for ansi-enabled consoles which use reset
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    m_defAttributes = (uint16_t) csbi.wAttributes;
#endif

    SetColor(clr);
}

concolor::~concolor()
{
    ResetColor();
};

void concolor::SetColor(int clr)
{
#if defined(_WIN32)
    auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SetConsoleTextAttribute(hConsole, (csbi.wAttributes & 0xFFF0) | (WORD) clr);  // Foreground colors take up the least significant byte

#else   // following section uses ANSI escape codes
    const char* color;

    switch (clr)
    {
        case BLACK:
            color = "\033[22;30m";
            break;
        case BLUE:
            color = "\033[22;34m";
            break;
        case GREEN:
            color = "\033[22;32m";
            break;
        case CYAN:
            color = "\033[22;36m";
            break;
        case RED:
            color = "\033[22;31m";
            break;
        case MAGENTA:
            color = "\033[22;35m";
            break;
        case BROWN:
            color = "\033[22;33m";
            break;
        case GREY:
            color = "\033[22;37m";
            break;
        case DARKGREY:
            color = "\033[01;30m";
            break;
        case LIGHTBLUE:
            color = "\033[01;34m";
            break;
        case LIGHTGREEN:
            color = "\033[01;32m";
            break;
        case LIGHTCYAN:
            color = "\033[01;36m";
            break;
        case LIGHTRED:
            color = "\033[01;31m";
            break;
        case LIGHTMAGENTA:
            color = "\033[01;35m";
            break;
        case YELLOW:
            color = "\033[01;33m";
            break;
        case WHITE:
            color = "\033[01;37m";
            break;
        default:
            color = "";
            break;
    }

    std::cout << color;
#endif  // !defined(_WIN32)
}

void concolor::ResetColor()
{
#if defined(_WIN32) && !defined(PTEST)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD) m_defAttributes);
#else
    std::cout << "\033[0m";
#endif
}
