/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.cpp
// Purpose:   Function for translating strings into UTF8 char* pointer
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Currently, translation only occurs when use with wxWidgets.

#include "pch.h"

#include <unordered_map>

#if defined(_WX_DEFS_H_)
    #include <wx/intl.h>
#endif

#include "../include/ttTR.h"

std::unordered_map<std::string, std::string> tt_translations;

const char* ttTranslate(const char* psz)
{
    if (psz)
    {
        auto found = tt_translations.find(psz);
        if (found != tt_translations.end())
        {
            return found->second.c_str();
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
                        return translated.first->second.c_str();
                    }
                }
                else
                {
                    // No translation available, make certain we don't try again.
                    auto translated = tt_translations.insert({ psz, psz });
                    if (translated.second)
                    {
                        return translated.first->second.c_str();
                    }
                }
            }
            else
            {
                auto translated = tt_translations.insert({ psz, psz });
                if (translated.second)
                {
                    return translated.first->second.c_str();
                }
            }
#else    // !defined(_WX_DEFS_H_)
            // Currently all that happens is the string is added without translation.
            auto translated = tt_translations.insert({ psz, psz });
            if (translated.second)
            {
                return translated.first->second.c_str();
            }
#endif  // _WX_DEFS_H_
        }
    }

    // We only get here if the passed string ptr is null, or the unordered_map was unable to insert
    // the string.
    return psz;
}

void ttTRclear() noexcept
{
    tt_translations.clear();
}
