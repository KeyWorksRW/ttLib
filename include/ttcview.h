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
/// (it would no longer be zero-terminated.). There is a substr() function, but it returns a cview rather than the
/// expected tt::cview. There is a subview() function which returns tt::cview, but you can only specify the
/// starting position, not the length.

#pragma once

#include <filesystem>
#include <string_view>

namespace tt
{
    template<class char_t, class Traits = std::char_traits<char>>
    class basic_cview : private std::basic_string_view<char, Traits>
    {
    public:
        using string_view_base = std::basic_string_view<char, Traits>;

        using typename string_view_base::const_iterator;
        using typename string_view_base::const_pointer;
        using typename string_view_base::const_reference;
        using typename string_view_base::const_reverse_iterator;
        using typename string_view_base::difference_type;
        using typename string_view_base::iterator;
        using typename string_view_base::pointer;
        using typename string_view_base::reference;
        using typename string_view_base::reverse_iterator;
        using typename string_view_base::size_type;
        using typename string_view_base::traits_type;
        using typename string_view_base::value_type;

        /////////// begin additions to string_view /////////////////
        using cview_base = basic_cview<char, Traits>;

        constexpr const_pointer c_str() const noexcept { return data(); };

        // Note: all view...() functions start from the beginning of the view. On success
        // they change the view and return true. On failure, the view remains unchanged.

        /// View the next whitespace character
        bool viewspace();

        /// VIew the next non-whitespace character
        bool viewnonspace();

        /// View the next word (views the next whitespace, then the next non-whitespace
        /// after that)
        bool viewnextword();

        /// View the next numerical character
        bool viewdigit();

        /// View the next non-numerical character
        bool viewnondigit();

        /// View the extension in the current path
        bool viewextension();

        /// View the filename in the current path.
        ///
        /// A filename is any string after the last '/' (or '\' on Windows) in the current
        /// view.
        bool viewfilename();

        /// Returns a zero-terminated view. Unlike substr(), you can only specify the starting position.
        ///
        /// Caution: the returned view is only valid if the original string has not been modified or
        /// destroyed.
        constexpr cview_base subview(size_t start = 0) const noexcept
        {
            if (start >= size())
                return {};
            return { c_str() + start, size() - start };
        }

        /// Useful for passing the view to a function that expects a C-style string.
        ///
        /// Caustion: assigning this to a pointer is very risky since the original string could be modified
        /// or destroyed making the pointer assignment (and this view) invalid.
        operator const char*() { return data(); }

        /////////// end additions to string_view /////////////////

        using string_view_base::npos;

        constexpr basic_cview() noexcept = default;

        constexpr basic_cview(basic_cview const&) noexcept = default;
        constexpr basic_cview& operator=(basic_cview const&) noexcept = default;

        constexpr basic_cview(basic_cview&&) noexcept = default;
        constexpr basic_cview& operator=(basic_cview&&) noexcept = default;

        basic_cview(char const* s)
            : string_view_base{ s }
        {
        }

        basic_cview(char const* s, size_type len)
            : string_view_base{ s, len }
        {
        }

        basic_cview(std::string const& s)
            : string_view_base{ s }
        {
        }

        using string_view_base::begin;
        using string_view_base::cbegin;

        using string_view_base::cend;
        using string_view_base::end;

        using string_view_base::crbegin;
        using string_view_base::rbegin;

        using string_view_base::crend;
        using string_view_base::rend;

        using string_view_base::operator[];

        using string_view_base::at;
        using string_view_base::back;
        using string_view_base::data;
        using string_view_base::front;

        using string_view_base::length;
        using string_view_base::size;

        using string_view_base::empty;
        using string_view_base::max_size;

        using string_view_base::remove_prefix;

        // Cannot support remove_suffix() since the range end would no longer be followed by a zero.

        using string_view_base::swap;

        using string_view_base::copy;

        using string_view_base::substr;

        using string_view_base::compare;

        using string_view_base::find;
        using string_view_base::find_first_not_of;
        using string_view_base::find_first_of;
        using string_view_base::find_last_not_of;
        using string_view_base::find_last_of;
        using string_view_base::rfind;

// CAUTION: when compiling with MSVC, you must set the "/Zc:__cplusplus" flag to get these

// TODO: need to replace the value once C++20 defines it.
#if __cplusplus > 201703L
        using string_view_base::ends_with;
        using string_view_base::starts_with;
#endif

        friend inline std::ostream& operator<<(std::ostream& ostr, basic_cview cview)
        {
            ostr << cview.c_str();
            return ostr;
        }
    };

    using cview = basic_cview<char>;

}  // namespace tt
