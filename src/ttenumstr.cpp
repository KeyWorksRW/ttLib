/////////////////////////////////////////////////////////////////////////////
// Name:      ttCEnumStr
// Purpose:   Enumerate through substrings in a string
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ttenumstr.h"  // ttEnumStr, ttEnumView -- Enumerate through substrings in a string

using namespace ttlib;

enumstr::enumstr(std::string_view str, char separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        emplace_back();
        back().assign(str.substr(start, end - start));

        start = end + 1;
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    emplace_back();
    back().assign(str.substr(start));
}

enumview::enumview(std::string_view str, char separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        push_back(str.substr(start, end - start));

        start = end + 1;
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    push_back(str.substr(start));
}

ttEnumStr::ttEnumStr(std::string_view str, char separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        emplace_back();
        back().assign(str.substr(start, end - start));

        start = end + 1;
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    emplace_back();
    back().assign(str.substr(start));
}

ttEnumView::ttEnumView(std::string_view str, char separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        push_back(str.substr(start, end - start));

        start = end + 1;
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    push_back(str.substr(start));
}
