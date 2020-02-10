/////////////////////////////////////////////////////////////////////////////
// Name:      ttString
// Purpose:   std::string class with some additional functionality
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <locale>
#include <cassert>
#include <cctype>

#include "../include/ttstring.h"

// Global empty string.
const std::string ttEmptyString("");

bool ttString::issamesubstr(std::string_view str) const
{
    if (str.empty())
        return empty();

    if (empty() || length() < str.length())
        return false;

    auto iterMain = begin();
    for (auto iterSub : str)
    {
        if (*iterMain++ != iterSub)
            return false;
    }
    return true;
}

bool ttString::issamesubstri(std::string_view str) const
{
    if (str.empty())
        return empty();

    if (empty() || length() < str.length())
        return false;

    auto iterMain = begin();
    for (auto iterSub : str)
    {
        if (std::tolower(*iterMain++) != std::tolower(iterSub))
            return false;
    }
    return true;
}

int ttString::comparei(std::string_view str) const
{
    auto main = begin();
    auto sub = str.begin();
    while (sub != str.end())
    {
        auto diff = std::tolower(main[0]) - std::tolower(sub[0]);
        if (diff != 0)
            return diff;
        ++main;
        ++sub;
        if (main == end())
            return (sub != str.end() ? -1 : 0);
    }

    return (main != end() ? 1 : 0);
}

void ttString::trim(bool fromRight)
{
    if (empty())
        return;
    if (fromRight)
    {
        auto len = length();
        for (--len; len != std::string::npos; --len)
        {
            char ch = at(len);
            if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '\f')
                break;
        }

        if (len + 1 < length())
            erase(len + 1, length() - len);
    }
    else
    {
        // Assume that most strings won't start with whitespace, so return as quickly as possible if that is the
        // case.
        if (!tt::iswhitespace(at(0)))
            return;

        size_t pos;
        for (pos = 1; pos < length(); ++pos)
        {
            if (!tt::iswhitespace(at(pos)))
                break;
        }
        replace(0, length(), substr(pos, length() - pos));
    }
}

/**
 * @param pszString -- pointer to the string to parse
 * @param chBegin -- character that prefixes the string
 * @param chEnd -- character that terminates the string.
 */
size_t ttString::AssignSubString(std::string_view src, char chBegin, char chEnd)
{
    if (src.empty())
    {
        assign(ttEmptyString);
        return npos;
    }

    size_t pos = 0;
    // step over any leading whitespace unless chBegin is a whitespace character
    if (!tt::iswhitespace(chBegin))
    {
        while (tt::iswhitespace(src[pos]))
            ++pos;
    }

    if (src[pos] == chBegin)
    {
        ++pos;
        auto start = pos;
        while (pos < src.length() && src[pos] != chEnd)
        {
            // REVIEW: [KeyWorks - 01-26-2020] '\"' is also valid for the C compiler, though the slash
            // is unnecessary. Should we support it?

            // only check quotes -- a slash is valid before other character pairs.
            if (src[pos] == '\\' && (chBegin == '"' || chBegin == '\'') && pos + 1 < src.length() &&
                (src[pos + 1] == chEnd))
            {
                // step over an escaped quote if the string to fetch is within a quote
                pos += 2;
                continue;
            }
            ++pos;
        }
        insert(0, src.substr(start, pos - start));
    }
    else
    {
        // if the string didn't start with chBegin, just copy the string
        *this = src;
        return src.length();
    }
    return pos;
}

/**
 * @param str -- the string to extract from
 * @param start -- the offset into the string to begin extraction from -- this should either be to leading
 *                 whitespace, or to the character that marks the beginning of the sub string.
 * @return The offset to the character that terminated extraction, or **npos** if a terminating
 *         character could not be found.
 */
size_t ttString::ExtractSubString(std::string_view src, size_t start)
{
    if (src.empty())
    {
        assign(ttEmptyString);
        return npos;
    }

    // start by finding the first non-whitespace character
    size_t pos = start;
    while (pos < src.length() && tt::iswhitespace(src[pos]))
    {
        ++pos;
    }

    if (pos >= src.length())
        return npos;

    // based on the opening character, determine what the matching end character should be
    char chEnd;
    char chBegin = src[pos];
    if (chBegin == '`' || chBegin == '\'')
        chEnd = '\'';
    else if (chBegin == '"')
        chEnd = '"';
    else if (chBegin == '<')
        chEnd = '>';
    else if (chBegin == '[')
        chEnd = '>';
    else if (chBegin == ']')
        chEnd = ')';
    else
        return npos;

    // now find the substring between chBegin and chEnd
    auto startPos = pos + 1;
    for (++pos; pos < src.length(); ++pos)
    {
        // only check quotes -- a slash is valid before other character pairs.
        if (src[pos] == '\\' && (chBegin == '"' || chBegin == '\'') && pos + 1 < src.length() &&
            (src[pos + 1] == chEnd))
        {
            // step over an escaped quote if the string to fetch is within a quote
            ++pos;
            continue;
        }
        else if (src[pos] == chEnd)
        {
            break;
        }
    }

    if (pos >= src.length())
        return npos;

    assign(src.substr(startPos, pos - startPos));
    return pos;
}

