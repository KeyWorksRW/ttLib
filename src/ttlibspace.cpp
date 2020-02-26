/////////////////////////////////////////////////////////////////////////////
// Name:      ttnamespace.cpp
// Purpose:   ttlib namespace functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <locale>
#include <cassert>
#include <cctype>

#include "../include/ttlibspace.h"
#include "../include/utf8unchecked.h"

using namespace ttlib;

// Global empty string.
const std::string ttlib::emptystring("");

const char* ttlib::nextut8fchar(const char* psz) noexcept
{
    if (!psz)
        return nullptr;
    if (!*psz)
        return psz;
    size_t i = 0;
    (void) (ttlib::isutf8(psz[++i]) || ttlib::isutf8(psz[++i]) || ttlib::isutf8(psz[++i]));

    return psz + i;
}

size_t ttlib::gethash(std::string_view str) noexcept
{
    if (str.empty())
        return 0;

    // djb2 hash algorithm

    size_t hash = 5381;

    for (auto iter : str)
        hash = ((hash << 5) + hash) ^ iter;

    return hash;
}

std::string_view ttlib::findspace(std::string_view str) noexcept
{
    if (str.empty())
        return {};
    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (ttlib::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};
    else
        return str.substr(pos);
}

size_t ttlib::findspace_pos(std::string_view str)
{
    auto view = ttlib::findspace(str);
    if (view.empty())
        return ttlib::npos;
    else
        return (str.size() - view.size());
}

std::string_view ttlib::findnonspace(std::string_view str) noexcept
{
    if (str.empty())
        return {};
    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (!ttlib::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};
    else
        return str.substr(pos);
}

size_t ttlib::findnonspace_pos(std::string_view str) noexcept
{
    auto view = ttlib::findnonspace(str);
    if (view.empty())
        return ttlib::npos;
    else
        return (str.size() - view.size());
}

