/////////////////////////////////////////////////////////////////////////////
// Name:      ttmultistr.cpp
// Purpose:   Breaks a single string into multiple strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ttmultistr.h"

using namespace ttlib;

multistr::multistr(std::string_view str, char separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        emplace_back();
        back().assign(str.substr(start, end - start));

        start = end + sizeof(char);
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    emplace_back();
    back().assign(str.substr(start));
}

multistr::multistr(std::string_view str, std::string_view separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        emplace_back();
        back().assign(str.substr(start, end - start));

        start = end + separator.size();
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    emplace_back();
    back().assign(str.substr(start));
}

multiview::multiview(std::string_view str, char separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        push_back(str.substr(start, end - start));

        start = end + sizeof(char);
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    push_back(str.substr(start));
}

multiview::multiview(std::string_view str, std::string_view separator)
{
    size_t start = 0;
    size_t end = str.find_first_of(separator);
    while (end != std::string_view::npos)
    {
        push_back(str.substr(start, end - start));

        start = end + separator.size();
        if (start >= str.length())
            return;
        end = str.find_first_of(separator, start);
    }
    push_back(str.substr(start));
}
