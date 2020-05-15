/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.cpp
// Purpose:   Function for translating strings into UTF8 char* pointer
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ttTR.h"

ttTR::cmap tt_translations;

namespace ttTR
{
    ttlib::cstr trEmpty;
}  // namespace ttTR

// This conditional is in case the module is being compiled directly instead of into a library (wxUiEditor does this)

#if !defined(_WX_INTL_H_)

// The _tt() macro can be used to load a string from the STRINGTABLE resource when compiled for Windows, or it can
// specify a string when compiled for other platforms. If <wx/intl.h> has been loaded, then this function will never be
// called. It exists for when translation is available for Windows builds, but not for other platforms and wxWidgets is
// not available.

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
        // Currently all that happens is the string is added without translation. Ideally, this should hook up directly
        // to the xgettext translation system without requiring wxWidgets.
        if (auto [pair, success] = tt_translations.insert({ str, str }); success)
        {
            return pair->second;
        }
    }

    // We only get here if the passed string ptr is null, or the unordered_map was unable to insert the string.
    return ttTR::trEmpty;
}

#endif

void ttlib::clearTranslations() noexcept
{
    tt_translations.clear();
}
