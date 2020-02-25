/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::textfile, ttlib::viewfile
// Purpose:   Classes for reading and writing line-oriented files
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

/// @file
/// Contains classes for reading line-oriented SBCS or UTF8 files. Use ttlib::textfile if you
/// need to make changes to the file. Use ttlib::viewfile is you just want to examine, but not
/// modify the file.
///
/// If you want to read a file and optionally modify it you can do something like the following:
///
///      ttlib::viewfile original;
///      if (original.ReadFile("your filename"))
///      {
///         ttlib::textfile file;
///         file.ReadFile(original.GetContainer());
///             ... // possible modifications.
///         if (!file.issameas(original))
///             file.WriteFile("your filename");
///      }
///
/// Note: The entire file is read into memory, so these classes are not appropriate for extemely large
/// files.

#include <string_view>
#include <vector>

#include "ttlibspace.h"
#include "ttcstr.h"

namespace ttlib
{
    class viewfile;  // forward definition

    /// This reads a line-oriented file into a vector of ttlib::cstr (std::string)
    /// allowing you to modify, append, or delete individual lines. If you write
    /// the file each line written is appended with a single '\n' character.
    class textfile : public std::vector<ttlib::cstr>
    {
    public:
        /// Reads a line-oriented file and converts each line into a ttlib::cstr
        /// (std::string).
        bool ReadFile(std::string_view filename);

        /// Reads a string as if it was a file (see ReadFile).
        void ReadString(std::string_view str);

        /// Writes each line to the file adding a '\n' to the end of the line.
        bool WriteFile(std::string_view filename) const;

        /// Searches every line to see if it contains the sub-string.
        ///
        /// startline is the zero-based offset to the line to start searching.
        size_t FindLineContaining(std::string_view str, size_t startline = 0,
                                  ttlib::CHECK_CASE checkcase = ttlib::CHECK_CASE::yes) const;

        bool issameas(ttlib::textfile other, ttlib::CHECK_CASE checkcase = ttlib::CHECK_CASE::yes);
        bool issameas(ttlib::viewfile other, ttlib::CHECK_CASE checkcase = ttlib::CHECK_CASE::yes);

        ttlib::cstr& GetTempLine()
        {
            m_tempLine.clear();
            return m_tempLine;
        }

        /// Adds the temporary string and clears it.
        void WriteTempLine()
        {
            if (!m_tempLine.empty())
                push_back(std::move(m_tempLine));
            else
                addblankline();
        }

        /// Appends text to the end of the temporary string, then adds the entire
        /// string and clears it.
        void WriteTempLine(std::string_view text)
        {
            m_tempLine.append(text);
            push_back(std::move(m_tempLine));
        }

        void addblankline() { push_back(""); }

    protected:
        // Converts lines into a vector of std::string members. Lines can end with \n, \r, or \r\n.
        void ParseLines(std::string_view str);

    private:
        ttlib::cstr m_tempLine;
    };
}  // namespace ttlib

////////////////////////////// ttlib::viewfile class ///////////////////////////////

namespace ttlib
{
    /// Almost identical to ttlib::textfile, only the entire file is stored as a single
    /// string, and the vector contains a std::string_view for each line. This is
    /// a faster way to read the file if you don't need to modify the contents.
    class viewfile : public std::vector<std::string_view>
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
        /// startline is the zero-based offset to the line to start searching.
        size_t FindLineContaining(std::string_view str, size_t startline = 0,
                                  ttlib::CHECK_CASE checkcase = ttlib::CHECK_CASE::yes) const;

        bool issameas(ttlib::textfile other, ttlib::CHECK_CASE checkcase = ttlib::CHECK_CASE::yes);
        bool issameas(ttlib::viewfile other, ttlib::CHECK_CASE checkcase = ttlib::CHECK_CASE::yes);

        const std::string& GetContainer() { return m_buffer; }

    protected:
        // Converts lines into a vector of std::string members. Lines can end with \n, \r, or \r\n.
        void ParseLines(std::string_view str);

    private:
        std::string m_buffer;
    };
}  // namespace ttlib
