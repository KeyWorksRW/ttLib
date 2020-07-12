/////////////////////////////////////////////////////////////////////////////
// Name:      ttcview.cpp
// Purpose:   string_view functionality on a zero-terminated char string.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <cassert>
#include <cctype>
#include <locale>

#include "ttcview.h"

using namespace ttlib;

bool cview::issameas(std::string_view str, tt::CASE checkcase) const
{
    if (size() != str.size())
        return false;

    if (empty())
        return str.empty();

    // if both strings have the same length, then we can compare as a prefix.
    return issameprefix(str, checkcase);
}

bool cview::issameprefix(std::string_view str, tt::CASE checkcase) const
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

size_t cview::locate(std::string_view str, size_t posStart, tt::CASE checkcase) const
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

bool cview::viewspace() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (ttlib::iswhitespace(at(pos)))
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

bool cview::viewnonspace() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (!ttlib::iswhitespace(at(pos)))
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

bool cview::viewnextword() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (ttlib::iswhitespace(at(pos)))
            break;
    }
    if (pos >= length())
        return false;

    // whitespace found, look for non-whitespace
    else
    {
        for (++pos; pos < length(); ++pos)
        {
            if (!ttlib::iswhitespace(at(pos)))
                break;
        }
        if (pos >= length())
            return false;

        remove_prefix(pos);
        return true;
    }
}

cview cview::view_digit(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (ttlib::isdigit(at(start)))
            return subview(start);
    }

    return subview(length());
}

cview cview::view_nondigit(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (!ttlib::isdigit(at(start)))
            return subview(start);
    }

    return subview(length());
}

bool cview::viewdigit() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (ttlib::isdigit(at(pos)))
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

bool cview::viewnondigit() noexcept
{
    if (empty())
        return false;
    size_t pos;
    for (pos = 0; pos < length(); ++pos)
    {
        if (!ttlib::isdigit(at(pos)))
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

bool cview::viewextension() noexcept
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

bool cview::viewfilename() noexcept
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

ttlib::cview cview::extension() const noexcept
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
    else if (c_str()[pos + 1] == '.')
        return "";

    return { c_str() + pos, length() - pos };
}

ttlib::cview cview::filename() const noexcept
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
            return { c_str(), length() };
    }

    return { c_str() + pos + 1, length() - (pos + 1) };
}

bool cview::fileExists() const
{
    if (empty())
        return false;
    auto file = std::filesystem::directory_entry(std::filesystem::u8path(c_str()));
    return (file.exists() && !file.is_directory());
}

bool cview::dirExists() const
{
    if (empty())
        return false;
    auto file = std::filesystem::directory_entry(std::filesystem::u8path(c_str()));
    return (file.exists() && file.is_directory());
}

size_t cview::gethash() const noexcept
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

size_t cview::findoneof(const std::string& set) const
{
    if (set.empty())
        return tt::npos;
    const char* pszFound = std::strpbrk(c_str(), set.c_str());
    if (!pszFound)
        return tt::npos;
    return (static_cast<size_t>(pszFound - c_str()));
}

size_t cview::findspace(size_t start) const
{
    if (start >= length())
        return npos;
    const char* pszFound = std::strpbrk(c_str() + start, " \t\r\n\f");
    if (!pszFound)
        return npos;
    return (static_cast<size_t>(pszFound - c_str()));
}

size_t cview::findnonspace(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (!std::strchr(" \t\r\n\f", at(start)))
            break;
    }
    return start;
}

size_t cview::stepover(size_t start) const
{
    auto pos = findspace(start);
    if (pos != npos)
    {
        pos = findnonspace(pos);
    }
    return pos;
}
