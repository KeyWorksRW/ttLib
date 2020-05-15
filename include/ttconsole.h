/////////////////////////////////////////////////////////////////////////////
// Name:      ttconsole.h
// Purpose:   Sets/restores console foreground color
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>

namespace ttlib
{
    class concolor
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

        concolor(int clr);
        ~concolor();

        void SetColor(int clr);
        void ResetColor();

    private:
#if defined(_WIN32)
        uint16_t m_defAttributes;
#endif
    };

}  // namespace ttlib
