/////////////////////////////////////////////////////////////////////////////
// Name:      ttString, ttStrlist
// Purpose:   String and vector classes with some additional functionality
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ttnamespace.h"

#include <filesystem>

#if defined(__WXMSW__)
    #include <wx/string.h>
#endif

/// Global empty string.
extern const std::string ttEmptyString;

/// @file
/// The ttString class inherits from std::string providing some additional functionality
/// such as case-insensitive searching and some limited filename handling functions.
///
/// While std::filesystem:path is generally a better way to work with filenames, this class does provide
/// some similar functionality in case you need to store filenames in std::string instead of filesystem::path.

/// @brief This class inherits from **std::string** and provides additional string handling functionality.
class ttString : public std::string
{
public:
    // Public functions

    ttString(void) {}
    ttString(const char* psz) { assign(psz); }
    ttString(std::string_view str) { assign(str); }
    ttString(const ttString& str) { assign(str.c_str()); }

    ttString(const std::filesystem::path& path) { assign(path.string()); }
    ttString(const std::filesystem::directory_entry& dir) { assign(dir.path().string()); }

#if defined(__WXMSW__)
    /// Converts wxString to a UTF8 string.
    ///
    /// This is needed for Windows because wxString is UTF16 and we need to convert it
    /// to UTF8. On POSIX, wxString is already UTF8 so no conversion is needed.
    ttString(wxString& str) { assign(str.utf8_str().data()); }
#endif  // _WIN32

#if defined(_TT_TCSTR)
    ttString(ttCStr& csz) { assign(csz.c_str() ? csz.c_str() : ""); }
#endif

    /// Case-insensitive comparison.
    int comparei(std::string_view str) const;

    /// Case-insensitive version of find.
    ///
    /// If bUtf8 is true, current string and string to search for are assumed to be
    /// coded as UTF8 strings.
    size_t findi(std::string_view str, bool bUtf8 = false) const;

    /// Returns true if the sub string exists
    ///
    /// Same as find/findi but with a boolean return instead of a position.
    bool contains(std::string_view sub, bool CaseSensitive = true);

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

    /// Same as compare only it returns a boolean instead of the difference.
    inline bool issamestr(std::string_view str) const { return (compare(str) == 0); }

    /// Same as issamestr only case insensitive comparison of ASCII characters
    bool issamestri(std::string_view str) const { return (comparei(str) == 0); }

    /// Returns true if the sub-string is identical to the first part of the main string
    bool issamesubstr(std::string_view str) const;

    /// Same as issamestr only case insensitive comparison of ASCII characters
    bool issamesubstri(std::string_view str) const;

    /// Similar to find only it returns a view. The view is empty if the string was not found.
    std::string_view strstr(std::string_view str) const { return tt::findstr(*this, str, true); }

    /// Similar to find only it does a case-insensitve search and returns a view.
    /// The view is empty if the string was not found.
    std::string_view strstri(std::string_view str) const  { return tt::findstr(*this, str, false); }

    int atoi() const { return tt::atoi(*this); }

    /// Removes whitespace: ' ', \t, \r, \\n, \f
    ///
    /// The default is to remove trailing whitespace. trim(false) will remove
    /// leading whitespace.
    void trim(bool fromRight = true);

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
    size_t Replace(std::string_view oldtext, std::string_view newtext, bool replaceAll = true,
                   bool CaseSensitive = true, bool Utf8 = false);

    /// Replace everything from pos to the end of the current string with str
    std::string& replaceAll(size_t pos, std::string_view str) { return replace(pos, length() - pos, str); }

    /// Generates hash of current string using djb2 hash algorithm
    size_t gethash() const;

    // Convert the entire string to lower case. Assumes the string is UTF8.
    std::string_view MakeLower();

    // Convert the entire string to upper case. Assumes the string is UTF8.
    std::string_view MakeUpper();

#if defined(__WXMSW__)
    wxString FromUTF8() const { return wxString::FromUTF8(*this); }
#endif

    /// Converts all backslashes in the string to forward slashes.
    ///
    /// Note: Windows works just fine using forward slashes instead of backslashes.
    void backslashestoforward();

    /// Returns a view to the current extension. View is empty if there is no extension.
    std::string_view extension() const;

    /// Returns a view to the current filename. View is empty if there is no filename.
    std::string_view filename() const;

    /// Replaces any existing extension with a new extension, or appends the extension if the
    /// name doesn't currently have an extension.
    ///
    /// If newExtension is empty, any existing extension will be removed.
    ///
    /// Returns view to the entire string.
    std::string_view replace_extension(std::string_view newExtension);

    /// Replaces the filename portion of the string. Returns a view to the entire string.
    std::string_view replace_filename(std::string_view newFilename);

    /// Removes the filename portion of the string. Returns a view to the entire string.
    std::string_view remove_filename() { return replace_filename(""); }

    /// Appends the filename -- assumes current string is a path. This will add a trailing
    /// slash (if needed) before adding the filename.
    std::string_view append_filename(std::string_view filename);

    /// Makes the current path relative to the supplied path. Supplied path should not
    /// contain a filename.
    std::string_view make_relative(const std::string& relative_to);

    /// Changes any current path to an absolute path.
    std::string_view make_absolute();

    /// Replaces any current string with the full path to the current working directory.
    std::string_view assignCwd();

    /// Returns true if the current string refers to an existing file.
    bool fileExists() const;

    /// Returns true if the current string refers to an existing directory.
    bool dirExists() const;
};

/// Retrieves the current working directory and restores it in the dtor.
class ttCwd : public ttString
{
public:
    ttCwd() { assignCwd(); }
    ~ttCwd()
    {
        std::filesystem::path cwd(c_str());
        std::filesystem::current_path(cwd);
    }
};

/// Contains a vector of ttString classes with some additional functionality such
/// as only adding a string if it doesn't already exist.
class ttStrVector : public std::vector<ttString>
{
public:
    /// Same as find(pos, ch) but with a boolean result
    bool bfind(size_type pos, char ch) const { return (at(pos).find(ch) != std::string::npos); }

    /// Same as find(pos, psz) but with a boolean result
    bool bfind(size_type pos, std::string_view str) const { return (at(pos).find(str) != std::string::npos); }

    /// Only appends the string if it doesn't already exist.
    ///
    /// Returns true if the string was added, false if it already existed.
    bool append(std::string_view str)
    {
        for (auto iter : *this)
        {
            if (iter.compare(str) == 0)
                return false;
        }
        push_back(str);
        return true;
    }

    /// Only appends the string if it doesn't already exist (case-insensitive).
    ///
    /// Returns true if the string was added, false if it already existed.
    bool appendi(std::string_view str)
    {
        for (auto iter : *this)
        {
            if (iter.comparei(str) == 0)
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

    /// Unlike append(), this will add the string even if it already exists.
    void operator+=(std::string_view str) { push_back(str); }
};
