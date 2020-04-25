/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.cpp
// Purpose:   Function for translating strings into UTF8 char* pointer
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Note: currently, translation only occurs when compiled with wxWidgets.

#include "pch.h"

#include <map>

#if defined(_WX_DEFS_H_)
    #include <wx/intl.h>
#endif

#include "ttTR.h"
#include "ttcstr.h"

std::map<const std::string, ttlib::cstr> tt_translations;

// We already have ttlib::emptystring, but it's a const and returning it causes a compiler error, so we create a
// non-const here to return. Note that this does provide the option of setting the string to a specific value to
// indicate an error, since it only gets returned if an empty string is passed, or in the case of translate(WORD id), it
// is returned if the resource id doesn't exists.

ttlib::cstr _tt__emptystr;

const ttlib::cstr& ttlib::translate(const std::string& str)
{
    if (str.empty())
    {
        return _tt__emptystr;
    }

    auto found = tt_translations.find(str);
    if (found != tt_translations.end())
    {
        return found->second;
    }
    else
    {
#if defined(_WX_DEFS_H_)
        auto trans = wxTranslations::Get();
        if (trans)
        {
            auto strTranslation = trans->GetTranslatedString(psz);
            if (strTranslation)
            {
                auto translated = tt_translations.insert({ psz, strTranslation->utf8_str().data() });
                if (translated.second)
                {
                    return translated.first->second;
                }
            }
            else
            {
                // No translation available, make certain we don't try again.
                auto translated = tt_translations.insert({ psz, psz });
                if (translated.second)
                {
                    return translated.first->second;
                }
            }
        }
        else
        {
            auto translated = tt_translations.insert({ psz, psz });
            if (translated.second)
            {
                return translated.first->second;
            }
        }
#else   // !defined(_WX_DEFS_H_)
        // Currently all that happens is the string is added without translation.
        auto translated = tt_translations.insert({ str, str });
        if (translated.second)
        {
            return translated.first->second;
        }
#endif  // _WX_DEFS_H_
    }

    // We only get here if the passed string ptr is null, or the unordered_map was unable to insert
    // the string.
    return _tt__emptystr;
}

void clearTranslations() noexcept
{
    tt_translations.clear();
}
