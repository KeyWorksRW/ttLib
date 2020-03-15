/////////////////////////////////////////////////////////////////////////////
// Name:      ttcvector.cpp
// Purpose:   Vector class for storing ttlib::cstr strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ttlibspace.h"
#include "ttcvector.h"

using namespace ttlib;
using namespace tt;

size_t cstrVector::find(size_t start, std::string_view str, CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (ttlib::issameas(at(start), str, checkcase))
            return start;
    }
    return tt::npos;
}

size_t cstrVector::findprefix(size_t start, std::string_view str, CASE checkcase) const
{
    if (checkcase == CASE::exact)
    {
        for (; start < size(); ++start)
        {
            if (ttlib::issameprefix(at(start), str, checkcase))
                return start;
        }
    }
    else
    {
        for (; start < size(); ++start)
        {
            if (ttlib::issameprefix(at(start), str, checkcase))
                return start;
        }
    }
    return tt::npos;
}

size_t cstrVector::contains(size_t start, std::string_view str, CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (ttlib::contains(at(start), str, checkcase))
            return start;
    }
    return tt::npos;
}
