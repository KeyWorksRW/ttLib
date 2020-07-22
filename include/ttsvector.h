/////////////////////////////////////////////////////////////////////////////
// Name:      ttcvector.h
// Purpose:   Vector of ttString strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !(__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
    #error "The contents of <ttsvector.h> are available only with C++17 or later."
#endif

/// @file
///
/// The ttlib::strVector class stores ttString strings. It inherits from std::vector, providing all of the functionality
/// of std::vector along with some functionality specific to ttString handling.

#include <vector>

#include "ttstr.h"

namespace ttlib
{
    // Note that parameters use "ttString&" instead of "const ttString&" even though the parameter is not modified. This
    // is because the compiler won't know if "foo" is a std::string_view or a const ttString& (ttString derives from
    // wxString, and wxString automatically converts "foo" to wxString&("foo") and passes the const reference).

    /// Contains a vector of ttString classes
    class strVector : public std::vector<ttString>
    {
    public:
        /// Same as find(pos, ch) but with a boolean result
        bool bfind(size_type pos, wxChar ch) const { return (at(pos).find(ch) != tt::npos); }

        /// Same as find(pos, str) but with a boolean result
        bool bfind(size_type pos, ttString& str) const { return (at(pos).find(str) != tt::npos); }

        /// Only adds the string if it doesn't already exist.
        ttString& append(ttString& str, tt::CASE checkcase = tt::CASE::exact)
        {
            if (auto index = find(0, str, checkcase); !ttlib::is_error(index))
            {
                return at(index);
            }
            return emplace_back(str);
        }

        /// Only adds the filename if it doesn't already exist. On Windows, the case of the
        /// filename is ignored when checking to see if the filename already exists.
        ttString& addfilename(ttString& filename)
        {
#if defined(_WIN32)
            return append(filename, tt::CASE::either);
#else
            return append(filename, tt::CASE::exact);
#endif  // _WIN32
        }

        /// On Windows, the case of the filename is ignored when checking to see if the
        /// filename already exists.
        bool has_filename(ttString& filename) const
        {
#if defined(_WIN32)
            return (find(0, filename, tt::CASE::either) != tt::npos);
#else
            return (find(0, filename, tt::CASE::exact) != tt::npos);
#endif  // _WIN32
        }

        /// Finds the position of the first string identical to the specified string.
        size_t find(ttString& str, tt::CASE checkcase = tt::CASE::exact) const { return find(0, str, checkcase); }

        /// Finds the position of the first string identical to the specified string.
        size_t find(size_t start, ttString& str, tt::CASE checkcase = tt::CASE::exact) const;

        /// Finds the position of the first string with specified prefix.
        size_t findprefix(ttString& prefix, tt::CASE checkcase = tt::CASE::exact) const { return findprefix(0, prefix, checkcase); }

        /// Finds the position of the first string with specified prefix.
        size_t findprefix(size_t start, ttString& prefix, tt::CASE checkcase = tt::CASE::exact) const;

        /// Finds the position of the first string containing the specified sub-string.
        size_t contains(ttString& substring, tt::CASE checkcase = tt::CASE::exact) const { return contains(0, substring, checkcase); }

        /// Finds the position of the first string containing the specified sub-string.
        size_t contains(size_t start, ttString& substring, tt::CASE checkcase = tt::CASE::exact) const;

        /// Unlike append(), this will add the string even if it already exists.
        void operator+=(ttString& str)
        {
            emplace_back(str);
        }
    };

}  // namespace ttlib
