/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::cview
// Purpose:   string_view functionality on a zero-terminated char string.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// @file
/// Provides a view of a zero-terminated char string. Includes a c_str() function and a const char*() operator
/// to make it easier to pass to functions expecting a C-style string.
///
/// Unlike std::string_view, there is no remove_suffix() function since you cannot change the length of the buffer
/// (it would no longer be zero-terminated.). There is a subview() function which returns a cview, but you can
/// only specify the starting position, not the length. You can use substr() to get a non-zero terminated
/// std::string_view.
///
/// Caution: as with std::string_view, the view is only valid as long as the string you are viewing has not
/// been modified or destroyed. This is also true of substr() and subview().

#pragma once

#define _TTLIB_CVIEW_AVAILABLE_

#include <filesystem>
#include <sstream>
#include <string_view>

#include "ttlibspace.h"

namespace ttlib
{
    class cview : public std::basic_string_view<char, std::char_traits<char>>
    {
        using bsv = std::basic_string_view<char, std::char_traits<char>>;

    public:
        // clang-format off

        cview(std::string str) : bsv(str.c_str(), str.length()) { }
        cview(std::stringstream str) : bsv(str.str().c_str(), str.str().length()) { }
        cview(const char* str, size_t len) : bsv(str, len) { }
        cview(const char* str) : bsv(str) { }

        // clang-format on

        // A string view is not guarenteed to be zero-terminated, so you can't construct from it
        cview(std::string_view str) = delete;

        /// View is zero-terminated, so c_str() can be used wherever std::string.c_str()
        /// would be used.
        constexpr const char* c_str() const noexcept { return data(); };

        /// Can be used to pass the view to a function that expects a C-style string.
        operator const char*() const noexcept { return data(); }

        /// Returns true if the sub-string is identical to the first part of the main string
        bool issameas(std::string_view str, tt::CASE checkcase = tt::CASE::exact) const;

        /// Returns true if the sub-string is identical to the first part of the main string
        bool issameprefix(std::string_view str, tt::CASE checkcase = tt::CASE::exact) const;

        /// Locates the position of a substring.
        size_t locate(std::string_view str, size_t posStart = 0, tt::CASE check = tt::CASE::exact) const;

        /// Returns true if the sub string exists
        bool contains(std::string_view sub, tt::CASE checkcase = tt::CASE::exact) const
        {
            return (locate(sub, 0, checkcase) != npos);
        }

        /// Find any one of the characters in a set. Returns offset if found, npos if not.
        ///
        /// This is equivalent to calling std::strpbrk but returns an offset instead of a pointer.
        size_t findoneof(const std::string& set) const;

        // You can't remove a suffix and still have the view zero-terminated
        constexpr void remove_suffix(size_type n) = delete;

        // Note: all view...() functions start from the beginning of the view. On success
        // they change the view and return true. On failure, the view remains unchanged.

        /// Set view to the next whitespace character
        bool viewspace() noexcept;

        /// Set view to the next non-whitespace character
        bool viewnonspace() noexcept;

        /// Set view to the next word (views the next whitespace, then the next non-whitespace
        /// after that)
        bool viewnextword() noexcept;

        /// Set view to the next numerical character
        bool viewdigit() noexcept;

        /// Set view to the next non-numerical character
        bool viewnondigit() noexcept;

        /// Set view to the extension in the current path
        bool viewextension() noexcept;

        /// Set view to the filename in the current path.
        ///
        /// A filename is any string after the last '/' (or '\' on Windows) in the current
        /// view.
        bool viewfilename() noexcept;

        /// Returns a zero-terminated view. Unlike substr(), you can only specify the starting position.
        cview subview(size_t start = 0) const noexcept
        {
            if (start >= size())
                return "";
            return cview(c_str() + start, length() - start);
        }
    };
}  // namespace ttlib
