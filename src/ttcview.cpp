/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::cview
// Purpose:   string_view functionality on a zero-terminated char string.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <cctype>
#include <locale>

#include "../include/ttlibspace.h"
#include "../include/ttcview.h"

using namespace ttlib;

template<>
bool cview::viewspace()
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

template<>
bool cview::viewnonspace()
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

template<>
bool cview::viewnextword()
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

template<>
bool cview::viewdigit()
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

template<>
bool cview::viewnondigit()
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

template<>
bool cview::viewextension()
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

template<>
bool cview::viewfilename()
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
