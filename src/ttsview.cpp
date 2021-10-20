/////////////////////////////////////////////////////////////////////////////
// Purpose:   std::string_view with additional methods
// Author:    Ralph Walden
// Copyright: Copyright (c) 2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cctype>
#include <locale>

#include "ttsview.h"

using namespace ttlib;

bool sview::is_sameas(std::string_view str, tt::CASE checkcase) const
{
    if (size() != str.size())
        return false;

    if (empty())
        return str.empty();

    // if both strings have the same length, then we can compare as a prefix.
    return is_sameprefix(str, checkcase);
}

bool sview::is_sameprefix(std::string_view str, tt::CASE checkcase) const
{
    if (str.empty())
        return empty();

    if (empty() || length() < str.length())
        return false;

    if (checkcase == tt::CASE::exact)
    {
        auto iterMain = begin();
        for (auto iterSub: str)
        {
            if (*iterMain++ != iterSub)
                return false;
        }
        return true;
    }
    else if (checkcase == tt::CASE::either)
    {
        auto iterMain = begin();
        for (auto iterSub: str)
        {
            if (std::tolower(*iterMain++) != std::tolower(iterSub))
                return false;
        }
        return true;
    }
    else if (checkcase == tt::CASE::utf8)
    {
        auto utf8locale = std::locale("en_US.utf8");
        auto iterMain = begin();
        for (auto iterSub: str)
        {
            if (std::tolower(*iterMain++, utf8locale) != std::tolower(iterSub, utf8locale))
                return false;
        }
        return true;
    }
    assert(!"Unknown tt::CASE value");
    return false;
}

size_t sview::locate(std::string_view str, size_t posStart, tt::CASE checkcase) const
{
    if (str.empty() || posStart >= size())
        return npos;

    if (checkcase == tt::CASE::exact)
        return find(str, posStart);

    if (checkcase == tt::CASE::either)
    {
        auto chLower = std::tolower(str[0]);
        for (auto pos = posStart; pos < length(); ++pos)
        {
            if (std::tolower(at(pos)) == chLower)
            {
                size_t posSub;
                for (posSub = 1; posSub < str.length(); ++posSub)
                {
                    if (pos + posSub >= length())
                        return npos;
                    if (std::tolower(at(pos + posSub)) != std::tolower(str.at(posSub)))
                        break;
                }
                if (posSub >= str.length())
                    return pos;
            }
        }
    }
    else
    {
        auto utf8locale = std::locale("en_US.utf8");
        auto chLower = std::tolower(str[0], utf8locale);
        for (auto pos = posStart; pos < length(); ++pos)
        {
            if (std::tolower(at(pos), utf8locale) == chLower)
            {
                size_t posSub;
                for (posSub = 1; posSub < str.length(); ++posSub)
                {
                    if (pos + posSub >= length())
                        return npos;
                    if (std::tolower(at(pos + posSub), utf8locale) != std::tolower(str.at(posSub), utf8locale))
                        break;
                }
                if (posSub >= str.length())
                    return pos;
            }
        }
    }
    return npos;
}

bool sview::moveto_space() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (ttlib::is_whitespace(at(pos)))
            break;
    }
    if (pos >= length())
        return false;
    else
    {
        remove_prefix(pos);
        return true;
    }
}

bool sview::moveto_nonspace() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (!ttlib::is_whitespace(at(pos)))
            break;
    }
    if (pos >= length())
        return false;
    else
    {
        remove_prefix(pos);
        return true;
    }
}

bool sview::moveto_nextword() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (ttlib::is_whitespace(at(pos)))
            break;
    }
    if (pos >= length())
        return false;

    // whitespace found, look for non-whitespace
    else
    {
        for (++pos; pos < length(); ++pos)
        {
            if (!ttlib::is_whitespace(at(pos)))
                break;
        }
        if (pos >= length())
            return false;

        remove_prefix(pos);
        return true;
    }
}

sview sview::view_digit(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (ttlib::is_digit(at(start)))
            return subview(start);
    }

    return subview(length());
}

sview sview::view_nondigit(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (!ttlib::is_digit(at(start)))
            return subview(start);
    }

    return subview(length());
}

bool sview::moveto_digit() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (ttlib::is_digit(at(pos)))
            break;
    }
    if (pos >= length())
        return false;
    else
    {
        remove_prefix(pos);
        return true;
    }
}

bool sview::moveto_nondigit() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (!ttlib::is_digit(at(pos)))
            break;
    }
    if (pos >= length())
        return false;
    else
    {
        remove_prefix(pos);
        return true;
    }
}

bool sview::moveto_extension() noexcept
{
    if (empty())
        return false;

    auto pos = rfind('.');
    if (pos == npos)
        return false;
    else if (pos + 1 >= length())  // . by itself is a folder
        return false;
    else if (pos < 2 && (at(pos + 1) == '.'))
        return false;

    remove_prefix(pos);
    return true;
}

bool sview::moveto_filename() noexcept
{
    if (empty())
        return false;

    auto pos = find_last_of('/');

#if defined(_WIN32)
    // Windows filenames can contain both forward and back slashes, so check for a backslash as well.
    auto back = find_last_of('\\');
    if (back != npos)
    {
        // If there is no forward slash, or the backslash appears after the forward slash, then use it's position.
        if (pos == npos || back > pos)
            pos = back;
    }
#endif
    if (pos == npos)
    {
        pos = find_last_of(':');
        if (pos == npos)
            return false;
    }

    remove_prefix(pos + 1);
    return true;
}

