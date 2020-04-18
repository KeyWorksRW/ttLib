/////////////////////////////////////////////////////////////////////////////
// Name:      time
// Purpose:   Class for handling a Windows SYSTEMTIME or FILETIME structure
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2020 KeyWorks Software (Ralph Walden)
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

namespace ttlib
{
    // Class for handling a Windows SYSTEMTIME or FILETIME structure
    class time
    {
    public:
        time() { GetLocalTime(); }
        time(FILETIME* pftm, bool bLocalTime = false)
        {
            ttASSERT(pftm);
            ConvertFileTime(pftm, bLocalTime);
        }  // Copies the file time into a SYSTEMTIME structure
        ~time() {}

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
            auto len = ::GetDateFormatW(locale, dwFlags, &m_tm, NULL, nullptr, 0);
            if (len)
            {
                ++len;  // Add room for trailing zero

                auto str16 = std::make_unique<wchar_t[]>(len).get();
                ::GetDateFormatW(locale, dwFlags, &m_tm, NULL, str16, len);
                m_date.clear();
                ttlib::utf16to8(str16, m_date);
                return m_date.c_str();
            }
            else
            {
                return ttlib::emptystring.c_str();
            }
        }

        const char* GetTimeFormat(DWORD dwFlags = 0, LCID locale = LOCALE_USER_DEFAULT)
        {
            auto len = ::GetTimeFormatW(locale, dwFlags, &m_tm, NULL, nullptr, 0);
            if (len)
            {
                ++len;  // Add room for trailing zero

                auto str16 = std::make_unique<wchar_t[]>(len).get();
                ::GetTimeFormatW(locale, dwFlags, &m_tm, NULL, str16, len);
                m_time.clear();
                ttlib::utf16to8(str16, m_time);
                return m_time.c_str();
            }
            else
            {
                return ttlib::emptystring.c_str();
            }
        }

        const char* GetFullFormat()
        {
            m_full = GetDateFormat(DATE_LONGDATE);
            m_full += ", ";
            m_full += GetTimeFormat();
            return m_full.c_str();
        }

        const char* GetShortFormat()
        {
            m_short = GetDateFormat();  // GetDateFormat() modifies m_cszFormatted, so we have to use a temporary
            m_short += ", ";
            m_short += GetTimeFormat();
            return m_short.c_str();
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

        ttlib::cstr m_date;
        ttlib::cstr m_time;
        ttlib::cstr m_full;
        ttlib::cstr m_short;
    };
}  // namespace ttlib
