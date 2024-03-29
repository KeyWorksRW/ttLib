/////////////////////////////////////////////////////////////////////////////
// Name:      tttextfile.cpp
// Purpose:   Classes for reading and writing text files.
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <fstream>

#include "ttlibspace.h"
#include "tttextfile.h"

using namespace ttlib;
using namespace tt;

bool textfile::ReadFile(std::string_view filename)
{
    m_filename.assign(filename);
    clear();
    std::ifstream file(m_filename, std::ios::binary);
    if (!file.is_open())
        return false;
    std::string buf(std::istreambuf_iterator<char>(file), {});
    if (buf.size() > 2)
    {
        // Check for BOM LE or BOM UTF-8 -- other types are not supported.
        if (buf[0] == static_cast<char>(0xFF) && buf[1] == static_cast<char>(0xFE))
        {
            // BOM LE format, so convert to utf-8 before parsing
            auto utf8_buf = ttlib::utf16to8(reinterpret_cast<const wchar_t*>(buf.c_str() + 2));
            ParseLines(utf8_buf);
        }
        else if (buf[0] == static_cast<char>(0xEF) && buf[1] == static_cast<char>(0xBB) && buf[1] == static_cast<char>(0xBF))
        {
            // BOM utf-8 string, so skip over the BOM and process normally
            ParseLines(buf.c_str() + 3);
        }
        else
        {
            ParseLines(buf);
        }
    }
    else
    {
        // A file with only 2 bytes or less is probably worthless, but parse it anyway.
        ParseLines(buf);
    }
    return true;
}

bool textfile::WriteFile(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    for (auto iter: *this)
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

void textfile::ReadArray(const char** begin)
{
    assert(begin);
    if (!begin)
        return;

    while (*begin)
    {
        push_back(*begin);
        ++begin;
    }
}

void textfile::ReadArray(const char** begin, size_t count)
{
    assert(begin && count != tt::npos);
    if (!begin || count == tt::npos)
        return;

    while (count > 0)
    {
        push_back(*begin++);
        --count;
    }
}

void textfile::ParseLines(std::string_view str)
{
    size_t posBeginLine = 0;
    for (size_t pos = 0; pos < str.size(); ++pos)
    {
        if (str[pos] == '\r')
        {
            emplace_back();
            if (pos > posBeginLine)
            {
                back().assign(str.substr(posBeginLine, pos - posBeginLine));
            }
            else
            {
                back().assign(ttlib::emptystring);
            }

            // Some Apple format files only use \r. Windows files tend to use \r\n.
            if (pos + 1 < str.size() && str[pos + 1] == '\n')
                ++pos;
            posBeginLine = pos + 1;
        }
        else if (str[pos] == '\n')
        {
            emplace_back();
            if (pos > posBeginLine)
            {
                back().assign(str.substr(posBeginLine, pos - posBeginLine));
            }
            else
            {
                back().assign(ttlib::emptystring);
            }
            posBeginLine = pos + 1;
        }
    }
}

size_t textfile::FindLineContaining(std::string_view str, size_t start, tt::CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (at(start).contains(str, checkcase))
            return start;
    }
    return tt::npos;
}

size_t textfile::ReplaceInLine(std::string_view orgStr, std::string_view newStr, size_t posLine, tt::CASE checkcase)
{
    for (; posLine < size(); ++posLine)
    {
        if (at(posLine).contains(orgStr, checkcase))
        {
            at(posLine).Replace(orgStr, newStr, false, checkcase);
            return posLine;
        }
    }
    return tt::npos;
}

bool textfile::is_sameas(viewfile other, CASE checkcase) const
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!at(pos).is_sameas(other[pos], checkcase))
            break;
    }
    return (pos == size());
}

bool textfile::is_sameas(textfile other, CASE checkcase) const
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!at(pos).is_sameas(other[pos], checkcase))
            break;
    }
    return (pos == size());
}

