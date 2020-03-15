/////////////////////////////////////////////////////////////////////////////
// Name:      include\ttcvector.h
// Purpose:   Vector of ttlib::cstr strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

/// @file
///
/// The ttlib::cstrVector class stores ttlib::cstr (zero-terminated char containter class) strings. It inherits
/// from std::vector, providing all of the functionality of std::vector along with some functionality specific to
/// string handling. most places where std::string<char> is used.

#include <vector>

#include "ttcstr.h"

namespace ttlib
{
    /// Contains a vector of cstr classes with some additional functionality such
    /// as only adding a string if it doesn't already exist.
    class cstrVector : public std::vector<ttlib::cstr>
    {
    public:
        /// Same as find(pos, ch) but with a boolean result
        bool bfind(size_type pos, char ch) const { return (at(pos).find(ch) != tt::npos); }

        /// Same as find(pos, str) but with a boolean result
        bool bfind(size_type pos, std::string_view str) const { return (at(pos).find(str) != tt::npos); }

        /// Only appends the string if it doesn't already exist.
        ///
        /// Returns true if the string was added, false if it already existed.
        bool append(std::string_view str, tt::CASE checkcase = tt::CASE::exact)
        {
            if (find(0, str, checkcase) != tt::npos)
            {
                return false;
            }
            push_back(str);
            return true;
        }

        /// Only adds the filename if it doesn't already exist. On Windows, the case of the
        /// filename is ignored when checking to see if the filename already exists.
        ///
        /// Returns true if the file was added, false if it already existed.
        bool addfilename(std::string_view filename)
        {
            for (auto iter : *this)
            {
#if defined(_WIN32)
                if (iter.comparei(filename) == 0)
                    return false;
#else
                if (iter.compare(filename) == 0)
                    return false;
#endif  // _WIN32
            }
            push_back(filename);
            return true;
        }

        bool hasFilename(std::string_view filename)
        {
#if defined(_WIN32)
            return (find(0, filename, tt::CASE::either) != tt::npos);
#else
            return (find(0, filename, tt::CASE::exact) != tt::npos);
#endif  // _WIN32
        }

        /// Finds the position of the first string identical to the specified string.
        size_t find(size_t start, std::string_view str, tt::CASE checkcase = tt::CASE::exact) const;

        /// Finds the position of the first string with specified prefix.
        size_t findprefix(size_t start, std::string_view prefix, tt::CASE checkcase = tt::CASE::exact) const;

        /// Finds the position of the first string containing the specified sub-string.
        size_t contains(size_t start, std::string_view substring,
                        tt::CASE checkcase = tt::CASE::exact) const;

        /// Unlike append(), this will add the string even if it already exists.
        void operator+=(std::string_view str) { push_back(str); }
    };

}  // namespace ttlib
