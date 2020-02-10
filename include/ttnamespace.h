/////////////////////////////////////////////////////////////////////////////
// Name:      ttnamespace.h
// Purpose:   Contains the tt namespace functions common to all tt libraries
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

// clang-format off
#ifndef _TTNAMESPACE_H_GUARD_
#define _TTNAMESPACE_H_GUARD_

#include <string>
#include <string_view>

#include <filesystem>  // directory_entry
// clang-format on

namespace tt
{
    /// Only valid for ANSI or UTF8 characters
    inline bool isdigit(char ch) { return ((ch >= '0' && ch <= '9') || ch == '-'); }

    /// Is ch the start of a utf8 sequence?
    inline bool isutf8(char ch) { return ((ch & 0xC0) != 0x80); }

    /// Returns true if character is a space, tab, eol or form feed character.
    inline bool iswhitespace(char ch) { return (ch != 0 && std::strchr(" \t\r\n\f", ch)) ? true : false; };

    /// Returns true if strings are identical
    bool issameas(std::string_view str1, std::string_view str2, bool CaseSensitive = true);

    /// Same as compare only it returns a boolean instead of the difference.
    inline bool issamestr(std::string_view str1, std::string_view str2) { return (str1.compare(str2) == 0); }

    /// Same as issamestr only case insensitive comparison of ASCII characters
    bool issamestri(std::string_view str1, std::string_view str2);

    /// Returns true if the sub-string is identical to the first part of the main string
    bool issamesubstr(std::string_view strMain, std::string_view strSub);

    /// Case-insensitive sub string comparison
    bool issamesubstri(std::string_view strMain, std::string_view strSub);

    /// Return a view to the portion of the string beginning with the sub string.
    ///
    /// Return view is empty if substring is not found.
    std::string_view findstr(std::string_view main, std::string_view sub, bool CaseSensitive = true);

    /// Returns the position of sub within main, or tt::npos if not found.
    size_t findstr_pos(std::string_view main, std::string_view sub, bool CaseSensitive = true);

    /// Returns true if the sub string exists withing the main string.
    ///
    /// Same as findstr but with a boolean return instead of a string_view.
    bool contains(std::string_view main, std::string_view sub, bool CaseSensitive = true);

    /// Return a view to the portion of the string beginning with the sub string.
    ///
    /// Return view is empty if substring is not found.
    std::string_view strstr(std::string_view strMain, std::string_view strSub);

    /// Case-insensitive search for a sub string.
    ///
    /// Return view is empty if substring is not found.
    std::string_view strstri(std::string_view strMain, std::string_view strSub);

    /// Returns a pointer to the next character in a UTF8 string.
    const char* nextchar(const char* psz);

    /// Returns view to the next whitespace character. View is empty if there are no more
    /// whitespaces.
    std::string_view findspace(std::string_view str);

    /// Returns position of next whitespace character or tt::npos if not found.
    size_t findspace_pos(std::string_view str);

    /// Returns view to the next non-whitespace character. View is empty if there are no
    /// non-whitespace characters.
    std::string_view findnonspace(std::string_view str);

    /// Returns position of next non-whitespace character or tt::npos if not found.
    size_t findnonspace_pos(std::string_view str);

    /// Equivalent to findnonspace(findspace(str)).
    std::string_view stepover(std::string_view str);

    /// Equivalent to findnonspace(findspace(str)) returning the position or tt::npos.
    size_t stepover_pos(std::string_view str);

    /// Generates hash of string using djb2 hash algorithm
    size_t gethash(std::string_view str);

    /// Converts a string into an integer.
    ///
    /// If string begins with '0x' it is assumed to be hexadecimal and is converted.
    /// String may begin with a '-' or '+' to indicate the sign of the integer.
    int atoi(std::string_view str);

    /// Return a view to a filename's extension. View is empty if there is no extension.
    std::string_view findext(std::string_view str);

    /// Determines whether the character at pos is part of a filename. This will
    /// differentiate between '.' being used as part of a path (. for current directory, or ..
    /// for relative directory) versus being the leading character in a file.
    bool isvalidfilechar(std::string_view str, size_t pos);

    /// Converts all backslashes in a filename to forward slashes.
    ///
    /// Note: Windows handles paths that use forward slashes, so backslashes are normally
    /// unnecessary.
    void backslashestoforward(std::string& str);

    /// Performs a case-insensitive check to see if a directory entry is a filename and
    /// contains the specified extension.
    bool hasextension(std::filesystem::directory_entry name, std::string_view extension);

    /// Confirms newdir exists and is a directory and then changes to that directory.
    ///
    /// Returns false only if newdir is not an existing directory. Throws filesystem_error
    /// if the directory is valid but could not be changed to.
    bool ChangeDir(std::string_view newdir);

    bool dirExists(std::string_view dir);
    bool fileExists(std::string_view filename);
}  // namespace tt

#endif  // _TTNAMESPACE_H_GUARD_
