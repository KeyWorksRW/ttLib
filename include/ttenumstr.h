/////////////////////////////////////////////////////////////////////////////
// Name:      ttEnumStr
// Purpose:   Enumerate through substrings in a string
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/**
    This class is used to enumerate through substrings that are separated by a single character (typically a
    semicolon). The following example uses this to parse through each of the directories in an INCLUDE environment
    variable:

        ttEnumView enumPaths("c:/sdk/include;c:/github/wxwidgets/include/wx");
        for (auto path : enumPaths)
            std::cout << path << '\n'
*/

#pragma once

#include "ttstring.h"

/// @file
/// These two classes break a string containing substrings into a vector of substrings.
/// Use ttEnumStr if you want a copy of the substring that can be modified. Use ttEnumView
/// if you just need to iterate through the substrings.
///
/// An example usage is getting the PATH$ envionment variable which contains multiple
/// paths separated by a semicolon. Handing the PATH$ string to either of these classes
/// would give you a vector of each individual path.

class ttEnumStr : public std::vector<ttString>
{
public:
    ttEnumStr(std::string_view str, char separator = ';');
};

class ttEnumView : public std::vector<std::string_view>
{
public:
    ttEnumView(std::string_view str, char separator = ';');
};
