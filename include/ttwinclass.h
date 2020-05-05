/////////////////////////////////////////////////////////////////////////////
// Name:      ttwinclass.h
// Purpose:   Contains structures that add methods to some Windows structures
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// These header-only structures provides some additional methods for working with some of the Windows structures. The
/// structures can also be used in a non-Windows build should you need them on other platforms.

#pragma once

namespace tt
{
#if defined(_WIN32)
    struct WINPOINT : POINT
    {
#else
    struct WINPOINT
    {
        int32_t x;
        int32_t y;
#endif  // _WIN32
        inline void clear() noexcept { x = y = 0; }

        template<typename T>
        inline void offset(T xoff, T yoff) noexcept
        {
            x += (int32_t) xoff;
            y += (int32_t) yoff;
        }

        bool operator==(WINPOINT pt) const noexcept { return (x == pt.x && y == pt.y); }
        bool operator!=(WINPOINT pt) const noexcept { return !(x == pt.x && y == pt.y); }
    };

#if defined(_WIN32)
    struct WINRECT : RECT
    {
#else
    struct WINRECT
    {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
#endif  // _WIN32

        WINRECT(const WINRECT& rcSrc) { Copy(rcSrc); }
        WINRECT()
        {
            left = 0;
            top = 0;
            right = 0;
            bottom = 0;
        }

        template<typename T>
        WINRECT(T l, T t, T r, T b)
        {
            left = l;
            top = t;
            right = r;
            bottom = b;
        }

        inline void Copy(const WINRECT& rcSrc) noexcept
        {
            left = rcSrc.left;
            right = rcSrc.right;
            top = rcSrc.top;
            bottom = rcSrc.bottom;
        }

        inline int Width() const { return std::abs(right - left); }
        inline int Height() const { return std::abs(bottom - top); }

        inline bool isPosInRect(WINPOINT pt) const noexcept { return (pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom); }

        template<typename T>
        inline bool isPosInRect(T xPos, T yPos) const noexcept
        {
            return ((int32_t) xPos >= left && (int32_t) xPos <= right && (int32_t) yPos >= top && (int32_t) yPos <= bottom);
        }

        inline void clear() { left = top = right = bottom = 0; }
    };

}  // namespace tt
