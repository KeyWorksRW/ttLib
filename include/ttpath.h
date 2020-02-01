/////////////////////////////////////////////////////////////////////////////
// Name:      ttpath.h
// Purpose:   Contains filename parsing functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// @file
/// Ideally, std::filesystem::path or boost::filesystem::path is used for parsing paths. The functions here add a
/// bit of functionality for those times when using a path container is not an option.

#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace tt
{
    /// Return a view to a filename's extension. View is empty if there is no extension.
    std::string_view findext(std::string_view str)
    {
        auto pos = str.rfind('.');
        if (pos == std::string_view::npos)
            return {};
        else if (pos + 1 >= str.length())  // . by itself is a folder
            return {};
        else if (pos < 2 && (str.at(pos + 1) == '.'))
            return {};

        return str.substr(pos);
    }

    /// Determines whether that character as pos is part of a filename. This will
    /// differentiate between '.' being used as part of a path (. for current directory, or ..
    /// for relative directory) versus being the leading character in a file.
    bool isvalidfilechar(std::string_view str, size_t pos)
    {
        if (str.empty() || pos > str.length())
            return false;

        switch (str.at(pos))
        {
            case '.':
                if (pos + 1 >= str.length())  // . by itself is a folder
                    return false;
                if (pos < 2 && (str.at(pos + 1) == '.'))
                    return false;  // ".." is a folder
                return true;

            case '<':
            case '>':
            case ':':
            case '/':
            case '\\':
            case '|':
            case '?':
            case '*':
            case 0:
                return false;
        }
        return true;
    }

    /// Converts all backslashes in a filename to forward slashes.
    ///
    /// Note: Windows handles paths that use forward slashes, so backslashes are normally
    /// unnecessary.
    void backslashestoforward(std::string& str)
    {
        for (auto pos = str.find('\\'); pos != std::string::npos; pos = str.find('\\'))
        {
            str.replace(pos, 1, "/");
        }
    }

}  // namespace tt
