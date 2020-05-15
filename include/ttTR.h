/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.h
// Purpose:   Function for translating strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

/// @file
/// The _tt() macro lets you load localized strings from a STRINGTABLE resource when compiled for Windows. If you are
/// using the wxWidgets library and you have loaded <wx/intl.h> prior to this library, then strings passed to the _tt()
/// macro will be translated using the wxTranslations class.
///
/// You can setup your string id header file in a way that will load strings from the STRINGTABLE resource and either
/// use non-localized strings, or strings translated via the xgettext system if you are using the wxWidgets library. For
/// example:
///
///    #ifdef _WIN32
///        #define IDS_CANNOT_OPEN  1024  // "Cannot open "
///    #else
///        constexpr auto IDS_CANNOT_OPEN = "Cannot open ";
///    #endif
///
/// If you need to change languages within the program, you must call **ttlib::clearTranslations()** to clear any
/// previously stored translated strings.

#pragma once

#include <map>
#include <optional>
#include <string>

#include "ttcstr.h"

namespace ttTR
{
    // We already have ttlib::emptystring, but it's a const and returning it causes a compiler error, so we create a
    // non-const here to return. Note that this does provide the option of setting the string to a specific value to
    // indicate an error, since it only gets returned if an empty string is passed, or in the case of translate(WORD
    // id), it is returned if the resource id doesn't exists.

    extern ttlib::cstr trEmpty;

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

extern ttTR::cmap tt_translations;

namespace ttlib
{
#if defined(_WIN32)

    /// If you use a numerical id in the _tt() macro, this will load the string from a
    /// STRINGTABLE resource and return a pointer to the string.
    const ttlib::cstr translate(WORD id);

#endif

// This is only useful if you have BOTH translated STRINGTABLE resources and xgettext translated .mo files. Otherwise, either include
// ttTR.h and use _tt() macros or include wx/intl.h and use _() macros.
#if defined(_WX_INTL_H_)
    inline const ttlib::cstr& translate(const std::string& str)
    {
        if (auto [found, value] = tt_translations.getValue(str); found)
            return value;
        if (auto strTranslation = wxTranslations::Get()->GetTranslatedString(str); strTranslation->size())
        {
            ttlib::cstr tmp;
            tmp.assign(strTranslation->utf8_str().data());
            if (auto [pair, success] = tt_translations.insert({ str, std::move(tmp) }); success)
            {
                return pair->second;
            }
        }

        // This should never happen, but an empty string is returned just in case...
        return ttTR::trEmpty;
    }
#else

    /// Returns either a translated string, or the original string if no translation is available.
    const ttlib::cstr& translate(const std::string& str);

#endif

    /// Clears all previously translated strings. Required after changing locale.
    void clearTranslations() noexcept;
}  // namespace ttlib

/// Macro that can be parsed by xgettext to add a string to translate.
#define _tt(txt) ttlib::translate((txt))

/// This macro can be placed around static text that you want xgettext.exe to extract for
/// translation using the "xgettext.exe -kttTR" keyword option.
#define ttTR(txt) txt

/// If count == 1, returns a translation of single, otherwise it returns a translation of plural.
inline const ttlib::cstr& _ttp(const std::string& single, const std::string& plural, size_t count)
{
    return (count == 1 ? _tt(single) : _tt(plural));
}
