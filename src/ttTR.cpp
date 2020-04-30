/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.cpp
// Purpose:   Function for translating strings into UTF8 char* pointer
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <map>
#include <optional>

#if defined(_WX_DEFS_H_)
    #include <wx/intl.h>
#endif

#include "ttTR.h"
#include "ttcstr.h"

namespace ttTR
{
    ttlib::cstr trEmpty;

    class cmap : public std::map<const std::string, ttlib::cstr>
    {
    public:
        std::pair<bool, const ttlib::cstr&> getValue(const std::string& key) const
        {
            if (auto found = find(key); found != end())
                return { true, found->second };
            else
                return { false, trEmpty };
        }
    };
}  // namespace ttTR

ttTR::cmap tt_translations;

// We already have ttlib::emptystring, but it's a const and returning it causes a compiler error, so we create a
// non-const here to return. Note that this does provide the option of setting the string to a specific value to
// indicate an error, since it only gets returned if an empty string is passed, or in the case of translate(WORD id), it
// is returned if the resource id doesn't exists.

const ttlib::cstr& ttlib::translate(const std::string& str)
{
    if (str.empty())
    {
        return ttTR::trEmpty;
    }

    if (auto [found, value] = tt_translations.getValue(str); found)
    {
        return value;
    }

    else
    {
#if defined(_WX_DEFS_H_)
        if (auto trans = wxTranslations::Get(); trans)
        {
            if (auto strTranslation = trans->GetTranslatedString(str); !strTranslation.empty())
            {
                if (auto [pair, success] = tt_translations.insert({ str, strTranslation->utf8_str().data() }, success))
                {
                    return pair->second;
                }
            }
            else
            {
                // No translation available, make certain we don't try again.
                if (auto [pair, success] = tt_translations.insert({ str, str }); success)
                {
                    return pair->second;
                }
            }
        }
        else
        {
            // No translation available, make certain we don't try again.
            if (auto [pair, success] = tt_translations.insert({ str, str }); success)
            {
                return pair->second;
            }
        }
#else   // !defined(_WX_DEFS_H_)
        // Currently all that happens is the string is added without translation.
        if (auto [pair, success] = tt_translations.insert({ str, str }); success)
        {
            return pair->second;
        }
#endif  // _WX_DEFS_H_
    }

    // We only get here if the passed string ptr is null, or the unordered_map was unable to insert
    // the string.
    return ttTR::trEmpty;
}

void ttlib::clearTranslations() noexcept
{
    tt_translations.clear();
}
