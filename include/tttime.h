/////////////////////////////////////////////////////////////////////////////
// Name:      ttCTime
// Purpose:   Class for handling a Windows SYSTEMTIME or FILETIME structure
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <winbase.h>
#include <winnls.h>

#include "ttcstr.h"   // cstr -- Classes for handling zero-terminated char strings.
#include "ttdebug.h"  // ttASSERT macros

// Class for handling a Windows SYSTEMTIME or FILETIME structure
class ttCTime
{
public:
    ttCTime() { GetLocalTime(); }
    ttCTime(FILETIME* pftm, bool bLocalTime = false)
    {
        ttASSERT(pftm);
        ConvertFileTime(pftm, bLocalTime);
    }  // Copies the file time into a SYSTEMTIME structure
    ~ttCTime() {}

    int GetYear() const { return (int) m_tm.wYear; }
    int GetMonth() const { return (int) m_tm.wMonth; }  // month of year (1 = Jan)
    int GetDay() const { return (int) m_tm.wDay; }      // day of month
    int GetHour() const { return (int) m_tm.wHour; }
    int GetMinute() const { return (int) m_tm.wMinute; }
    int GetSecond() const { return (int) m_tm.wSecond; }
    int GetMilliSecond() const { return (int) m_tm.wMilliseconds; }
    int GetDayOfWeek() const { return (int) m_tm.wDayOfWeek; }  // 1=Sun, 2=Mon, ..., 7=Sat

    const char* GetDateFormat(DWORD dwFlags = 0, LCID locale = LOCALE_USER_DEFAULT)
    {
        char szBuf[256];
        ::GetDateFormatA(locale, dwFlags, &m_tm, NULL, szBuf, sizeof(szBuf));
        m_cszDate = szBuf;
        return m_cszDate.c_str();
    }
    const char* GetTimeFormat(DWORD dwFlags = 0, LCID locale = LOCALE_USER_DEFAULT)
    {
        char szBuf[256];
        ::GetTimeFormatA(locale, dwFlags, &m_tm, NULL, szBuf, sizeof(szBuf));
        m_cszTime = szBuf;
        return m_cszTime.c_str();
    }
    const char* GetFullFormat()
    {  // full date/time
        m_cszFull = GetDateFormat(
            DATE_LONGDATE);  // GetDateFormat() modifies m_cszFormatted, so we have to use a temporary
        m_cszFull += ", ";
        m_cszFull += GetTimeFormat();
        return m_cszFull.c_str();
    }
    const char* GetShortFormat()
    {
        m_cszShort = GetDateFormat();  // GetDateFormat() modifies m_cszFormatted, so we have to use a temporary
        m_cszShort += ", ";
        m_cszShort += GetTimeFormat();
        return m_cszShort.c_str();
    }

    void GetLocalTime() { ::GetLocalTime(&m_tm); }
    void GetSystemTime() { ::GetSystemTime(&m_tm); }

    void ConvertFileTime(const FILETIME* pftm, bool bLocalTime = false)
    {
        ttASSERT(pftm);
        if (bLocalTime)
        {
            FILETIME tmLocal;
            FileTimeToLocalFileTime(pftm, &tmLocal);
            FileTimeToSystemTime(&tmLocal, &m_tm);
        }
        else
            FileTimeToSystemTime(pftm, &m_tm);
    }
    void SysTimeToFileTime(FILETIME* pftm)
    {
        ttASSERT(pftm);
        SystemTimeToFileTime(&m_tm, pftm);
    }

    operator SYSTEMTIME*() { return &m_tm; }

private:
    SYSTEMTIME m_tm;

    // We keep a copy of each format in case we're called multiple times in a printf() call

    ttlib::cstr m_cszDate;
    ttlib::cstr m_cszTime;
    ttlib::cstr m_cszFull;
    ttlib::cstr m_cszShort;
};
