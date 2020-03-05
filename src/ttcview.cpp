/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::cview
// Purpose:   string_view functionality on a zero-terminated char string.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <cassert>
#include <cctype>
#include <locale>

#include "ttcview.h"

using namespace ttlib;

bool cview::issameas(std::string_view str, CASE checkcase) const
{
    if (size() != str.size() || empty())
        return false;

    // if both strings have the same length, then we can compare as a prefix.
    return issameprefix(str, checkcase);
}

bool cview::issameprefix(std::string_view str, CASE checkcase) const
{
    if (str.empty())
        return empty();

    if (empty() || length() < str.length())
        return false;

    if (checkcase == CASE::exact)
    {
        auto iterMain = begin();
        for (auto iterSub: str)
        {
            if (*iterMain++ != iterSub)
                return false;
        }
        return true;
    }
    else if (checkcase == CASE::either)
    {
        auto iterMain = begin();
        for (auto iterSub: str)
        {
            if (std::tolower(*iterMain++) != std::tolower(iterSub))
                return false;
        }
        return true;
    }
    else if (checkcase == CASE::utf8)
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
    assert(!"Unknown CASE value");
    return false;
}

size_t cview::locate(std::string_view str, size_t posStart, CASE checkcase) const
{
    if (str.empty() || posStart >= size())
        return npos;

    if (checkcase == CASE::exact)
        return find(str, posStart);

    if (checkcase == CASE::either)
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
