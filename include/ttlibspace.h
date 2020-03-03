/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib namespace
// Purpose:   Contains the ttlib namespace functions/declarations common to all ttLib libraries
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

// clang-format off

#ifndef _TTLIB_NAMESPACE_H_GUARD_
#define _TTLIB_NAMESPACE_H_GUARD_

#include <string>
#include <string_view>
#include <stdlib.h>  // for std::abs(long)

#include <filesystem>  // directory_entry

// Some useful macros if you don't already have them

#ifndef assertm
/// assert with a message
    #define assertm(exp, msg) assert(((void) msg, exp))
#endif

#if defined(NDEBUG)
    #define DBG_PARAM(param) /* param */
#else
/// Use this macro around a function parameter that you only use in DEBUG builds. In
/// non-debug builds, the argument is commented out.
    #define DBG_PARAM(param) param
#endif

// clang-format on

namespace ttlib
{
    /// Use to compare a size_t against -1
    constexpr size_t npos = static_cast<size_t>(-1);

    extern const std::string emptystring;

    enum class CASE : size_t
    {
        exact,
        either,
        utf8  // comparisons are done by converting characters to lowercase UTF8
    };

    /// Equivalent to Windows RECT structure -- this makes it available on non-Windows
    /// platforms.
    struct WINRECT
    {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;

        inline int32_t GetWidth() const { return std::abs(right - left); }
        inline int32_t GetHeight() const { return std::abs(bottom - top); }
    };

    /// Only valid for ANSI characters
    constexpr inline bool isalpha(char ch) noexcept
    {
        return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
    }

    constexpr inline bool isdigit(char ch) noexcept { return ((ch >= '0' && ch <= '9') || ch == '-'); }

    /// Is ch the start of a utf8 sequence?
    constexpr inline bool isutf8(char ch) noexcept { return ((ch & 0xC0) != 0x80); }

    /// Returns true if character is a space, tab, eol or form feed character.
    constexpr inline bool iswhitespace(char ch) noexcept
    {
        return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f');
    }

    /// Returns true if character is a period, comma, semi-colon, colon, question or exclamation
    constexpr inline bool ispunctuation(char ch) noexcept
    {
        return (ch == '.' || ch == ',' || ch == ';' || ch == ':' || ch == '?' || ch == '!');
    }

    /// Returns true if strings are identical
    bool issameas(std::string_view str1, std::string_view str2, CASE checkcase = CASE::exact);

    /// Same as compare only it returns a boolean instead of the difference.
    [[deprecated("Use issameas()")]] inline bool issamestr(std::string_view str1, std::string_view str2)
    {
        return (str1.compare(str2) == 0);
    }

    /// Same as issamestr only case insensitive comparison of ASCII characters
    [[deprecated("Use issameas()")]] bool issamestri(std::string_view str1, std::string_view str2);

    /// Returns true if the sub-string is identical to the first part of the main string
    bool issameprefix(std::string_view strMain, std::string_view strSub, CASE checkcase = CASE::exact);

    /// Returns true if the sub-string is identical to the first part of the main string
    [[deprecated("Use issameprefix()")]] bool issamesubstr(std::string_view strMain, std::string_view strSub);

    /// Case-insensitive sub string comparison
    [[deprecated("Use issameprefix()")]] bool issamesubstri(std::string_view strMain, std::string_view strSub);

    /// Return a view to the portion of the string beginning with the sub string.
    ///
    /// Return view is empty if substring is not found.
    std::string_view findstr(std::string_view main, std::string_view sub, CASE checkcase = CASE::exact);

    /// Returns the position of sub within main, or npos if not found.
    size_t findstr_pos(std::string_view main, std::string_view sub, CASE checkcase = CASE::exact);

    /// Returns true if the sub string exists withing the main string.
    ///
    /// Same as findstr but with a boolean return instead of a string_view.
    bool contains(std::string_view main, std::string_view sub, CASE checkcase = CASE::exact);

    /// Return a view to the portion of the string beginning with the sub string.
    ///
    /// Return view is empty if substring is not found.
    [[deprecated("Use findstr()")]] std::string_view strstr(std::string_view strMain, std::string_view strSub);

    /// Case-insensitive search for a sub string.
    ///
    /// Return view is empty if substring is not found.
    [[deprecated("Use findstr()")]] std::string_view strstri(std::string_view strMain, std::string_view strSub);

    /// Returns a pointer to the next character in a UTF8 string.
    const char* nextut8fchar(const char* psz) noexcept;

    /// Returns view to the next whitespace character. View is empty if there are no more
    /// whitespaces.
    std::string_view findspace(std::string_view str) noexcept;

