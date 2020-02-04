/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.cpp
// Purpose:   Function for translating strings into UTF8 char* pointer
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <unordered_map>
#include <wx/intl.h>

#include "../include/ttTR.h"

static std::unordered_map<std::string, std::string> tt_translations;

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
        }
    }

    // We only get here if the passed string ptr is null, or the unordered_map was unable to insert
    // the string.
    return psz;
}

void ttTRclear()
{
    tt_translations.clear();
}