/////////////////////// ttViewFile /////////////////////////////////

bool viewfile::ReadFile(std::string_view filename)
{
    m_filename.assign(filename);

    clear();
    std::ifstream file(m_filename, std::ios::binary);
    if (!file.is_open())
        return false;
    m_buffer.assign(std::istreambuf_iterator<char>(file), {});
    if (m_buffer.size() > 2)
    {
        // Check for BOM LE or BOM UTF-8 -- other types are not supported.
        if (m_buffer[0] == static_cast<char>(0xFF) && m_buffer[1] == static_cast<char>(0xFE))
        {
            // BOM LE format, so convert to utf-8 before parsing
            auto utf8_buf = ttlib::utf16to8(reinterpret_cast<const wchar_t*>(m_buffer.c_str() + 2));
            m_buffer.clear();
            m_buffer = std::move(utf8_buf);
            ParseLines(m_buffer);
        }
        else if (m_buffer[0] == static_cast<char>(0xEF) && m_buffer[1] == static_cast<char>(0xBB) &&
                 m_buffer[1] == static_cast<char>(0xBF))
        {
            // BOM utf-8 string, so skip over the BOM and process normally
            ParseLines(m_buffer.c_str() + 3);
        }
        else
        {
            ParseLines(m_buffer);
        }
    }
    else
    {
        // A file with only 2 bytes or less is probably worthless, but parse it anyway.
        ParseLines(m_buffer);
    }

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

bool viewfile::WriteFile(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;
    for (auto iter: *this)
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
            if (pos > posBeginLine)
            {
                emplace_back(str.data() + posBeginLine, pos - posBeginLine);
            }
            else
            {
                emplace_back(nullptr, 0);
            }

            // Some Apple format files only use \r. Windows files tend to use \r\n.
            if (pos + 1 < str.size() && str[pos + 1] == '\n')
                ++pos;
            posBeginLine = pos + 1;
        }
        else if (str[pos] == '\n')
        {
            if (pos > posBeginLine)
            {
                emplace_back(str.data() + posBeginLine, pos - posBeginLine);
            }
            else
            {
                emplace_back(nullptr, 0);
            }
            posBeginLine = pos + 1;
        }
    }
}

void viewfile::ParseBuffer()
{
    clear();

    size_t posBeginLine = 0;
    for (size_t pos = 0; pos < m_buffer.size(); ++pos)
    {
        if (m_buffer[pos] == '\r')
        {
            if (pos > posBeginLine)
            {
                emplace_back(m_buffer.data() + posBeginLine, pos - posBeginLine);
            }
            else
            {
                emplace_back(nullptr, 0);
            }

            // Some Apple format files only use \r. Windows files tend to use \r\n.
            if (pos + 1 < m_buffer.size() && m_buffer[pos + 1] == '\n')
                ++pos;
            posBeginLine = pos + 1;
        }
        else if (m_buffer[pos] == '\n')
        {
            if (pos > posBeginLine)
            {
                emplace_back(m_buffer.data() + posBeginLine, pos - posBeginLine);
            }
            else
            {
                emplace_back(nullptr, 0);
            }
            posBeginLine = pos + 1;
        }
    }
}

size_t viewfile::FindLineContaining(std::string_view str, size_t start, tt::CASE checkcase) const
{
    for (; start < size(); ++start)
    {
        if (ttlib::contains(at(start), str, checkcase))
            return start;
    }
    return tt::npos;
}

bool viewfile::is_sameas(viewfile other, CASE checkcase) const
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!ttlib::is_sameas(at(pos), other[pos], checkcase))
            break;
    }
    return (pos == size());
}

bool viewfile::is_sameas(textfile other, CASE checkcase) const
{
    if (size() != other.size())
        return false;

    size_t pos = 0;
    for (; pos < other.size(); ++pos)
    {
        if (!ttlib::is_sameas(at(pos), other[pos], checkcase))
            break;
    }
    return (pos == size());
}
