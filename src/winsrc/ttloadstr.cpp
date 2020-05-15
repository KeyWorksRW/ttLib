/////////////////////////////////////////////////////////////////////////////
// Name:      ttloadstr.cpp
// Purpose:   Load a language-specific version of a string resource
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <map>
#include <string_view>

#include "ttTR.h"     // cmap -- Function for translating strings
#include "ttcstr.h"   // cstr -- Classes for handling zero-terminated char strings.
#include "ttdebug.h"  // ttASSERT macros

namespace ttlib
{
    LANGINFO lang_info;

    std::map<DWORD, std::string> mapStrings;

}  // namespace ttlib

using namespace ttlib;

void ttlib::SetLangInfo(HMODULE hinstResource, USHORT PrimaryLanguage, USHORT SubLanguage)
{
    lang_info.hinstResource = hinstResource;
    lang_info.PrimaryLanguage = PrimaryLanguage;
    lang_info.SubLanguage = SubLanguage;
}

ttlib::cstr ttlib::LoadStringEx(WORD id)
{
    ttlib::cstr str8;
    LoadStringEx(str8, id);
    return str8;
}

bool ttlib::LoadStringEx(std::string& Result, WORD id)
{
    auto key = MAKELONG(id, MAKELANGID(lang_info.PrimaryLanguage, lang_info.SubLanguage));

    auto found = mapStrings.find(key);
    if (found != mapStrings.end())
    {
        Result = found->second;
        return true;
    }

    // Setup Result string in case the string id cannot be located.
#if !defined(NDEBUG)  // Starts debug section.
    {
        ttlib::cstr tmp;
        Result = std::move(tmp.Format("String resource not found: %u", id));
    }
#else
    Result.clear();  // Clear the string in case we return false;
#endif

    // Strings are in blocks of 16 items per block, so find the block containing the string we want

    UINT blockNumber = (id >> 4) + 1;
    auto hres = FindResourceEx(lang_info.hinstResource, RT_STRING, MAKEINTRESOURCE(blockNumber),
                               MAKELANGID(lang_info.PrimaryLanguage, lang_info.SubLanguage));
    if (!hres)
        return false;

    auto sizeRes = SizeofResource(lang_info.hinstResource, hres);
    auto memory = LoadResource(lang_info.hinstResource, hres);

    if (!sizeRes || !memory)
        return false;

    auto begin = static_cast<wchar_t*>(LockResource(memory));
    if (!begin)
        return false;
    auto end = begin + sizeRes;

    // Strings in the block are Pascal-style -- first a WORD specifying the length, followed by the actual string.

    WORD idx = 0;
    WORD indexNumber = id % 16;
    while (begin < end)
    {
        auto length = static_cast<WORD>(begin[0]);
        if (idx == indexNumber)
        {
            // Note that the string can be zero-length, but we still return true since we did find the string.

            std::wstring_view str16(begin + 1, length);
            Result.clear();
            utf16to8(str16, Result);
            mapStrings.insert({ key, Result });
            return true;
        }
        else
        {
            // Step over the lead string width and the string itself
            begin += (length + 1);
            ++idx;
        }
    }
    return false;
}

std::map<WORD, ttlib::cstr> tt_stringtable;

using namespace ttTR;

const ttlib::cstr ttlib::translate(WORD id)
{
    auto found = tt_stringtable.find(id);
    if (found != tt_stringtable.end())
    {
        return found->second;
    }

    std::string str;
    if (ttlib::LoadStringEx(str, id))
    {
        auto entry = tt_stringtable.insert({ id, str });
        return entry.first->second;
    }

#if !defined(NDEBUG)  // Starts debug section.
    trEmpty.Format("String Resource id %u not found", id);
    assertm(false, trEmpty.c_str());
    trEmpty.clear();
#endif

    return trEmpty;
}
