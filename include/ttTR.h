/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.h
// Purpose:   Function for translating strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

/// @file
/// Currently, actual translation requires wxWidgets. You can use these macros without wxWidgets, but the
/// strings will not be translated.
///
/// **_tt()** is similar to the _() macro in **wxWidgets** in that it is used to translate a string.
/// The most significant difference is that _tt() returns **const std::string&** instead of **wxString&**. On
/// Windows, that means you can pass **_tt()** to **std:string** and **std::string_view** without having to first
/// convert it from a **UTF16** string to a **UTF8** string `wxString.utf8().data()`.
///
/// **_tt()** will only use the underlying **wxWidgets** library to translate a string once. It then stores
/// the translation in an **std::map** and the next time a translation for that string is requested, it
/// will return the already-translated string.
///
/// If you need to change languages within the program, you must call **ttlib::clearTranslations()** to clear any
/// previously stored translated strings.
///
/// Note: If you use these functions/macro, be sure to add `-k_tt -k_ttp:1,2 -kttTR` to your **xgettext**
/// command line.

#pragma once

#include <string>

#include "ttcstr.h"

namespace ttlib
{
    class cstr;
    /// Returns either a translated string, or the original string if no translation is available.
    const ttlib::cstr& translate(const std::string& str);

    /// Clears all previously translated strings. Required after changing locale.
    void clearTranslations() noexcept;
}  // namespace ttlib

/// Macro that can be parsed by xgettext to add a string to translate.
#define _tt(txt) ttlib::translate(txt)

/// This macro can be placed around static text that you want xgettext.exe to extract for
/// translation using the "xgettext.exe -kttTR" keyword option.
#define ttTR(txt) txt

/// If count == 1, returns a translation of single, otherwise it returns a translation of plural.
inline const ttlib::cstr& _ttp(const std::string& single, const std::string& plural, size_t count)
{
    return (count == 1 ? _tt(single) : _tt(plural));
}
