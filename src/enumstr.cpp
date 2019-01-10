/////////////////////////////////////////////////////////////////////////////
// Name:		ttEnumStr
// Purpose:		Enumerate through substrings in a string
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/enumstr.h"
#include "../include/ttstr.h"	// various functions dealing with strings

ttEnumStr::ttEnumStr()
{
	m_pszCur = nullptr;
	m_pszEnd = nullptr;
}

ttEnumStr::ttEnumStr(const char* psz, char chSeparator)
{
	if (psz)
		m_csz = psz;
	m_pszEnd = nullptr;
	m_chSeparator = chSeparator;
	ResetEnum(chSeparator);
}

void ttEnumStr::ResetEnum(char chSeparator)
{
	if (m_csz.IsEmpty()) {
		m_pszCur = nullptr;
		m_pszEnd = nullptr;
		return;
	}

	if (m_pszEnd)
		*m_pszEnd = m_chSeparator;	// restore any previous separator

	m_chSeparator = chSeparator;
	m_pszCur = nullptr;
	m_pszEnd = nullptr;
}

void ttEnumStr::SetNewStr(const char* psz, char chSeparator)
{
	if (psz)
		m_csz = psz;
	else
		m_csz.Delete();
	m_pszEnd = nullptr;
	m_chSeparator = chSeparator;
	ResetEnum(chSeparator);
}

bool ttEnumStr::Enum(const char** ppszCurrent)
{
	ttASSERT_MSG(!m_csz.IsNull(), "Calling Enum() without a valid master string (ttEnumStr(nullptr) or SetNewStr(nullptr))!");
	if (m_csz.IsNull()) {
		if (ppszCurrent)
			*ppszCurrent = nullptr;
		return false;
	}

	if (m_pszCur == nullptr) {	// means we haven't been called before, or ResetEnum() was called to reset
		m_pszCur = (const char*) m_csz;
		m_pszEnd = tt::strchr(m_pszCur, m_chSeparator);
		if (m_pszEnd)
			*m_pszEnd = 0;
		if (ppszCurrent)
			*ppszCurrent = m_pszCur;
		return true;
	}
	else {
		if (!m_pszEnd) {
			if (ppszCurrent)
				*ppszCurrent = nullptr;
			return false;
		}
		else {
			*m_pszEnd = m_chSeparator;
			do {	// handle doubled characters, or characters with only space between them (";;" or "; ;")
				m_pszCur = tt::nextnonspace(m_pszEnd + 1);
				m_pszEnd = tt::strchr(m_pszCur, m_chSeparator);
			} while(*m_pszCur == m_chSeparator && m_pszEnd);

			if (*m_pszCur == m_chSeparator) {	// means we got to the end with no more separators
				if (ppszCurrent)
					*ppszCurrent = nullptr;
				return false;
			}
			if (m_pszEnd)
				*m_pszEnd = 0;
			if (ppszCurrent)
				*ppszCurrent = m_pszCur;
			return true;
		}
	}
}
