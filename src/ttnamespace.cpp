/////////////////////////////////////////////////////////////////////////////
// Name:      ttnamespace.cpp
// Purpose:   tt namespace functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <locale>
#include <cassert>
#include <cctype>

#include "ttnamespace.h"

// Global empty string.
const std::string ttEmptyString("");

const char* tt::nextchar(const char* psz)
{
    if (!psz)
        return nullptr;
    if (!*psz)
        return psz;
    size_t i = 0;
    (void) (tt::isutf8(psz[++i]) || tt::isutf8(psz[++i]) || tt::isutf8(psz[++i]));

    return psz + i;
}

size_t tt::gethash(std::string_view str)
{
    if (str.empty())
        return 0;

    // djb2 hash algorithm

    size_t hash = 5381;

    for (auto iter : str)
        hash = ((hash << 5) + hash) ^ iter;

    return hash;
}

std::string_view tt::findspace(std::string_view str)
{
    if (str.empty())
        return {};
    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (tt::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};
    else
        return str.substr(pos);
}

size_t tt::findspace_pos(std::string_view str)
{
    auto view = tt::findspace(str);
    if (view.empty())
        return tt::npos;
    else
        return (str.size() - view.size());
}

std::string_view tt::findnonspace(std::string_view str)
{
    if (str.empty())
        return {};
    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (!tt::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};
    else
        return str.substr(pos);
}

size_t tt::findnonspace_pos(std::string_view str)
{
    auto view = tt::findnonspace(str);
    if (view.empty())
        return tt::npos;
    else
        return (str.size() - view.size());
}

std::string_view tt::stepover(std::string_view str)
{
    if (str.empty())
        return {};

    size_t pos;
    for (pos = 0; pos < str.length(); ++pos)
    {
        if (tt::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};

    for (; pos < str.length(); ++pos)
    {
        if (!tt::iswhitespace(str.at(pos)))
            break;
    }
    if (pos >= str.length())
        return {};
    else
        return str.substr(pos);
}

size_t tt::stepover_pos(std::string_view str)
{
    auto view = tt::stepover(str);
    if (view.empty())
        return tt::npos;
    else
        return (str.size() - view.size());
}

bool tt::issamesubstr(std::string_view strMain, std::string_view strSub)
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

bool tt::issamesubstri(std::string_view strMain, std::string_view strSub)
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

std::string_view tt::strstri(std::string_view strMain, std::string_view strSub)
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

std::string_view tt::findstr(std::string_view main, std::string_view sub, CASE checkcase)
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

size_t tt::findstr_pos(std::string_view main, std::string_view sub, CASE checkcase)
{
    auto view = tt::findstr(main, sub, checkcase);
    if (view.empty())
        return tt::npos;
    else
        return (main.size() - view.size());
}

bool tt::contains(std::string_view main, std::string_view sub, CASE checkcase)
{
    return !tt::findstr(main, sub, checkcase).empty();
}

std::string_view tt::strstr(std::string_view strMain, std::string_view strSub)
{
    if (strSub.empty())
        return {};

    auto pos = strMain.find(strSub);
    if (pos < strMain.length())
        return strMain.substr(pos);

    return {};
}

bool tt::issamestri(std::string_view str1, std::string_view str2)
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

bool tt::issameas(std::string_view str1, std::string_view str2, CASE checkcase)
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

int tt::atoi(std::string_view str)
{
    assert(!str.empty());
    if (str.empty())
        return 0;

    str = tt::findnonspace(str);

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
