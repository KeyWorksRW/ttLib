/////////////////////////////////////////////////////////////////////////////
// Name:      ttcvector.cpp
// Purpose:   Vector class for storing ttlib::cstr strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttlibspace.h"
#include "../include/ttcvector.h"

using namespace ttlib;

size_t cstrVector::find(size_t start, std::string_view str, CHECK_CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (ttlib::issameas(at(start), str, checkcase))
            return start;
    }
    return ttlib::npos;
}

size_t cstrVector::findprefix(size_t start, std::string_view str, CHECK_CASE checkcase) const
{
    if (checkcase == CHECK_CASE::yes)
    {
        for (; start < size(); ++start)
        {
            if (ttlib::issamesubstr(at(start), str))
                return start;
        }
    }
    else
    {
        for (; start < size(); ++start)
        {
            if (ttlib::issamesubstri(at(start), str))
                return start;
        }
    }
    return ttlib::npos;
}

size_t cstrVector::contains(size_t start, std::string_view str, CHECK_CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (ttlib::contains(at(start), str, checkcase))
            return start;
    }
    return ttlib::npos;
}
