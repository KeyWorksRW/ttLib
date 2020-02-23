/////////////////////////////////////////////////////////////////////////////
// Name:      tt::cstr
// Purpose:   Classes for handling zero-terminated char strings.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

/// @file
///
/// The tt::cstr class handles zero-terminated char strings. It inherits from std::string and can be used in most
/// places where std::string<char> is used. It provides additional functionality including utf8/16 conversions,
/// file name handling, etc.

#include <cassert>
#include <filesystem>
#include <string>
#include <string_view>

#include "ttcview.h"
#include "ttnamespace.h"

#if defined(__WXMSW__)
    #include <wx/string.h>
#endif

namespace tt
{
    extern std::string emptystring;
}  // namespace tt

namespace tt
{
    /// @brief This class inherits from **std::string** and provides additional string handling functionality.
    class cstr : public std::string
    {
    public:
        cstr(void) {}
        cstr(const char* psz) { assign(psz); }
        cstr(std::string_view view) { assign(view, view.size()); }
        cstr(tt::cview view) { assign(view, view.size()); }
        cstr(const cstr& str) { assign(str.c_str(), str.size()); }

        cstr(const std::filesystem::path& path) { assign(path.string(), path.string().size()); }
        cstr(const std::filesystem::directory_entry& dir)
        {
            assign(dir.path().string(), dir.path().string().size());
        }

#if defined(__WXMSW__)
        /// Converts wxString to a UTF8 string.
        ///
        /// This is needed for Windows because wxString is UTF16 and we need to convert it
        /// to UTF8. On POSIX, wxString is already UTF8 so no conversion is needed.
        cstr(wxString& str) { assign(str.utf8_str().data()); }
#endif

#if defined(_TT_TCSTR)
        // ttCStr is obsolete, but we'll support it until all callers have replaced it.
        cstr(ttCStr& csz) { assign(csz.c_str() ? csz.c_str() : tt::emptystring); }
#endif

        /// Caution: tt::cview is only valid until tt::cstr is modified or destroyed.
        tt::cview subview(size_t start = 0)
        {
            assert(start < length());
            return tt::cview(c_str() + start, length() - start);
        }

        std::wstring to_utf16() const;
        void from_utf16(std::wstring_view str);

        /// Case-insensitive comparison.
        int comparei(std::string_view str) const;

        /// Locates the position of a substring.
        size_t locate(std::string_view str, size_t posStart = 0, tt::CHECK_CASE check = tt::CHECK_CASE::yes) const;

        /// Returns true if the sub string exists
        bool contains(std::string_view sub, tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes)
        {
            return (locate(sub, 0, checkcase) != npos);
        }

        /// Find any one of the characters in a set. Returns offset if found, npos if not.
        ///
        /// This is equivalent to calling std::strpbrk but returns an offset instead of a pointer.
        size_t findoneof(const char* pszSet) const;

        /// Returns offset to the next whitespace character starting with pos. Returns npos if
        /// there are no more whitespaces.
        ///
        /// A whitespace character is a space, tab, eol or form feed character.
        size_t findspace(size_t start = 0) const;

        /// Returns offset to the next non-whitespace character starting with pos. Returns npos
        /// if there are no more non-whitespace characters.
        ///
        /// A whitespace character is a space, tab, eol or form feed character.
        size_t findnonspace(size_t start = 0) const;

        /// Returns an offset to the next word -- i.e., find the first non-whitedspace character
        /// after the next whitespace character.
        ///
        /// Equivalent to findnonspace(findspace(start)).
        size_t stepover(size_t start = 0) const;

        /// Returns true if the sub-string is identical to the first part of the main string
        bool issameas(std::string_view str, tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes) const;

        /// Returns true if the sub-string is identical to the first part of the main string
        bool issameprefix(std::string_view str, tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes) const;

        int atoi() const { return tt::atoi(*this); }

        enum class TRIM : size_t
        {
            right,
            left,
            both
        };

        /// Removes whitespace: ' ', \t, \r, \\n, \f
        ///
        /// where: TRIM::right, TRIM::left, or TRIM::both
        cstr& trim(TRIM where = TRIM::right);

        /// Assigns the string between chBegin and chEnd. This is typically used to copy the
        /// contents of a quoted string. Returns the position of the ending character in src.
        size_t AssignSubString(std::string_view src, char chBegin, char chEnd);

        /// Extracts a string from another string using start and end characters deduced from
        /// the first non-whitespace character after offset. Supports double and single quotes,
        /// angle and square brackets, and parenthesis.
        ///
        /// The return position is to the character in src that ended the string, or **npos** if no
        /// ending character was found.
        size_t ExtractSubString(std::string_view src, size_t offset = 0);

        /// Replace first (or all) occurrences of substring with another one
        size_t Replace(std::string_view oldtext, std::string_view newtext, bool replaceAll = false,
                       tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes);