ttlib::sview sview::extension() const noexcept
{
    if (empty())
        return "";

    auto pos = find_last_of('.');
    if (pos == npos)
        return "";

    // . by itself is a folder
    else if (pos + 1 >= length())
        return "";
    // .. is not a valid extension (it's usually part of a folder as in "../dir/")
    else if (data()[pos + 1] == '.')
        return "";

    return { data() + pos, length() - pos };
}

ttlib::sview sview::filename() const noexcept
{
    if (empty())
        return "";

    auto pos = find_last_of('/');

#if defined(_WIN32)
    // Windows filenames can contain both forward and back slashes, so check for a backslash as well.
    auto back = find_last_of('\\');
    if (back != npos)
    {
        // If there is no forward slash, or the backslash appears after the forward slash, then use it's position.
        if (pos == npos || back > pos)
            pos = back;
    }
#endif
    if (pos == npos)
    {
        pos = find_last_of(':');
        if (pos == npos)
            return { data(), length() };
    }

    return { data() + pos + 1, length() - (pos + 1) };
}

bool sview::file_exists() const
{
    if (empty())
        return false;
#ifdef _MSC_VER
    auto file = std::filesystem::directory_entry(std::filesystem::path((to_utf16())));
#else
    auto file = std::filesystem::directory_entry(std::filesystem::path(c_str()));
#endif
    return (file.exists() && !file.is_directory());
}

bool sview::dir_exists() const
{
    if (empty())
        return false;
#ifdef _MSC_VER
    auto file = std::filesystem::directory_entry(std::filesystem::path(to_utf16()));
#else
    auto file = std::filesystem::directory_entry(std::filesystem::path(c_str()));
#endif
    return (file.exists() && file.is_directory());
}

size_t sview::get_hash() const noexcept
{
    if (empty())
        return 0;

    // djb2 hash algorithm

    size_t hash = 5381;

    for (auto iter: *this)
    {
        hash = ((hash << 5) + hash) ^ iter;
    }

    return hash;
}

size_t sview::find_oneof(const std::string& set, size_t start) const
{
    if (set.empty())
        return tt::npos;

    for (start = 0; start < length(); ++start)
    {
        if (std::strchr(set.c_str(), at(start)))
            break;
    }
    return (start < length() ? start : tt::npos);
}

size_t sview::find_oneof(sview set, size_t start) const
{
    if (set.empty())
        return tt::npos;
    std::string copy(set);
    return find_oneof(copy, start);
}

size_t sview::find_space(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (std::strchr(" \t\r\n\f", at(start)))
            break;
    }
    return (start < length() ? start : tt::npos);
}

size_t sview::find_nonspace(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (!std::strchr(" \t\r\n\f", at(start)))
            break;
    }
    return (start < length() ? start : tt::npos);
}

size_t sview::stepover(size_t start) const
{
    auto pos = find_space(start);
    if (pos != npos)
    {
        pos = find_nonspace(pos);
    }
    return pos;
}

std::wstring sview::to_utf16() const
{
    std::wstring str16;
    ttlib::utf8to16(*this, str16);
    return str16;
}

int sview::comparei(std::string_view str) const
{
    auto main = begin();
    auto sub = str.begin();
    while (sub != str.end())
    {
        auto diff = std::tolower(main[0]) - std::tolower(sub[0]);
        if (diff != 0)
            return diff;
        ++main;
        ++sub;
        if (main == end())
            return (sub != str.end() ? -1 : 0);
    }

    return (main != end() ? 1 : 0);
}

/**
 * @param chBegin -- character that prefixes the string
 * @param chEnd -- character that terminates the string.
 */
sview sview::view_substr(size_t offset, char chBegin, char chEnd)
{
    if (empty() || offset >= size())
    {
        return ttlib::emptystring;
    }

    // step over any leading whitespace unless chBegin is a whitespace character
    if (!ttlib::is_whitespace(chBegin))
    {
        while (ttlib::is_whitespace(at(offset)))
            ++offset;
    }

    if (at(offset) == chBegin)
    {
        ++offset;
        auto start = offset;
        while (offset < size() && at(offset) != chEnd)
        {
            // REVIEW: [KeyWorks - 01-26-2020] '\"' is also valid for the C compiler, though the slash
            // is unnecessary. Should we support it?

            // only check quotes -- a slash is valid before other character pairs.
            if (at(offset) == '\\' && (chBegin == '"' || chBegin == '\'') && offset + 1 < size() &&
                (at(offset + 1) == chEnd))
            {
                // step over an escaped quote if the string to fetch is within a quote
                offset += 2;
                continue;
            }
            ++offset;
        }

        return sview(data() + start, offset - start);
    }
    else
    {
        // if the string didn't start with chBegin, just copy the string. Note that offset may have changed if
        // chBegin was not whitespace and at(offset) was whitespace.

        return subview(offset);
    }
}

sview sview::find_space(std::string_view str) noexcept
{
    if (str.empty())
        return ttlib::emptystring;
    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (ttlib::is_whitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return ttlib::emptystring;
    else
        return sview(str.data() + pos, str.length() - pos);
}

sview sview::find_nonspace(std::string_view str) noexcept
{
    if (str.empty())
        return ttlib::emptystring;
    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (!ttlib::is_whitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return ttlib::emptystring;
    else
        return sview(str.data() + pos, str.length() - pos);
}

sview sview::stepover(std::string_view str) noexcept
{
    if (str.empty())
        return ttlib::emptystring;

    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (ttlib::is_whitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return ttlib::emptystring;

    for (; pos < str.length(); ++pos)
    {
        if (!ttlib::is_whitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return ttlib::emptystring;
    else
        return sview(str.data() + pos, str.length() - pos);
}