    /// Returns position of next whitespace character or npos if not found.
    size_t findspace_pos(std::string_view str);

    /// Returns view to the next non-whitespace character. View is empty if there are no
    /// non-whitespace characters.
    std::string_view findnonspace(std::string_view str) noexcept;

    /// Returns position of next non-whitespace character or npos if not found.
    size_t findnonspace_pos(std::string_view str) noexcept;

    /// Equivalent to findnonspace(findspace(str)).
    std::string_view stepover(std::string_view str);

    /// Equivalent to findnonspace(findspace(str)) returning the position or npos.
    size_t stepover_pos(std::string_view str) noexcept;

    /// Generates hash of string using djb2 hash algorithm
    size_t gethash(std::string_view str) noexcept;

    /// Converts a string into an integer.
    ///
    /// If string begins with '0x' it is assumed to be hexadecimal and is converted.
    /// String may begin with a '-' or '+' to indicate the sign of the integer.
    int atoi(std::string_view str) noexcept;

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

    /// Performs a check to see if a directory entry is a filename and contains the
    /// specified extension.
    bool hasextension(std::filesystem::directory_entry name, std::string_view extension,
                      CASE checkcase = CASE::exact);

    /// Confirms newdir exists and is a directory and then changes to that directory.
    ///
    /// Returns false only if newdir is not an existing directory. Throws filesystem_error
    /// if the directory is valid but could not be changed to.
    bool ChangeDir(std::string_view newdir);

    bool dirExists(std::string_view dir);
    bool fileExists(std::string_view filename);

}  // namespace ttlib

/////////////// Following section can only be used when compiling for Windows ///////////////
///
/// CAUTION: If you use any of the following functions, you MUST link to ttLibwin not ttLib as
/// all the underlying code that supports them is only available in the Windows version of the
/// library.
///
/////////////////////////////////////////////////////////////////////////////////////////////

// clang-format off

#if defined(_WIN32)

#include <windows.h>

namespace ttlib
{
    // Caution! If you use any of the following functions, you MUST link with ttLibwin.lib NOT ttlib.lib

    /// Sets title to use in all calls to ttlib::MsgBox
    void SetMsgBoxTitle(std::string_view utf8Title);

    /// Converts the message to UTF16 and displays it in a Windows message box (MessageBox(...))
    ///
    /// Caption is whatever was set by last call to ttlib::SetMsgBoxTitle().
    int MsgBox(std::string_view utf8str, UINT uType = MB_OK | MB_ICONWARNING);

    /// Converts the message and caption to UTF16 and displays them in a Windows message box
    /// (MessageBox(...))
    int MsgBox(std::string_view utf8str, std::string_view utf8Caption, UINT uType = MB_OK | MB_ICONWARNING);

    /// Converts window text to UTF8 and returns it in a std::string container
    std::string GetWndText(HWND hwnd);

    /// Converts window text to UTF8 and assigns it to str
    bool GetWndText(HWND hwnd, std::string& str);

    /// Sends LB_GETTEXT, converts the result to UTF8 and returns it in std::string.
    ///
    /// returned string will be empty() if index is invalid.
    std::string GetListboxText(HWND hwndLB, WPARAM index);

    /// Sends LB_GETTEXT, converts the result to UTF8 and returns it in str.
    ///
    /// Returns false if index is invalid (and sets str to ttEmptyString)
    bool GetListboxText(HWND hwndLB, WPARAM index, std::string& str);

    /// Sends CB_GETLBTEXT, converts the result to UTF8 and returns it in std::string.
    ///
    /// std::string will be empty() if index is invalid.
    std::string GetComboLBText(HWND hwndLB, WPARAM index);

    /// Sends CB_GETLBTEXT, converts the result to UTF8 and returns it in str.
    ///
    /// Returns false if index is invalid (and sets str to ttEmptyString)
    bool GetComboLBText(HWND hwndLB, WPARAM index, std::string& str);

    /// Converts the text to UTF16 before calling SetWindowTextW(...)
    void SetWndText(HWND hwnd, std::string_view utf8str);

    /// Loads the specified UTF8 text string from a resource (calls Windows LoadResource API).
    ///
    /// Return string will be empty if an error occurred.
    std::string LoadTextResource(DWORD idResource, HMODULE hmodResource = NULL);

    /// Converts all text to UTF16 before calling ShellExecuteW(...)
    HINSTANCE ShellRun(std::string_view filename, std::string_view args, std::string_view directory,
                       INT nShow = SW_SHOWNORMAL, HWND hwndParent = NULL);

}  // namespace ttlib

#endif  // end _WIN32 section

#endif  // _TTLIB_NAMESPACE_H_GUARD_