        /// Replace everything from pos to the end of the current string with str
        cstr& replaceAll(size_t pos, std::string_view str)
        {
            replace(pos, length() - pos, str);
            return *this;
        }

        /// Generates hash of current string using djb2 hash algorithm
        size_t gethash() const;

        /// Convert the entire string to lower case. Assumes the string is UTF8.
        cstr& MakeLower();

        /// Convert the entire string to upper case. Assumes the string is UTF8.
        cstr& MakeUpper();

        /// Similer to sprintf, but without floating point support.
        ///
        /// %v expects a std::string_view argument.
        ///
        /// %k flag will place a string argument in quotes, and format a numerical argument
        /// with commas or periods (depending on the current locale).
        ///
        /// %z is considered unsigned unless the value is -1.
        cstr& cdecl Format(std::string_view format, ...);

#if defined(__WXMSW__)
        wxString FromUTF8() const { return wxString::FromUTF8(*this); }
#endif

        /// Caution: view is only valid until cstr is modified or destroyed!
        std::string_view subview(size_t start, size_t len = tt::npos) const;

        /// Converts all backslashes in the string to forward slashes.
        ///
        /// Note: Windows works just fine using forward slashes instead of backslashes.
        cstr& backslashestoforward();

        /// Returns true if current filename contains the specified case-insensitive extension.
        bool hasExtension(std::string_view ext) const
        {
            return tt::issameas(extension(), ext, tt::CHECK_CASE::no);
        }

        /// Returns true if current filename contains the specified case-insensitive file name.
        bool hasFilename(std::string_view name) const
        {
            return tt::issameas(filename(), name, tt::CHECK_CASE::no);
        }

        /// Returns a view to the current extension. View is empty if there is no extension.
        ///
        /// Caution: view is only valid until cstr is modified or destroyed.
        std::string_view extension() const;

        /// Returns a view to the current filename. View is empty if there is no filename.
        ///
        /// Caution: view is only valid until cstr is modified or destroyed.
        std::string_view filename() const;

        /// Replaces any existing extension with a new extension, or appends the extension if the
        /// name doesn't currently have an extension.
        ///
        /// If newExtension is empty, any existing extension will be removed.
        ///
        /// Returns view to the entire string.
        cstr& replace_extension(std::string_view newExtension);

        /// Replaces the filename portion of the string. Returns a view to the entire string.
        cstr& replace_filename(std::string_view newFilename);

        /// Removes the filename portion of the string. Returns a view to the entire string.
        cstr& remove_filename() { return replace_filename(""); }

        /// Appends the filename -- assumes current string is a path. This will add a trailing
        /// slash (if needed) before adding the filename.
        cstr& append_filename(std::string_view filename);

        /// Makes the current path relative to the supplied path. Supplied path should not
        /// contain a filename.
        cstr& make_relative(const std::string& relative_to);

        /// Changes any current path to an absolute path.
        cstr& make_absolute();

        /// Replaces any current string with the full path to the current working directory.
        cstr& assignCwd();

        /// Returns true if the current string refers to an existing file.
        bool fileExists() const;

        /// Returns true if the current string refers to an existing directory.
        bool dirExists() const;
    };

    /// Retrieves the current working directory and restores it in the dtor.
    class cwd : public tt::cstr
    {
    public:
        cwd() { assignCwd(); }
        ~cwd()
        {
            std::filesystem::path cwd(c_str());
            std::filesystem::current_path(cwd);
        }
    };

    /// Contains a vector of cstr classes with some additional functionality such
    /// as only adding a string if it doesn't already exist.
    class cstrVector : public std::vector<tt::cstr>
    {
    public:
        /// Same as find(pos, ch) but with a boolean result
        bool bfind(size_type pos, char ch) const { return (at(pos).find(ch) != npos); }

        /// Same as find(pos, str) but with a boolean result
        bool bfind(size_type pos, std::string_view str) const { return (at(pos).find(str) != npos); }

        /// Only appends the string if it doesn't already exist.
        ///
        /// Returns true if the string was added, false if it already existed.
        bool append(std::string_view str, tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes)
        {
            if (find(0, str, checkcase) != npos)
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

        /// Finds the position of the first string identical to the specified string.
        size_t find(size_t start, std::string_view str, tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes);

        /// Finds the position of the first string with specified prefix.
        size_t findprefix(size_t start, std::string_view prefix, tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes);

        /// Finds the position of the first string containing the specified sub-string.
        size_t contains(size_t start, std::string_view substring, tt::CHECK_CASE checkcase = tt::CHECK_CASE::yes);

        /// Unlike append(), this will add the string even if it already exists.
        void operator+=(std::string_view str) { push_back(str); }
    };

}  // namespace tt
