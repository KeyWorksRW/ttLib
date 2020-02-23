/////////////////////////////////////////////////////////////////////////////
// Name:      ttpath.cpp
// Purpose:   Contains functions for working with filesystem::path and filesystem::directory
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttnamespace.h"

// Define a namespace so that either std::filesystem or boost::filesystem can be used
namespace fs = std::filesystem;

std::string_view tt::findext(std::string_view str)
{
    auto pos = str.rfind('.');
    if (pos == std::string_view::npos)
        return {};
    else if (pos + 1 >= str.length())  // . by itself is a folder
        return {};
    else if (pos < 2 && (str.at(pos + 1) == '.'))
        return {};

    return str.substr(pos);
}

bool tt::isvalidfilechar(std::string_view str, size_t pos)
{
    if (str.empty() || pos > str.length())
        return false;

    switch (str.at(pos))
    {
        case '.':
            if (pos + 1 >= str.length())  // . by itself is a folder
                return false;
            if (pos < 2 && (str.at(pos + 1) == '.'))
                return false;  // ".." is a folder
            return true;

        case '<':
        case '>':
        case ':':
        case '/':
        case '\\':
        case '|':
        case '?':
        case '*':
        case 0:
            return false;
    }
    return true;
}

void tt::backslashestoforward(std::string& str)
{
    for (auto pos = str.find('\\'); pos != std::string::npos; pos = str.find('\\'))
    {
        str.replace(pos, 1, "/");
    }
}

bool tt::hasextension(std::filesystem::directory_entry name, std::string_view extension)
{
    if (!name.is_directory())
    {
        auto ext = name.path().extension();
        if (ext.empty())
            return false;
        return tt::issamestri(ext.string(), extension);
    }
    return false;
}

bool tt::ChangeDir(std::string_view newdir)
{
    if (newdir.empty())
        return false;
    try
    {
        // BUGBUG: [KeyWorks - 02-22-2020] On Windows, assume newdir is a UTF8 string, and convert it to
        // a wstring before handing it to path.
        auto dir = std::filesystem::directory_entry(std::filesystem::path(newdir));
        if (dir.exists() && dir.is_directory())
        {
            std::filesystem::current_path(dir);
            return true;
        }
    }
    catch (const std::exception& /* e */)
    {
    }
    return false;
}

bool tt::dirExists(std::string_view dir)
{
    if (dir.empty())
        return false;
    try
    {
        auto path = std::filesystem::directory_entry(std::filesystem::path(dir));
        return (path.exists() && path.is_directory());
    }
    catch (const std::exception& /* e */)
    {
    }
    return false;
}

bool tt::fileExists(std::string_view filename)
{
    if (filename.empty())
        return false;
    try
    {
        auto path = std::filesystem::directory_entry(std::filesystem::path(filename));
        return (path.exists() && !path.is_directory());
    }
    catch (const std::exception& /* e */)
    {
    }
    return false;
}
