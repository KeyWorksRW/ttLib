/////////////////////////////////////////////////////////////////////////////
// Name:      misc.cpp
// Purpose:   miscellaneous functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <cassert>

#include "../include/ttlibwin.h"
#include "../include/ttstr.h"  // ttCStr

#if defined(_MSC_VER)
    // We don't warn about deprecated functions that are still her for backwards compatability
    #pragma warning(disable : 4996)  // deprecated
#endif

size_t ttHashFromSz(const char* psz)
{
    assert(psz);

    if (!psz || !*psz)
        return 0;

    // djb2 hash algorithm

    size_t hash = 5381;

    while (*psz)
        hash = ((hash << 5) + hash) ^ (size_t) *psz++;

    return hash;
}

size_t ttHashFromSz(const wchar_t* psz)
{
    assert(psz);

    if (!psz || !*psz)
        return 5381;

    size_t hash = 5381;

    while (*psz)
        hash = ((hash << 5) + hash) + (size_t) *psz++;

    return hash;
}

// Unlike a "regular" hash, this version treats forward and backslashes identically, as well as upper and
// lower-case letters

size_t ttHashFromURL(const char* pszURL)
{
    ttCStr csz(pszURL);
    ttBackslashToForwardslash(csz);
    csz.MakeLower();
    return ttHashFromSz(csz);
}

size_t ttHashFromURL(const wchar_t* pszURL)
{
    ttCStr csz(pszURL);
    ttBackslashToForwardslash(csz);
    csz.MakeLower();
    return ttHashFromSz(csz);
}

///////////////// The following code is obsolete -- it's here until all caller's get changed

ttCEnumStr::ttCEnumStr()
{
    m_pszCur = nullptr;
    m_pszEnd = nullptr;
}

ttCEnumStr::ttCEnumStr(const char* psz, char chSeparator)
{
    if (psz)
        m_csz = psz;
    m_pszEnd = nullptr;
    m_chSeparator = chSeparator;
    ResetEnum(chSeparator);
}

void ttCEnumStr::ResetEnum(char chSeparator)
{
    if (m_csz.empty())
    {
        m_pszCur = nullptr;
        m_pszEnd = nullptr;
        return;
    }

    if (m_pszEnd)
        *m_pszEnd = m_chSeparator;  // restore any previous separator

    m_chSeparator = chSeparator;
    m_pszCur = nullptr;
    m_pszEnd = nullptr;
}

void ttCEnumStr::SetNewStr(const char* psz, char chSeparator)
{
    if (psz)
        m_csz = psz;
    else
        m_csz.clear();
    m_pszEnd = nullptr;
    m_chSeparator = chSeparator;
    ResetEnum(chSeparator);
}

bool ttCEnumStr::Enum(const char** ppszCurrent)
{
    assert(!m_csz.empty());
    if (m_csz.empty())
    {
        if (ppszCurrent)
            *ppszCurrent = nullptr;
        return false;
    }

    if (m_pszCur == nullptr)  // means we haven't been called before, or ResetEnum() was called to reset
    {
        m_pszCur = const_cast<char*>(m_csz.c_str());
        m_pszEnd = ttStrChr(m_pszCur, m_chSeparator);
        if (m_pszEnd)
            *m_pszEnd = 0;
        if (ppszCurrent)
            *ppszCurrent = m_pszCur;
        return true;
    }
    else
    {
        if (!m_pszEnd)
        {
            if (ppszCurrent)
                *ppszCurrent = nullptr;
            return false;
        }
        else
        {
            *m_pszEnd = m_chSeparator;
            do
            {  // handle doubled characters, or characters with only space between them (";;" or "; ;")
                m_pszCur = ttFindNonSpace(m_pszEnd + 1);
                m_pszEnd = ttStrChr(m_pszCur, m_chSeparator);
            } while (*m_pszCur == m_chSeparator && m_pszEnd);

            if (*m_pszCur == m_chSeparator)  // means we got to the end with no more separators
            {
                if (ppszCurrent)
                    *ppszCurrent = nullptr;
                return false;
            }
            if (m_pszEnd)
                *m_pszEnd = 0;

            if (m_pszCur && !m_pszCur[0])  // Don't return empty string -- this can happen when a original string
                                           // ends without a separator
                return Enum(ppszCurrent);

            if (ppszCurrent)
                *ppszCurrent = m_pszCur;
            return true;
        }
    }
}
