/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::winff class
// Purpose:   Wrapper around Windows FindFile
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !(__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
    #error "The contents of <ttlibspace.h> are available only with C++17 or later."
#endif

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

/// @file
///
/// This class uses the WIDE version of the Windows FindFile functionality, converting any name found
/// into a UTF8 string.
///
///    Example usage:
///
///    ttlib::winff ff("*.*");
///    if (ff.isvalid()) {
///        do {
///            if (ff.isdir())
///                std::cout << "Found the directory " << ff.c_str() << '\n';
///            else
///                std::cout << "Found the file " << ff.c_str() << '\n';
///        } while(ff.next());
///    }

#include <fileapi.h>
#include <stdint.h>
#include <string_view>

#include "ttcstr.h"

#if !defined(INVALID_HANDLE_VALUE)
    // normally defined in <handleapi.h>
    #define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR) -1)
#endif

namespace ttlib
{
    class winff : public WIN32_FIND_DATAW
    {
    public:
        winff() { m_hfind = INVALID_HANDLE_VALUE; }  // With this constructor, call NewPattern(...) to initialize

        winff(std::string_view filepattern)
        {
            std::wstring str16;
            ttlib::utf8to16(filepattern, str16);
            m_hfind = FindFirstFileExW(str16.c_str(), FindExInfoBasic, this, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);

            // Use same rule as std::filesystem directory_iterator and skip . and ..
            // If this is an empty directory, then the find handle will be closed and isvalid()
            // will return false.
            if (cFileName[0] == L'.' && !cFileName[1])
                next();
            else if (cFileName[0] == L'.' && cFileName[1] == L'.' && !cFileName[2])
                next();
            else
                m_filename.assignUTF16(cFileName);
        }
        ~winff()
        {
            if (m_hfind != INVALID_HANDLE_VALUE)
                FindClose(m_hfind);
        }

        bool next()
        {
            if (m_hfind == INVALID_HANDLE_VALUE)
                return false;

            if (FindNextFileW(m_hfind, this))
            {
                // Use same rule as std::filesystem directory_iterator and skip . and ..
                if (cFileName[0] == L'.' && !cFileName[1])
                    return next();
                else if (cFileName[0] == L'.' && cFileName[1] == L'.' && !cFileName[2])
                    return next();
                m_filename.assignUTF16(cFileName);
                return true;
            }
            else
            {
                if (m_hfind != INVALID_HANDLE_VALUE)
                    FindClose(m_hfind);
                m_hfind = INVALID_HANDLE_VALUE;
                m_filename.clear();
                return false;
            }
        }

        bool newpattern(std::string_view filepattern)
        {
            if (m_hfind != INVALID_HANDLE_VALUE)
                FindClose(m_hfind);
            std::wstring str16;
            ttlib::utf8to16(filepattern, str16);
            m_hfind = FindFirstFileExW(str16.c_str(), FindExInfoBasic, this, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
            m_filename.assignUTF16(cFileName);

            // Use same rule as std::filesystem directory_iterator and skip . and ..
            if (cFileName[0] == L'.' && !cFileName[1])
                next();
            else if (cFileName[0] == L'.' && cFileName[1] == L'.' && !cFileName[2])
                next();
            else
                m_filename.assignUTF16(cFileName);
            return isvalid();
        }

        const char* c_str() const noexcept { return m_filename.c_str(); }
        operator const char*() const noexcept { return m_filename.c_str(); }
        operator DWORD() const { return dwFileAttributes; }
        operator std::string_view() const noexcept { return m_filename.subview(0, m_filename.length()); }

        // Override [] to return char instead of w_char

        char operator[](int pos) { return m_filename.at(pos); }
        char operator[](size_t pos) { return m_filename.at(pos); }

        bool operator==(std::string_view name) { return m_filename.issameas(name); }
        bool operator!=(std::string_view name) { return !m_filename.issameas(name); }

        // Caution: this is NOT a copy! It returns a pointer to the internal cstr buffer. Any
        // changes you make will be overwritten by a call to next() or newpattern().
        ttlib::cstr& getcstr() { return m_filename; }

        bool isarchive() const { return (dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? true : false; }
        bool iscompressed() const { return (dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) ? true : false; }
        bool isdir() const { return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false; }
        bool ishidden() const { return (dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? true : false; }
        bool isoffline() const { return (dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) ? true : false; }
        bool isreadonly() const { return (dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? true : false; }
        bool issystem() const { return (dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? true : false; }
        bool istemporary() const { return (dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) ? true : false; }
        bool isvalid() const { return (m_hfind != INVALID_HANDLE_VALUE) ? true : false; }

        /// Call this before retrieving the filename if you want all backslashes converted to forward slahes.
        void backslashestoforward() { m_filename.backslashestoforward(); }

        /// Returns a 64-bit file size value.
        int64_t GetFullFileSize() { return (((int64_t) nFileSizeHigh) << 32) + (int64_t) nFileSizeLow; }

    private:
        HANDLE m_hfind { INVALID_HANDLE_VALUE };

        ttlib::cstr m_filename;
    };
}  // end namespace ttlib