std::string_view ttlib::stepover(std::string_view str)
{
    if (str.empty())
        return {};

    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (ttlib::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};

    for (; pos < str.length(); ++pos)
    {
        if (!ttlib::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};
    else
        return str.substr(pos);
}

size_t ttlib::stepover_pos(std::string_view str) noexcept
{
    auto view = ttlib::stepover(str);
    if (view.empty())
        return ttlib::npos;
    else
        return (str.size() - view.size());
}

bool ttlib::issameprefix(std::string_view strMain, std::string_view strSub, CASE checkcase)
{
    if (strSub.empty())
        return strMain.empty();

    if (strMain.empty() || strMain.length() < strSub.length())
        return false;

    if (checkcase == CASE::exact)
    {
        auto iterMain = strMain.begin();
        for (auto iterSub : strSub)
        {
            if (*iterMain++ != iterSub)
                return false;
        }
        return true;
    }
    else if (checkcase == CASE::either)
    {
        auto iterMain = strMain.begin();
        for (auto iterSub : strSub)
        {
            if (std::tolower(*iterMain++) != std::tolower(iterSub))
                return false;
        }
        return true;
    }
    else if (checkcase == CASE::utf8)
    {
        auto utf8locale = std::locale("en_US.utf8");
        auto iterMain = strMain.begin();
        for (auto iterSub : strSub)
        {
            if (std::tolower(*iterMain++, utf8locale) != std::tolower(iterSub, utf8locale))
                return false;
        }
        return true;
    }
    assert(!"Unknown CASE value");
    return false;
}

bool ttlib::issamesubstr(std::string_view strMain, std::string_view strSub)
{
    if (strSub.empty())
        return strMain.empty();

    if (strMain.empty() || strMain.length() < strSub.length())
        return false;

    for (auto pos = 0U; pos < strSub.length(); ++pos)
    {
        if (strMain.at(pos) != strSub.at(pos))
            return false;
    }
    return true;
}

bool ttlib::issamesubstri(std::string_view strMain, std::string_view strSub)
{
    if (strSub.empty())
        return strMain.empty();

    if (strMain.empty() || strMain.length() < strSub.length())
        return false;

    for (auto pos = 0U; pos < strSub.length(); ++pos)
    {
        if (std::tolower(strMain.at(pos)) != std::tolower(strSub.at(pos)))
            return false;
    }
    return true;
}

std::string_view ttlib::strstri(std::string_view strMain, std::string_view strSub)
{
    if (strSub.empty())
        return {};
    auto chLower = std::tolower(strSub[0]);

    for (auto pos = 0U; pos < strMain.length(); ++pos)
    {
        if (std::tolower(strMain.at(pos)) == chLower)
        {
            size_t posSub;
            for (posSub = 1; posSub < strSub.length(); ++posSub)
            {
                if (pos + posSub >= strMain.length())
                    return {};
                if (std::tolower(strMain.at(pos + posSub)) != std::tolower(strSub.at(posSub)))
                    break;
            }
            if (posSub >= strSub.length())
                return strMain.substr(pos);
        }
    }
    return {};
}

std::string_view ttlib::findstr(std::string_view main, std::string_view sub, CASE checkcase)
{
    if (sub.empty())
        return {};

    if (checkcase == CASE::exact)
    {
        auto pos = main.find(sub);
        if (pos < main.length())
            return main.substr(pos);

        return {};
    }

    auto chLower = std::tolower(sub[0]);

    for (auto pos = 0U; pos < main.length(); ++pos)
    {
        if (std::tolower(main.at(pos)) == chLower)
        {
            size_t posSub;
            for (posSub = 1; posSub < sub.length(); ++posSub)
            {
                if (pos + posSub >= main.length())
                    return {};
                if (std::tolower(main.at(pos + posSub)) != std::tolower(sub.at(posSub)))
                    break;
            }
            if (posSub >= sub.length())
                return main.substr(pos);
        }
    }
    return {};
}

size_t ttlib::findstr_pos(std::string_view main, std::string_view sub, CASE checkcase)
{
    auto view = ttlib::findstr(main, sub, checkcase);
    if (view.empty())
        return ttlib::npos;
    else
        return (main.size() - view.size());
}

bool ttlib::contains(std::string_view main, std::string_view sub, CASE checkcase)
{
    return !ttlib::findstr(main, sub, checkcase).empty();
}

std::string_view ttlib::strstr(std::string_view strMain, std::string_view strSub)
{
    if (strSub.empty())
        return {};

    auto pos = strMain.find(strSub);
    if (pos < strMain.length())
        return strMain.substr(pos);

    return {};
}

bool ttlib::issamestri(std::string_view str1, std::string_view str2)
{
    if (str1.empty())
    {
        return str2.empty();
    }
    else if (str2.empty())
    {
        return false;
    }

    auto main = str1.begin();
    auto sub = str2.begin();
    while (sub != str2.end())
    {
        auto diff = std::tolower(main[0]) - std::tolower(sub[0]);
        if (diff != 0)
            return false;
        ++main;
        ++sub;
        if (main == str1.end())
            return (sub != str2.end() ? false : true);
    }

    return (main != str1.end() ? false : true);
}

bool ttlib::issameas(std::string_view str1, std::string_view str2, CASE checkcase)
{
    if (str1.empty())
    {
        return str2.empty();
    }
    else if (str2.empty())
    {
        return false;
    }

    if (checkcase == CASE::exact)
        return (str1.compare(str2) == 0);

    auto main = str1.begin();
    auto sub = str2.begin();
    while (sub != str2.end())
    {
        auto diff = std::tolower(main[0]) - std::tolower(sub[0]);
        if (diff != 0)
            return false;
        ++main;
        ++sub;
        if (main == str1.end())
            return (sub != str2.end() ? false : true);
    }

    return (main != str1.end() ? false : true);
}

int ttlib::atoi(std::string_view str) noexcept
{
    assert(!str.empty());
    if (str.empty())
        return 0;

    str = ttlib::findnonspace(str);

    int total = 0;
    size_t pos = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        pos += 2;  // skip over 0x prefix in hexadecimal strings

        for (; pos < str.size(); ++pos)
        {
            auto c = str[pos];
            if (c >= '0' && c <= '9')
                total = 16 * total + (c - '0');
            else if (c >= 'a' && c <= 'f')
                total = total * 16 + c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                total = total * 16 + c - 'A' + 10;
            else
                break;
        }
        return total;
    }

    bool negative = false;
    if (str[pos] == '-')
    {
        negative = true;
        ++pos;
    }
    else if (str[pos] == '+')
    {
        negative = false;
        ++pos;
    }

    for (; pos < str.size() && str[pos] >= '0' && str[pos] <= '9'; ++pos)
    {
        total = 10 * total + (str[pos] - '0');
    }

    return (negative ? -total : total);
}

std::string_view ttlib::findext(std::string_view str)
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

bool ttlib::isvalidfilechar(std::string_view str, size_t pos)
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

void ttlib::backslashestoforward(std::string& str)
{
    for (auto pos = str.find('\\'); pos != std::string::npos; pos = str.find('\\'))
    {
        str.replace(pos, 1, "/");
    }
}

bool ttlib::hasextension(std::filesystem::directory_entry name, std::string_view extension, CASE checkcase)
{
    if (!name.is_directory())
    {
        auto ext = name.path().extension();
        if (ext.empty())
            return false;
        return ttlib::issameas(ext.string(), extension, checkcase);
    }
    return false;
}

bool ttlib::ChangeDir(std::string_view newdir)
{
    if (newdir.empty())
        return false;
    try
    {
#if defined(_WIN32)
        std::wstring str16;
        utf8::unchecked::utf8to16(newdir.begin(), newdir.end(), back_inserter(str16));
        auto dir = std::filesystem::directory_entry(std::filesystem::path(str16));
#else
        auto dir = std::filesystem::directory_entry(std::filesystem::path(newdir));
#endif
        if (dir.exists())
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

bool ttlib::dirExists(std::string_view dir)
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

bool ttlib::fileExists(std::string_view filename)
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