/**
 * @param oldtext -- the text to search for
 * @param newtext -- the text to replace it with
 * @param replaceAll -- replace all occurrences or just the first one
 * @param CaseSensitive -- indicates whether or not to use a case-insensitive search
 * @param Utf8 -- set to true if bCaseSensitive is false and strings are UTF8
 * @return Number of replacements made
 */
size_t ttString::Replace(std::string_view oldtext, std::string_view newtext, bool replaceAll, bool CaseSensitive,
                         bool Utf8)
{
    if (oldtext.empty())
        return false;

    size_t replacements = 0;
    size_t pos = CaseSensitive ? find(oldtext) : findi(oldtext, Utf8);

    do
    {
        if (pos == npos)
            return replacements;

        erase(pos, oldtext.length());
        insert(pos, newtext);
        ++replacements;
        if (replaceAll)
        {
            pos = CaseSensitive ? find(oldtext) : findi(oldtext, Utf8);
        }
    } while (replaceAll);

    return replacements;
}

size_t ttString::findi(std::string_view str, bool bUtf8) const
{
    if (str.empty())
        return npos;
    auto chLower = std::tolower(str[0]);

    if (!bUtf8)
    {
        for (auto pos = 0U; pos < length(); ++pos)
        {
            if (std::tolower(at(pos)) == chLower)
            {
                size_t posSub;
                for (posSub = 1; posSub < str.length(); ++posSub)
                {
                    if (pos + posSub >= length())
                        return npos;
                    if (std::tolower(at(pos + posSub)) != std::tolower(str.at(posSub)))
                        break;
                }
                if (posSub >= str.length())
                    return pos;
            }
        }
    }
    else
    {
        auto utf8locale = std::locale("en_US.utf8");
        for (auto pos = 0U; pos < length(); ++pos)
        {
            if (std::tolower(at(pos), utf8locale) == chLower)
            {
                size_t posSub;
                for (posSub = 1; posSub < str.length(); ++posSub)
                {
                    if (pos + posSub >= length())
                        return npos;
                    if (std::tolower(at(pos + posSub), utf8locale) != std::tolower(str.at(posSub), utf8locale))
                        break;
                }
                if (posSub >= str.length())
                    return pos;
            }
        }
    }
    return npos;
}

bool ttString::contains(std::string_view sub, bool CaseSensitive)
{
    return tt::contains(*this, sub, CaseSensitive);
}

size_t ttString::gethash() const
{
    if (empty())
        return 0;

    // djb2 hash algorithm

    size_t hash = 5381;

    for (auto iter : *this)
    {
        hash = ((hash << 5) + hash) ^ iter;
    }

    return hash;
}

std::string_view ttString::MakeLower()
{
    auto utf8locale = std::locale("en_US.utf8");
    for (auto iter = begin(); iter != end(); ++iter)
    {
        *iter = std::tolower(*iter, utf8locale);
    }
    return *this;
}

std::string_view ttString::MakeUpper()
{
    auto utf8locale = std::locale("en_US.utf8");
    for (auto iter = begin(); iter != end(); ++iter)
    {
        *iter = std::toupper(*iter, utf8locale);
    }
    return *this;
}

/// Converts all backslashes in the string to forward slashes.
///
/// Note: Windows handles paths that use forward slashes, so backslashes are normally
/// unnecessary.
void ttString::backslashestoforward()
{
    for (auto pos = find('\\'); pos != std::string::npos; pos = find('\\'))
    {
        replace(pos, 1, "/");
    }
}

std::string_view ttString::replace_extension(std::string_view newExtension)
{
    if (empty())
    {
        if (newExtension.empty())
            return *this;
        if (newExtension.at(0) != '.')
            push_back('.');
        append(newExtension);
        return *this;
    }

    auto pos = find_last_of('.');
    if (pos != npos)
    {
        // If the string only contains . or .. then it is a folder
        if (pos == 0 || (pos == 1 && at(0) != '.'))
            return *this;  // can't add an extension if it isn't a valid filename

        if (newExtension.empty())
        {
            // If the new extension is empty, then just erase the old extension.
            erase(pos);
        }
        else
        {
            // If the new extension doesn't start with '.', then keep our own '.' prefix.
            if (newExtension.at(0) != '.')
                ++pos;
            replace(pos, length() - pos, newExtension);
        }
    }
    else
    {
        // Current filename doesn't have an extension, so append the new one
        if (newExtension.at(0) != '.')
            push_back('.');
        append(newExtension);
    }

    return *this;
}

std::string_view ttString::extension() const
{
    if (empty())
        return {};

    auto pos = find_last_of('.');
    if (pos == npos)
        return {};

    // . by itself is a folder
    else if (pos + 1 >= length())
        return {};
    // .. is not a valid extension (it's usually part of a folder as in "../dir/")
    else if (at(pos + 1) == '.')
        return {};

    return { data() + pos, length() - pos };
}

