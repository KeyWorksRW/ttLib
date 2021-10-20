/////////////////////////////////////////////////////////////////////////////
// Name:      ttsvector.cpp
// Purpose:   Vector class for storing ttString strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "ttsvector.h"

#include "ttlibspace.h"  // ttlib namespace functions and declarations

using namespace ttlib;
using namespace tt;

size_t strVector::find(size_t start, ttString& str, CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (at(start).is_sameas_wx(str, checkcase))
            return start;
    }
    return tt::npos;
}

size_t strVector::findprefix(size_t start, ttString& str, CASE checkcase) const
{
    if (checkcase == CASE::exact)
    {
        for (; start < size(); ++start)
        {
            if (at(start).is_sameprefix_wx(str, checkcase))
                return start;
        }
    }
    else
    {
        for (; start < size(); ++start)
        {
            if (at(start).is_sameprefix_wx(str, checkcase))
                return start;
        }
    }
    return tt::npos;
}

size_t strVector::contains(size_t start, ttString& str, CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (at(start).contains_wx(str, checkcase))
            return start;
    }
    return tt::npos;
}
