/////////////////////////////////////////////////////////////////////////////
// Name:      ttTextFile
// Purpose:   Class for reading and writing text files.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <exception>
#include <fstream>

#include "../include/tttextfile.h"

bool ttTextFile::ReadFile(std::string_view filename)
{
    clear();
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    std::string buf(std::istreambuf_iterator<char>(file), {});
    ParseLines(buf);
    return true;
}

bool ttTextFile::WriteFile(std::string_view filename)
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

void ttTextFile::ReadString(std::string_view str)
{
    if (!str.empty())
        ParseLines(str);
}

void ttTextFile::ParseLines(std::string_view str)
{
    size_t posBeginLine = 0;
    for (size_t pos = 0; pos < str.size(); ++pos)
    {
        if (str[pos] == '\r')
        {
            ttString line;
            if (pos > posBeginLine)
            {
                line.assign(str.substr(posBeginLine, pos - posBeginLine));
            }
            else
            {
                line.assign(ttEmptyString);
            }

            push_back(line);

            // Some Apple format files only use \r. Windows files tend to use \r\n.
            if (pos + 1 < str.size() && str[pos + 1] == '\n')
                ++pos;
            posBeginLine = pos + 1;
        }
        else if (str[pos] == '\n')
        {
            ttString line;
            if (pos > posBeginLine)
            {
                line.assign(str.substr(posBeginLine, pos - posBeginLine));
            }
            else
            {
                line.assign(ttEmptyString);
            }
            push_back(line);
            posBeginLine = pos + 1;
        }
    }
}

size_t ttTextFile::FindLineContaining(size_t start, std::string_view str, bool CaseSensitive)
{
    for (; start < size(); ++start)
    {
        if (tt::contains(at(start), str, CaseSensitive))
            return start;
    }
    return tt::npos;
}


/////////////////////// ttViewFile /////////////////////////////////

bool ttViewFile::ReadFile(std::string_view filename)
{
    clear();
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    m_buffer.assign(std::istreambuf_iterator<char>(file), {});
    ParseLines(m_buffer);
    return true;
}

void ttViewFile::ReadString(std::string_view str)
{
    if (!str.empty())
    {
        m_buffer.assign(str);
        ParseLines(m_buffer);
    }
}

bool ttViewFile::WriteFile(std::string_view filename)
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

void ttViewFile::ParseLines(std::string_view str)
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

size_t ttViewFile::FindLineContaining(size_t start, std::string_view str, bool CaseSensitive)
{
    for (; start < size(); ++start)
    {
        if (tt::contains(at(start), str, CaseSensitive))
            return start;
    }
    return tt::npos;
}
