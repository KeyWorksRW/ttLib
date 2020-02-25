/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::textfile, ttlib::viewfile
// Purpose:   Classes for reading and writing text files.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <fstream>

#include "../include/ttlibspace.h"
#include "../include/tttextfile.h"

using namespace ttlib;

bool textfile::ReadFile(std::string_view filename)
{
    clear();
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    std::string buf(std::istreambuf_iterator<char>(file), {});
    ParseLines(buf);
    return true;
}

bool textfile::WriteFile(std::string_view filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    for (auto iter : *this)
    {
        file << iter << '\n';
    }

    return true;
}

void textfile::ReadString(std::string_view str)
{
    if (!str.empty())
        ParseLines(str);
}

void textfile::ParseLines(std::string_view str)
{
    size_t posBeginLine = 0;
    for (size_t pos = 0; pos < str.size(); ++pos)
    {
        if (str[pos] == '\r')
        {
            ttlib::cstr line;
            if (pos > posBeginLine)
            {
                line.assign(str.substr(posBeginLine, pos - posBeginLine));
            }
            else
            {
                line.assign(ttlib::emptystring);
            }

            push_back(move(line));

            // Some Apple format files only use \r. Windows files tend to use \r\n.
            if (pos + 1 < str.size() && str[pos + 1] == '\n')
                ++pos;
            posBeginLine = pos + 1;
        }
        else if (str[pos] == '\n')
        {
            ttlib::cstr line;
            if (pos > posBeginLine)
            {
                line.assign(str.substr(posBeginLine, pos - posBeginLine));
            }
            else
            {
                line.assign(ttlib::emptystring);
            }
            push_back(move(line));
            posBeginLine = pos + 1;
        }
    }
}

size_t textfile::FindLineContaining(std::string_view str, size_t start, ttlib::CHECK_CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (ttlib::contains(at(start), str, checkcase))
            return start;
    }
    return ttlib::npos;
}

bool textfile::issameas(viewfile other, CHECK_CASE checkcase)
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!ttlib::issameas(at(pos), other[pos], checkcase))
            break;
    }
    return (pos == size());
}

bool textfile::issameas(textfile other, CHECK_CASE checkcase)
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!ttlib::issameas(at(pos), other[pos], checkcase))
            break;
    }
    return (pos == size());
}

/////////////////////// ttViewFile /////////////////////////////////

bool viewfile::ReadFile(std::string_view filename)
{
    clear();
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    m_buffer.assign(std::istreambuf_iterator<char>(file), {});
    ParseLines(m_buffer);
    return true;
}

void viewfile::ReadString(std::string_view str)
{
    if (!str.empty())
    {
        m_buffer.assign(str);
        ParseLines(m_buffer);
    }
}

bool viewfile::WriteFile(std::string_view filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    for (auto iter : *this)
    {
        file << iter << '\n';
    }

    return true;
}

void viewfile::ParseLines(std::string_view str)
{
    size_t posBeginLine = 0;
    for (size_t pos = 0; pos < str.size(); ++pos)
    {
        if (str[pos] == '\r')
        {
            std::string_view view;
            if (pos > posBeginLine)
            {
                view = str.substr(posBeginLine, pos - posBeginLine);
            }
            else
            {
                view = {};
            }

            push_back(view);

            // Some Apple format files only use \r. Windows files tend to use \r\n.
            if (pos + 1 < str.size() && str[pos + 1] == '\n')
                ++pos;
            posBeginLine = pos + 1;
        }
        else if (str[pos] == '\n')
        {
            std::string_view view;
            if (pos > posBeginLine)
            {
                view = str.substr(posBeginLine, pos - posBeginLine);
            }
            else
            {
                view = {};
            }
            push_back(view);
            posBeginLine = pos + 1;
        }
    }
}

size_t viewfile::FindLineContaining(std::string_view str, size_t start, ttlib::CHECK_CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (ttlib::contains(at(start), str, checkcase))
            return start;
    }
    return ttlib::npos;
}

bool viewfile::issameas(viewfile other, CHECK_CASE checkcase)
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!ttlib::issameas(at(pos), other[pos], checkcase))
            break;
    }
    return (pos == size());
}

bool viewfile::issameas(textfile other, CHECK_CASE checkcase)
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!ttlib::issameas(at(pos), other[pos], checkcase))
            break;
    }
    return (pos == size());
}
