/////////////////////////////////////////////////////////////////////////////
// Name:		CKeyTime
// Purpose:		Class for handling Windows time structure
// Author:		Ralph Walden (randalphwa)
// Copyright:   Copyright (c) 2002-2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_KTIME_H__
#define __TTLIB_KTIME_H__

#ifndef _WINDOWS_
	#error CKeyTime can only be used when building for Windows. Use wxWidgets file-io for cross-platform functionality
#endif

#include "cstr.h"	// CStr

// This is a header-only class.

class CKeyTime
{
public:
	CKeyTime(FILETIME* pftm) { ASSERT(pftm); ConvertFileTime(pftm); }
	CKeyTime() { GetLocalTime(); }
	~CKeyTime() {  }

	int GetYear() const { return (int) m_tm.wYear; }
	int GetMonth() const { return (int) m_tm.wMonth; }      // month of year (1 = Jan)
	int GetDay() const { return (int) m_tm.wDay; }         // day of month
	int GetHour() const { return (int) m_tm.wHour; }
	int GetMinute() const { return (int) m_tm.wMinute; }
	int GetSecond() const { return (int) m_tm.wSecond; }
	int GetMilliSecond() const { return (int) m_tm.wMilliseconds; }
	int GetDayOfWeek() const { return (int) m_tm.wDayOfWeek; }  // 1=Sun, 2=Mon, ..., 7=Sat

	const char* GetDateFormat(DWORD dwFlags = 0, LCID locale = LOCALE_USER_DEFAULT) {
				char szBuf[256];
				::GetDateFormat(locale, dwFlags, &m_tm, NULL, szBuf, sizeof(szBuf));
				m_cszFormatted = szBuf;
				return m_cszFormatted;
	}
	const char* GetTimeFormat(DWORD dwFlags = 0, LCID locale = LOCALE_USER_DEFAULT) {
				char szBuf[256];
				::GetTimeFormat(locale, dwFlags, &m_tm, NULL, szBuf, sizeof(szBuf));
				m_cszFormatted = szBuf;
				return m_cszFormatted;
	}
	const char* GetFullFormat() {	// full date/time
			char szBuf[256];
			tt::strcpy(szBuf, sizeof(szBuf), GetDateFormat(NULL, DATE_LONGDATE));
			tt::strcat(szBuf, ", ");
			tt::strcat(szBuf, sizeof(szBuf) + tt::strlen(szBuf), GetTimeFormat());
			m_cszFormatted = szBuf;
			return m_cszFormatted;
	}
	const char* GetShortFormat() {	// short date/time
			char szBuf[256];
			tt::strcpy(szBuf, sizeof(szBuf), GetDateFormat());
			tt::strcat(szBuf, ", ");
			tt::strcat(szBuf, sizeof(szBuf) + tt::strlen(szBuf), GetTimeFormat());
			m_cszFormatted = szBuf;
			return m_cszFormatted;
	}

	void GetLocalTime() { ::GetLocalTime(&m_tm); }
	void GetSystemTime() { ::GetSystemTime(&m_tm); }

	void ConvertFileTime(const FILETIME* pftm) { ASSERT(pftm); FileTimeToSystemTime(pftm, &m_tm); }
	void SysTimeToFileTime(FILETIME* pftm) { ASSERT(pftm); SystemTimeToFileTime(&m_tm, pftm); }

	operator SYSTEMTIME*() { return &m_tm; }

protected:
	SYSTEMTIME m_tm;
	CStr m_cszFormatted;
};

#endif	// __TTLIB_KTIME_H__
