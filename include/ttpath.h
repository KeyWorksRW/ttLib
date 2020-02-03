/////////////////////////////////////////////////////////////////////////////
// Name:      ttpath.h
// Purpose:   Contains functions for working with filesystem::path and filesystem::directory
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// @file
/// Ideally, std::filesystem::path or boost::filesystem::path is used for parsing paths. The functions here add a
/// bit of functionality for those times when using a path container is not an option.

#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace tt
{
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
