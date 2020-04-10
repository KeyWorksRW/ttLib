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

#include "ttcstr.h"
#include "ttlibspace.h"

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

        /// This will be the filename passed to ReadFile()
        ttlib::cstr& filename() { return m_filename; }

        /// Reads a string as if it was a file (see ReadFile).
        void ReadString(std::string_view str);

        /// Iterate through a list adding each iteration as a line.
        template<class iterT> void Read(const iterT iter)
        {
            for (const auto& line: iter)
            {
                emplace_back(line);
            }
        };

        /// Reads an array of char* strings. The last member of the array MUST be a null
        /// pointer.
        void ReadArray(const char** begin);

        /// Reads an count items from an array of char* strings.
        void ReadArray(const char** begin, size_t count);

        /// Writes each line to the file adding a '\n' to the end of the line.
        bool WriteFile(std::string_view filename) const;

        /// Searches every line to see if it contains the sub-string.
        ///
        /// startline is the zero-based offset to the line to start searching.
        size_t FindLineContaining(std::string_view str, size_t startline = 0,
                                  tt::CASE checkcase = tt::CASE::exact) const;

        /// If a line is found that contains orgStr, it will be replaced by newStr and the
        /// line position is returned. If no line is found, tt::npos is returned.
        size_t ReplaceInLine(std::string_view orgStr, std::string_view newStr, size_t startline = 0,
                             tt::CASE checkcase = tt::CASE::exact);

        bool issameas(ttlib::textfile other, tt::CASE checkcase = tt::CASE::exact) const;
        bool issameas(ttlib::viewfile other, tt::CASE checkcase = tt::CASE::exact) const;

        /// Use addEmptyLine() if you need to modify the line after adding it to the end.
        ///
        /// Use emplace_back(str) if you need to add an existing string.
        cstr& addEmptyLine() { return emplace_back(ttlib::emptystring); }

        cstr& insertEmptyLine(size_t pos)
        {
            if (pos >= size())
                return emplace_back(ttlib::emptystring);
            emplace(begin() + pos, ttlib::emptystring);
            return at(pos);
        }

        cstr& insertLine(size_t pos, const cstr& str)
        {
            if (pos >= size())
                return emplace_back(str);
            emplace(begin() + pos, str);
            return at(pos);
        }

        void RemoveLine(size_t line)
        {
            assert(line < size());
            if (line < size())
                erase(begin() + line);
        }

        void RemoveLastLine()
        {
            if (size())
                erase(begin() + (size() - 1));
        }

    protected:
        // Converts lines into a vector of std::string members. Lines can end with \n, \r, or \r\n.
        void ParseLines(std::string_view str);

    private:
        ttlib::cstr m_filename;
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

        /// This will be the filename passed to ReadFile()
        ttlib::cstr& filename() { return m_filename; }

        /// Reads a string as if it was a file (see ReadFile).
        void ReadString(std::string_view str);

        /// Writes each line to the file adding a '\n' to the end of the line.
        bool WriteFile(std::string_view filename) const;

        /// Returns the string storing the entire file. If you change this string, all
        /// the string_view vector entries will be invalid!
        ttlib::cstr& GetBuffer() { return m_buffer; }

        /// Call this if you change the buffer returned by GetBuffer() to turn the buffer
        /// into an array of string_views.
        void ParseBuffer();

        /// Searches every line to see if it contains the sub-string.
        ///
        /// startline is the zero-based offset to the line to start searching.
        size_t FindLineContaining(std::string_view str, size_t startline = 0,
                                  tt::CASE checkcase = tt::CASE::exact) const;

        bool issameas(ttlib::textfile other, tt::CASE checkcase = tt::CASE::exact) const;
        bool issameas(ttlib::viewfile other, tt::CASE checkcase = tt::CASE::exact) const;

    protected:
        // Converts lines into a vector of std::string members. Lines can end with \n, \r, or \r\n.
        void ParseLines(std::string_view str);

    private:
        ttlib::cstr m_buffer;
        ttlib::cstr m_filename;
    };
}  // namespace ttlib
