/////////////////////////////////////////////////////////////////////////////
// Name:      ttCEnumStr
// Purpose:   Enumerate through substrings in a string
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/*
    This class is used to enumerate through substrings that are separated by a single character (typically a
    semicolon). The following example uses this to parse through each of the directories in an INCLUDE environment
    variable:

        ttCEnumStr enumstr("c:/sdk/include;c:/github/wxwidgets/include/wx", ';');
        while (enumstr.Enum())
            puts(enumstr)
*/

#pragma once

#include "ttstr.h"  // ttCStr

// Enumerate through substrings in a string
class ttCEnumStr
{
public:
    ttCEnumStr(const char* psz, char chSeparator = ';');  // This will make a copy of psz
    ttCEnumStr();  // If using this constructor, you must call SetNewStr() before calling Enum()

    // Class functions

    // If no more substrings, *ppszCurrent (if non-null) will be set to nullptr
    bool Enum(const char** ppszCurrent = nullptr);
    // Call this to reset the enumeration to the beginning of the master string
    void ResetEnum(char chSeparator = ';');
    void SetNewStr(const char* psz, char chSeparator = ';');

    // Value is undefined if Enum() returned false
    char* GetCurrent() const { return m_pszCur; }

    operator char*() const { return m_pszCur; }

    bool operator==(const char* psz) { return ttIsSameStr(m_pszCur, psz); }
    bool operator==(char* psz) { return ttIsSameStr(m_pszCur, psz); }

private:
    // Class members

    char*  m_pszCur;
    ttCStr m_csz;
    char*  m_pszEnd;
    char   m_chSeparator;
};
