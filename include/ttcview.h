/////////////////////////////////////////////////////////////////////////////
// Name:      ttcview.h
// Purpose:   string_view functionality on a zero-terminated char string.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
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

#if !(__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
    #error "The contents of <ttcview.h> are available only with C++17 or later."
#endif

#include "ttlibspace.h"  // ttlib namespace functions and declarations

#define _TTLIB_CVIEW_AVAILABLE_

#include <filesystem>
#include <sstream>
#include <string_view>

namespace ttlib
{
    class cview : public std::basic_string_view<char, std::char_traits<char>>
    {
        using bsv = std::basic_string_view<char, std::char_traits<char>>;

    public:
        cview(const std::string& str) : bsv(str.c_str(), str.length()) {}
        cview(const char* str, size_t len) : bsv(str, len) {}
        cview(const char* str) : bsv(str) {}

        // A string view is not guarenteed to be zero-terminated, so you can't construct from it
        cview(std::string_view str) = delete;

        /// cview is zero-terminated, so c_str() can be used wherever std::string.c_str()
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
        bool contains(std::string_view sub, tt::CASE checkcase = tt::CASE::exact) const { return (locate(sub, 0, checkcase) != npos); }

        /// Returns true if any string in the iteration list appears somewhere in the the main string.
        template<class iterT>
        bool strContains(iterT iter, tt::CASE checkcase = tt::CASE::exact)
        {
            for (auto& strIter: iter)
            {
                if (contains(strIter, checkcase))
                    return true;
            }
            return false;
        }

        /// Find any one of the characters in a set. Returns offset if found, npos if not.
        ///
        /// This is equivalent to calling std::strpbrk but returns an offset instead of a pointer.
        size_t findoneof(const std::string& set) const;

        /// Returns offset to the next whitespace character starting with pos. Returns npos if
        /// there are no more whitespaces.
        ///
        /// A whitespace character is a space, tab, eol or form feed character.
        size_t findspace(size_t start = 0) const;

        /// Returns offset to the next non-whitespace character starting with pos. Returns npos
        /// if there are no more non-whitespace characters.
        ///
        /// A whitespace character is a space, tab, eol or form feed character.
        size_t findnonspace(size_t start = 0) const;

        /// Returns an offset to the next word -- i.e., find the first non-whitedspace character
        /// after the next whitespace character.
        ///
        /// Equivalent to findnonspace(findspace(start)).
        size_t stepover(size_t start = 0) const;

        // You can't remove a suffix and still have the view zero-terminated
        constexpr void remove_suffix(size_type n) = delete;

        /// Returns true if current filename contains the specified case-insensitive extension.
        bool hasExtension(std::string_view ext) const { return ttlib::issameas(extension(), ext, tt::CASE::either); }

        /// Returns true if current filename contains the specified case-insensitive file name.
        bool hasFilename(std::string_view name) const { return ttlib::issameas(filename(), name, tt::CASE::either); }

        /// Returns a view to the current extension. View is empty if there is no extension.
        ///
        /// Caution: view is only valid until cstr is modified or destroyed.
        ttlib::cview extension() const noexcept;

        /// Returns a view to the current filename. View is empty if there is no filename.
        ///
        /// Caution: view is only valid until cstr is modified or destroyed.
        ttlib::cview filename() const noexcept;

        /// Returns true if the current string refers to an existing file.
        bool fileExists() const;

        /// Returns true if the current string refers to an existing directory.
        bool dirExists() const;

        /// Returns a zero-terminated view. Unlike substr(), you can only specify the starting position.
        cview subview(size_t start = 0) const
        {
            if (start > length())
                start = length();
            return cview(c_str() + start, length() - start);
        }

        // All of the following view_() functions will return subview(length()) if the specified character cannot be
        // found, or the start position is out of range (including start == npos).

        cview view_space(size_t start = 0) const { return subview(findspace(start)); }
        cview view_nonspace(size_t start = 0) const { return subview(findnonspace(start)); }
        cview view_stepover(size_t start = 0) const { return subview(stepover(start)); }
        cview view_digit(size_t start = 0) const;
        cview view_nondigit(size_t start = 0) const;

        /// Generates hash of current string using djb2 hash algorithm
        size_t gethash() const noexcept;

        /////////////////////////////////////////////////////////////////////////////////
        // Note: all view...() functions start from the beginning of the view. On success
        // they change the view and return true. On failure, the view remains unchanged.
        /////////////////////////////////////////////////////////////////////////////////

        /// Set view to the next whitespace character
        [[deprecated ("use view_space")]] bool viewspace() noexcept;

        /// Set view to the next non-whitespace character
        [[deprecated ("use view_nonspace")]] bool viewnonspace() noexcept;

        /// Set view to the next word (views the next whitespace, then the next non-whitespace
        /// after that)
        [[deprecated ("use view_stepover")]] bool viewnextword() noexcept;

        /// Set view to the next numerical character
        [[deprecated ("use view_digit")]] bool viewdigit() noexcept;

        /// Set view to the next non-numerical character
        [[deprecated ("use view_nondigit")]] bool viewnondigit() noexcept;

        /// Set view to the extension in the current path
        [[deprecated ("use extension")]] bool viewextension() noexcept;

        /// Set view to the filename in the current path.
        ///
        /// A filename is any string after the last '/' (or '\' on Windows) in the current
        /// view.
        [[deprecated ("use filename")]] bool viewfilename() noexcept;
    };
}  // namespace ttlib
