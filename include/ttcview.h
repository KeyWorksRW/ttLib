/////////////////////////////////////////////////////////////////////////////
// Name:      tt::cview
// Purpose:   string_view functionality on a zero-terminated char string.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// @file
/// Provides a view of a zero-terminated char string.
///
/// Unlike std::string_view, there is no remove_suffix() function since you cannot change the length of the buffer
/// (it would no longer be zero-terminated.). There is a subview() function which returns tt::cview, but you can
/// only specify the starting position, not the length.
///
/// Caution: as with std::string_view, the view is only valid as long as the string you are viewing has not
/// been modified or destroyed.

#pragma once

#include <filesystem>
#include <string_view>
#include <sstream>

namespace ttlib
{
    class cview : public std::basic_string_view<char, std::char_traits<char>>
    {
        using bsv = std::basic_string_view<char, std::char_traits<char>>;
    public:
        cview(std::string str)
            : bsv(str.c_str(), str.length())
        {
        }

        cview(std::stringstream str)
            : bsv(str.str().c_str(), str.str().length())
        {
        }

        cview(const char* str, size_t len)
            : bsv(str, len)
        {
        }

        cview(const char* str)
            : bsv(str)
        {
        }

        // A string view is not guarenteed to be zero-terminated.
        cview(std::string_view str) = delete;

        // You can't remove a suffix and still have the view zero-terminated
        constexpr void remove_suffix(size_type n) = delete;

        constexpr const_pointer c_str() const noexcept { return data(); };

        // Note: all view...() functions start from the beginning of the view. On success
        // they change the view and return true. On failure, the view remains unchanged.

        /// View the next whitespace character
        bool viewspace() noexcept;

        /// VIew the next non-whitespace character
        bool viewnonspace() noexcept;

        /// View the next word (views the next whitespace, then the next non-whitespace
        /// after that)
        bool viewnextword() noexcept;

        /// View the next numerical character
        bool viewdigit() noexcept;

        /// View the next non-numerical character
        bool viewnondigit() noexcept;

        /// View the extension in the current path
        bool viewextension() noexcept;

        /// View the filename in the current path.
        ///
        /// A filename is any string after the last '/' (or '\' on Windows) in the current
        /// view.
        bool viewfilename() noexcept;

        /// Returns a zero-terminated view. Unlike substr(), you can only specify the starting position.
        ///
        /// Caution: the returned view is only valid if the original string has not been modified or
        /// destroyed.
        cview subview(size_t start = 0) const noexcept
        {
            if (start >= size())
                return "";
            return { c_str() + start };
        }

        /// Useful for passing the view to a function that expects a C-style string.
        ///
        /// Caution: assigning this to a pointer is very risky since the original string could be modified
        /// or destroyed making the pointer assignment (and this view) invalid.
        operator const char*() noexcept { return data(); }
    };
}  // namespace ttlib
