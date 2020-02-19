/////////////////////////////////////////////////////////////////////////////
// Name:      ttTextFile
// Purpose:   Similar to wxTextFile, but uses UTF8 strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string_view>
#include <vector>

#include "ttstring.h"

/// This reads a line-oriented file into a vector of ttString (std::string)
/// allowing you to modify, append, or delete individual lines. If you write
/// the file each line written is appended with a single '\n' character.
class ttTextFile : public std::vector<ttString>
{
public:
    /// Reads a line-oriented file and converts each line into a std::string.
    bool ReadFile(std::string_view filename);

    /// Reads a string as if it was a file (see ReadFile).
    void ReadString(std::string_view str);

    /// Writes each line to the file adding a '\n' to the end of the line.
    bool WriteFile(std::string_view filename) const;

    /// Searches every line to see if it contains the sub-string.
    ///
    /// start is the zero-based offset to the line to start searching.
    size_t FindLineContaining(std::string_view str, size_t start = 0, bool CaseSensitive = true) const;

protected:

    // Converts lines into a vector of std::string members. Lines can end with \n, \r, or \r\n.
    void ParseLines(std::string_view str);
};

/// Similar to ttTextFile, only the entire file is stored as a std::string,
/// and the vector contains a std::string_view for each line. This is a faster
/// way to read the file if you don't need to modify the existing contents
class ttViewFile : public std::vector<std::string_view>
{
public:
    /// Reads a line-oriented file and converts each line into a std::string.
    bool ReadFile(std::string_view filename);

    /// Reads a string as if it was a file (see ReadFile).
    void ReadString(std::string_view str);

    /// Writes each line to the file adding a '\n' to the end of the line.
    bool WriteFile(std::string_view filename) const;

    /// Returns the string storing the entire file. If you change this string, all
    /// the string_view vector entries will be invalid!
    const std::string& GetBuffer() const { return m_buffer; }

    /// Searches every line to see if it contains the sub-string.
    ///
    /// start is the zero-based offset to the line to start searching.
    size_t FindLineContaining(std::string_view str, size_t start = 0, bool CaseSensitive = true) const;

protected:

    // Converts lines into a vector of std::string members. Lines can end with \n, \r, or \r\n.
    void ParseLines(std::string_view str);

private:
    std::string m_buffer;
};
