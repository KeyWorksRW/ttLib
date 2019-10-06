/////////////////////////////////////////////////////////////////////////////
// Name:      color.h
// Purpose:   Sets/restores console foreground color
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>

class ttConsoleColor
{
public:
    enum
    {
        BLACK,
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENTA,
        BROWN,
        GREY,
        DARKGREY,
        LIGHTBLUE,
        LIGHTGREEN,
        LIGHTCYAN,
        LIGHTRED,
        LIGHTMAGENTA,
        YELLOW,
        WHITE
    };

    ttConsoleColor(int clr);
    ~ttConsoleColor();

    void SetColor(int clr);
    void ResetColor();

private:
    uint16_t m_defAttributes;
};