std::string_view ttString::filename() const
{
    if (empty())
        return {};

    auto pos = find_last_of('/');

#if defined(_WIN32)
    // Windows filenames can contain both forward and back slashes, so check for a backslash as well.
    auto back = find_last_of('\\');
    if (back != npos)
    {
        // If there is no forward slash, or the backslash appears after the forward slash, then use it's position.
        if (pos == npos || back > pos)
            pos = back;
    }
#endif
    if (pos == npos)
    {
        pos = find_last_of(':');
        if (pos == npos)
            return *this;
    }

    return { data() + pos + 1, length() - (pos + 1) };
}

std::string_view ttString::replace_filename(std::string_view newFilename)
{
    if (empty())
    {
        assign(newFilename);
        return *this;
    }

    auto pos = find_last_of('/');

#if defined(_WIN32)
    // Windows filenames can contain both forward and back slashes, so check for a backslash as well.
    auto back = find_last_of('\\');
    if (back != npos)
    {
        // If there is no forward slash, or the backslash appears after the forward slash, then use it's position.
        if (pos == npos || back > pos)
            pos = back;
    }
#endif
    if (pos == npos)
    {
        pos = find_last_of(':');
        if (pos == npos)
        {
            // If we get here, we think the entire current string is a filename.
            assign(newFilename);
            return *this;
        }
    }

    replace(pos + 1, length() - (pos + 1), newFilename);
    return *this;
}

std::string_view ttString::append_filename(std::string_view filename)
{
    if (filename.empty())
        return *this;
    if (empty())
    {
        assign(filename);
        return *this;
    }

    auto last = back();
    if (last != '/' && last != '\\')
        push_back('/');
    append(filename);
    return *this;
}

std::string_view ttString::assignCwd()
{
    assign(std::filesystem::absolute(".").u8string());
    return *this;
}

/// Uses const std::string& instead of std::string_view because std::filesystem::relative
/// will not accept a string_view.
std::string_view ttString::make_relative(const std::string& relative_to)
{
    if (!empty())
    {
        auto current = std::filesystem::u8path(c_str());
        auto relto = std::filesystem::u8path(relative_to);
        assign(std::filesystem::relative(current, relto).u8string());
    }
    return *this;
}

std::string_view ttString::make_absolute()
{
    if (!empty())
    {
        auto current = std::filesystem::u8path(c_str());
        assign(std::filesystem::absolute(current).u8string());
    }
    return *this;
}

bool ttString::fileExists() const
{
    if (empty())
        return false;
    auto file = std::filesystem::directory_entry(std::filesystem::u8path(c_str()));
    return (file.exists() && !file.is_directory());
}

bool ttString::dirExists() const
{
    if (empty())
        return false;
    auto file = std::filesystem::directory_entry(std::filesystem::u8path(c_str()));
    return (file.exists() && file.is_directory());
}

size_t ttString::findoneof(const char* pszSet) const
{
    if (!pszSet || !*pszSet)
        return npos;
    const char* pszFound = std::strpbrk(c_str(), pszSet);
    if (!pszFound)
        return npos;
    return (static_cast<size_t>(pszFound - c_str()));
}

size_t ttString::findspace(size_t start) const
{
    if (start >= length())
        return npos;
    const char* pszFound = std::strpbrk(c_str() + start, " \t\r\n\f");
    if (!pszFound)
        return npos;
    return (static_cast<size_t>(pszFound - c_str()));
}

size_t ttString::findnonspace(size_t start) const
{
    for (; start < length(); ++start)
    {
        if (!std::strchr(" \t\r\n\f", at(start)))
            break;
    }
    return start;
}

size_t ttString::stepover(size_t start) const
{
    auto pos = findspace(start);
    if (pos != npos)
    {
        pos = findnonspace(pos);
    }
    return pos;
}

#include "../utf8/unchecked.h"

std::wstring ttString::to_utf16() const
{
    std::wstring str16;
    utf8::unchecked::utf8to16(begin(), end(), back_inserter(str16));
    return str16;
}

void ttString::from_utf16(std::wstring_view str)
{
    utf8::unchecked::utf16to8(str.begin(), str.end(), back_inserter(*this));
}

////////////////////////// Functions below are tt:: namespace functions /////////////////////////////////////

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

std::string_view tt::findstr(std::string_view main, std::string_view sub, bool CaseSensitive)
{
    if (sub.empty())
        return {};

    if (CaseSensitive)
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

size_t tt::findstr_pos(std::string_view main, std::string_view sub, bool CaseSensitive)
{
    auto view = tt::findstr(main, sub, CaseSensitive);
    if (view.empty())
        return tt::npos;
    else
        return (main.size() - view.size());

}

bool tt::contains(std::string_view main, std::string_view sub, bool CaseSensitive)
{
    return !tt::findstr(main, sub, CaseSensitive).empty();
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

bool tt::issameas(std::string_view str1, std::string_view str2, bool CaseSensitive)
{
    if (CaseSensitive)
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
