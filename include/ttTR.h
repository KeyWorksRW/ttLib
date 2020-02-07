/////////////////////////////////////////////////////////////////////////////
// Name:      ttTR.h
// Purpose:   Function for translating strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// @file
/// Currently, actual translation requires wxWidgets. You can use these macros without wxWidgets, but the
/// strings will not be translated.
///
/// **_tt()** is similar to the _() macro in **wxWidgets** in that it is used to translate a string.
/// The most significant difference is that _tt() returns a **const char\*** pointer instead
/// of a **wxString&**. On Windows, that means you can pass **_tt()** to **std:string** and **std::string_view**
/// without having to first convert it from a **UTF16** string to a **UTF8** string `wxString.utf8().data()`.
///
/// **_tt()** will only use the underlying **wxWidgets** library to translate a string once. It then stores
/// the translation in an **std::unordered_map** and the next time a translation for that string is requested, it
/// will return a pointer to the already-translated string.
///
/// If you need to change languages within the program, you must call **ttTRclear()** to clear any previously
/// stored translated strings.
///
/// Note: If you use these functions/macro, be sure to add `-k_tt -k_ttp:1,2 -kttTR` to your **xgettext**
/// command line.

#pragma once

/// Macro that can be parsed by xgettext to add a string to translate.
#define _tt(txt) ttTranslate(txt)

/// Looks up the translation of the string, returns a pointer to the translation, or a copy of the original
/// string if no translation is available.
const char* ttTranslate(const char* psz);

/// If n == 1, returns a translation of psz, otherwise it returns a translation of pszPlural.
inline const char* _ttp(const char* psz, const char* pszPlural, unsigned int n)
{
    return (n == 1 ? _tt(psz) : _tt(pszPlural));
}

/// Clears all previously translated strings. Required after changing locale.
void ttTRclear();

/// Shorter version of wxTRANSLATE() macro which does the same thing.
///
/// This macro can be placed around static text that you want xgettext.exe to extract for
/// translation using the "xgettext.exe -kttTTRS" keyword option.
#define ttTR(txt) txt
