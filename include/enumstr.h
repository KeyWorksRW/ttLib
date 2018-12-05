/////////////////////////////////////////////////////////////////////////////
// Name:		CEnumStr
// Purpose:		Enumerate through substrings in a string
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/*
	This class is used to enumerate through substrings that are separated by a single character (typically a semicolon).
	The following example uses this to parse through each of the directories in an INCLUDE environment variable:

		char* pszInclude = "c:/sdk/include;c:/github/wxwidgets/include/wx";
		CEnumStr enumstr(pszInclude, ';');
		const char* pszSubDir;
		while (enumstr.Enum(&pszSubDir)) {
			// after the first call, pszSubDir will point to "c:/sdk/include", after the second call it will point to
			// c:/github/wxwidgets/include/wx
*/

#pragma once

#ifndef __TTLIB_CENUMSTR_H__
#define __TTLIB_CENUMSTR_H__

#include "../include/cstr.h"	// CStr

class CEnumStr
{
public:
	CEnumStr(const char* psz, char chSeparator = ';');	// This will make a copy of psz
	CEnumStr();	// If using this constructor, you must call SetNewStr() before calling Enum()

	// Class functions

	bool Enum(const char** ppszCurrent = nullptr);	// if no more substrings, *ppszCurrent (if non-null) will be set to nullptr
	void ResetEnum(char chSeparator = ';');			// Call this to reset the enumeration to the beginning of the master string
	void SetNewStr(const char* psz, char chSeparator = ';');

	const char* GetCurrent() const { return (const char*) m_pszCur; }	// value is undefined if Enum() returned false
	operator const char*() const { return (const char*) m_pszCur; }

protected:
	// Class members

	CStr		m_csz;
	const char* m_pszCur;
	char*		m_pszEnd;
	char		m_chSeparator;
};

#endif	// __TTLIB_CENUMSTR_H__
