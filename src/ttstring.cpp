/////////////////////////////////////////////////////////////////////////////
// Name:		ttCStr
// Purpose:		SBCS string class
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttdebug.h" 	// ttASSERT macros
#include "../include/ttstring.h"	// ttCStr

using namespace ttch;	// used for the CH_ constants

namespace ttpriv {
	void AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst);
}

#ifndef _MAX_U64TOSTR_BASE10_COUNT
	#define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

// Note that the limit here of 64k is smaller then the kstr functions that use 16m (_KSTRMAX)

#define MAX_STRING (64 * 1024)	// Use this to limit the length of a single string as a security precaution
#define	DEST_SIZE (tt::size(m_psz) - sizeof(char))

void ttCStr::AppendFileName(const char* pszFile)
{
	ttASSERT_NONEMPTY(pszFile);

	if (!pszFile || !*pszFile)
		return;

	if (!m_psz)	{	// no folder or drive to append to, so leave as is without adding slash
		m_psz = tt::strdup(pszFile);		// REVIEW: [ralphw - 06-03-2018] We could prefix this with ".\"
		return;
	}

	if (*m_psz)
		AddTrailingSlash();
	*this += pszFile;
}

void ttCStr::ChangeExtension(const char* pszExtension)
{
	ttASSERT_NONEMPTY(pszExtension);

	if (!pszExtension || !*pszExtension)
		return;

	if (!m_psz)
		m_psz = tt::strdup("");

	char* psz = tt::strchrR(m_psz, '.');
	if (psz && !(psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/'))	// ignore .file, ./file, and ../file
		*psz = 0;	// remove the extension if none of the above is true

	if (*pszExtension != '.')
		*this += ".";
	*this += pszExtension;
}

char* ttCStr::FindExt() const
{
	char* psz = tt::strchrR(m_psz, '.');
	if (psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/')	// ignore .file, ./file, and ../file
		return nullptr;
	return psz;
}

void ttCStr::RemoveExtension()
{
	 if (m_psz) {
		char* psz = tt::strchrR(m_psz, '.');
		if (psz) {
			if (psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/')	// ignore .file, ./file, and ../file
				return;
			*psz = 0;
		}
	}
}

void ttCStr::AddTrailingSlash()
{
	if (!m_psz) {
		m_psz = tt::strdup("/");
		return;
	}
	const char* pszLastSlash = FindLastSlash();
	if (!pszLastSlash || pszLastSlash[1])	// only add if there was no slash or there was something after the slash
		*this += "/";
}

char* ttCStr::FindLastSlash()
{
	ttASSERT_MSG(m_psz, "NULL pointer!");

	if (!m_psz || !*m_psz)
		return nullptr;

	char* pszLastBackSlash = tt::strchrR(m_psz, '\\');
	char* pszLastFwdSlash	 = tt::strchrR(m_psz, '/');
	if (!pszLastBackSlash)
		return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
	else if (!pszLastFwdSlash)
		return pszLastBackSlash ? pszLastBackSlash : nullptr;
	else
		return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash : pszLastBackSlash;		// Impossible for them to be equal
}

char* ttCStr::getCWD()
{
#ifdef _WINDOWS_
	resize(MAX_PATH);
	DWORD cb = GetCurrentDirectoryA(MAX_PATH, m_psz);
	m_psz[cb] = 0;	// in case GetCurrentDirectory() failed
#else
	resize(4096);
	char* psz = getcwd(m_psz, 4096);
	if (!psz)
		m_psz[0] = 0;	// in case getcwd() failed
#endif
	return m_psz;		// we leave the full buffer allocated in case you want to add a filename to the end
}

#ifdef _WINDOWS_

void ttCStr::GetFullPathName()
{
	ttASSERT(m_psz);
	char szPath[MAX_PATH];
	::GetFullPathNameA(m_psz, sizeof(szPath), szPath, NULL);
	tt::strdup(szPath, &m_psz);
}

char* ttCStr::GetListBoxText(HWND hwnd, size_t sel)
{
	if (m_psz)
		tt::free(m_psz);
	if (sel == (size_t) LB_ERR)
		m_psz = tt::strdup("");
	else {
		size_t cb = ::SendMessage(hwnd, LB_GETTEXTLEN, sel, 0);
		ttASSERT(cb != (size_t) LB_ERR);
		if (cb != (size_t) LB_ERR) {
			m_psz = (char*) tt::malloc(cb + 1);
			::SendMessageA(hwnd, LB_GETTEXT, sel, (LPARAM) m_psz);
		}
		else {
			m_psz = tt::strdup("");
		}
	}
	return m_psz;
}

/*
	tt::hinstResources is typically set by InitCaller() and determines where to load resources from. If you need to load the resources
	from a DLL, then first call:

		tt::hinstResources = LoadLibrary("dll name");
*/

char* ttCStr::GetResString(size_t idString)
{
	char szStringBuf[1024];

	if (LoadStringA(tt::hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0) {
		ttCStr strMsg;
		strMsg.printf("Invalid string id: %zu", idString);
		ttFAIL(strMsg);
		if (m_psz)
			tt::free(m_psz);
		m_psz = tt::strdup("");
	}
	else {
		tt::strdup(szStringBuf, &m_psz);
	}
	return m_psz;
}

bool ttCStr::GetWindowText(HWND hwnd)
{
	if (m_psz) {
		 tt::free(m_psz);
		 m_psz = nullptr;
	}

	ttASSERT_MSG(hwnd && IsWindow(hwnd), "Invalid window handle");
	if (!hwnd || !IsWindow(hwnd)) {
		m_psz = tt::strdup("");
		return false;
	}

	int cb = GetWindowTextLengthA(hwnd);
	ttASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	if (cb == 0 || cb > MAX_STRING) {
		m_psz = tt::strdup("");
		return false;
	}

	char* psz = (char*) tt::malloc(cb + sizeof(char));
	cb = ::GetWindowTextA(hwnd, psz, cb + sizeof(char));
	if (cb == 0) {
		m_psz = tt::strdup("");
		tt::free(psz);
		return false;
	}
	else
		m_psz = psz;
	return true;
}

#endif	// _WINDOWS_

void ttCStr::MakeLower()
{
	if (m_psz && *m_psz) {
		char* psz = m_psz;
		while (*psz) {
			*psz = (char) tolower(*psz);
			psz = (char*) tt::nextchr(psz);	// handles utf8
		}
	}
}

void ttCStr::MakeUpper()
{
	if (m_psz && *m_psz) {
		char* psz = m_psz;
		while (*psz) {
			*psz = (char) toupper(*psz);
			psz = (char*) tt::nextchr(psz);	// handles utf8
		}
	}
}

bool ttCStr::CopyWide(const wchar_t* pwsz)	// convert UNICODE to UTF8 and store it
{
	if (m_psz) {
		tt::free(m_psz);
		m_psz = nullptr;
	}

	ttASSERT_NONEMPTY(pwsz);

	if (!pwsz || !*pwsz) {
		m_psz = tt::strdup("");
		return false;
	}

	size_t cb = wcslen(pwsz);
	ttASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	// BUGBUG: [randalphwa - 09-09-2018]	WideCharToMultiByte() only works on Windows -- need a portable way to convert

	int cbNew = WideCharToMultiByte(CP_UTF8, 0, pwsz, (int) cb, nullptr, 0, NULL, NULL);
	if (cbNew) {
		m_psz = (char*) tt::malloc(cbNew + sizeof(char));
		cb = WideCharToMultiByte(CP_UTF8, 0, pwsz, (int) cb, m_psz, cbNew, NULL, NULL);
		if (cb == 0)
			tt::free(m_psz);
		else
			m_psz[cb] = 0;
	}
	if (cbNew == 0 || cb == 0) {
		m_psz = tt::strdup("");
		return false;
	}

	return true;
}

void ttCStr::resize(size_t cbNew)
{
	ttASSERT(cbNew <= MAX_STRING);
	if (cbNew > MAX_STRING)
		cbNew = MAX_STRING;

	size_t curSize = m_psz ? tt::size(m_psz) : 0;
	if (cbNew != curSize)
		m_psz = m_psz ? (char*) tt::realloc(m_psz, cbNew) : (char*) tt::malloc(cbNew);
}

bool ttCStr::ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive)
{
	ttASSERT_MSG(pszOldText, "NULL pointer!");
	ttASSERT(*pszOldText);
	ttASSERT(pszNewText);

	if (!pszOldText || !*pszOldText || !pszNewText || !m_psz || !*m_psz)
		return false;

	char* pszPos = bCaseSensitive ? tt::strstr(m_psz, pszOldText) : tt::stristr(m_psz, pszOldText);
	if (!pszPos)
		return false;

	size_t cbOld = tt::strlen(pszOldText);
	size_t cbNew = tt::strlen(pszNewText);

	if (cbNew == 0) {	// delete the old text since new text is empty
		char* pszEnd = m_psz + tt::strbyte(m_psz);
		ptrdiff_t cb = pszEnd - pszPos;
		memmove(pszPos, pszPos + cbOld, cb);
		m_psz = (char*) tt::realloc(m_psz, tt::strbyte(m_psz));
	}
	else if (cbNew == cbOld) {
		while (*pszNewText) {	// copy and return
			*pszPos++ = *pszNewText++;
		}
	}
	else if (cbNew > cbOld) {
		while (cbOld--) {	// replace the old, insert what's left
			*pszPos++ = *pszNewText++;
		}
		ttCStr cszTrail(pszPos);
		*pszPos = 0;
		m_psz = (char*) tt::realloc(m_psz, tt::strbyte(m_psz));
		*this += pszNewText;
		*this += (char*) cszTrail;
	}
	else {	// new text is shorter
		cbOld -= cbNew;
		while (cbNew--) {
			*pszPos++ = *pszNewText++;
		}
		char* pszEnd = m_psz + tt::strbyte(m_psz);
		ptrdiff_t cb = pszEnd - pszPos;
		memmove(pszPos, pszPos + cbOld, cb);
		m_psz = (char*) tt::realloc(m_psz, tt::strbyte(m_psz));
	}
	return true;
}

void ttCStr::operator=(const char* psz)
{
	ttASSERT_MSG(psz, "null pointer!");

	if (m_psz && m_psz == psz)	// This can happen when getting a point to ttCStr and then assigning it to the same ttCStr
		return;

	tt::strdup(psz ? psz : "", &m_psz);
}

void ttCStr::operator+=(const char* psz)
{
	ttASSERT_MSG(m_psz != psz, "Attempt to append string to itself!");
	if (m_psz && m_psz == psz)
		return;
	if (!m_psz)
		m_psz = tt::strdup(psz && *psz ? psz : "");
	else if (!psz || !*psz)
		return;		// nothing to add
	else {
		size_t cbNew = tt::strbyte(psz);
		size_t cbOld = tt::strbyte(m_psz);
		ttASSERT_MSG(cbNew + cbOld <= MAX_STRING, "String is over 64k in size!");
		if (cbNew + cbOld > MAX_STRING)
			return;		// ignore it if it's too large
		m_psz = (char*) tt::realloc(m_psz, cbNew + cbOld);
		tt::strcat(m_psz, psz);
	}
}

void ttCStr::operator+=(char ch)
{
	char szTmp[2];
	szTmp[0] = ch;
	szTmp[1] = 0;
	if (!m_psz)
		m_psz = tt::strdup(szTmp);
	else {
		m_psz = (char*) tt::realloc(m_psz, tt::strbyte(m_psz) + sizeof(char));	// include room for ch
		tt::strcat_s(m_psz, DEST_SIZE, szTmp);
	}
}

void ttCStr::operator+=(ptrdiff_t val)
{
	char szNumBuf[_MAX_U64TOSTR_BASE10_COUNT];
	tt::itoa(val, szNumBuf, sizeof(szNumBuf));
	*this += szNumBuf;
}

char ttCStr::operator[](int pos)
{
	if (!m_psz || pos > (int) tt::strlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

char ttCStr::operator[](size_t pos)
{
	if (!m_psz || pos > tt::strlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

char* cdecl ttCStr::printfAppend(const char* pszFormat, ...)
{
	ttCStr csz;
	va_list argList;
	va_start(argList, pszFormat);
	tt::vprintf(&csz.m_psz, pszFormat, argList);
	va_end(argList);

	*this += csz;

	return m_psz;
}

char* cdecl ttCStr::printf(const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	tt::vprintf(&m_psz, pszFormat, argList);
	va_end(argList);
	return m_psz;
}

char* cdecl ttCStr::printf(size_t idFmtString, ...)
{
	ttCStr cszTmp;
	cszTmp.GetResString(idFmtString);

	va_list argList;
	va_start(argList, idFmtString);
	tt::vprintf(&m_psz, cszTmp, argList);
	va_end(argList);
	return m_psz;
}

int ttCStr::strcat(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");

	if (psz == nullptr)
		return EINVAL;

	if (!m_psz)
		m_psz = tt::strdup(psz);
	else {
		size_t cbNew = tt::strbyte(psz);
		size_t cbOld = tt::strbyte(m_psz);
		ttASSERT_MSG(cbNew + cbOld <= MAX_STRING, "String is over 64k in size!");
		if (cbNew + cbOld > MAX_STRING)
			return EOVERFLOW;		// ignore it if it's too large
		m_psz = (char*) tt::realloc(m_psz, cbNew + cbOld);
		::strcat_s(m_psz, cbNew + cbOld, psz);
	}
	return 0;
}

int ttCStr::strcpy(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");

	if (psz == nullptr)
		return EINVAL;

	if (!m_psz)
		m_psz = tt::strdup(psz);
	else {
		size_t cbNew = tt::strbyte(psz);
		size_t cbOld = tt::size(m_psz);
		ttASSERT_MSG(cbNew + cbOld <= MAX_STRING, "String is over 64k in size!");
		if (cbNew + cbOld > MAX_STRING)
			return EOVERFLOW;		// ignore it if it's too large
		tt::strdup(psz, &m_psz);
	}
	return 0;
}

char* ttCStr::itoa(int32_t val)
{
	char szNum[32];
	tt::itoa(val, szNum, sizeof(szNum));
	return tt::strdup(szNum, &m_psz);
}

char* ttCStr::itoa(int64_t val)
{
	char szNum[32];
	tt::itoa(val, szNum, sizeof(szNum));
	return tt::strdup(szNum, &m_psz);
}

char* ttCStr::utoa(uint32_t val)
{
	char szNum[32];
	tt::utoa(val, szNum, sizeof(szNum));
	return tt::strdup(szNum, &m_psz);
}

char* ttCStr::utoa(uint64_t val)
{
	char szNum[32];
	tt::utoa(val, szNum, sizeof(szNum));
	return tt::strdup(szNum, &m_psz);
}

char* ttCStr::GetString(const char* pszString, char chBegin, char chEnd)
{
	ttASSERT_NONEMPTY(pszString);

	Delete();	// current string, if any, should be deleted no matter what

	if (!pszString || !*pszString)
		return nullptr;

	size_t cb = tt::strbyte(pszString);
	ttASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	if (cb == 0 || cb > MAX_STRING)
		return nullptr;
	else {
		m_psz = (char*) tt::malloc(cb);		// this won't return if it fails, so you will never get a nullptr on return
		*m_psz = 0;
	}

	// step over any leading whitespace
	while (tt::iswhitespace(*pszString))
		++pszString;

	if (*pszString == chBegin) {
		pszString++;
		const char* pszStart = pszString;
		while (*pszString != chEnd && *pszString)
			pszString = tt::nextchr(pszString);
		strncpy_s(m_psz, DEST_SIZE, pszStart, pszString - pszStart);
		m_psz[pszString - pszStart] = 0;	// make certain it is null terminated
	}
	else {	// if the string didn't start with chBegin, so just copy the string
		tt::strcpy_s(m_psz, tt::size(m_psz), pszString);
		pszString += cb;
	}

	// If there is a significant size difference, then reallocate the memory

	if (cb > 32)	// don't bother reallocating if total allocation is 32 bytes or less
		m_psz = (char*) tt::realloc(m_psz, tt::strbyte(m_psz));
	return m_psz;
}

char* ttCStr::GetQuotedString(const char* pszQuote)
{
	ttASSERT_NONEMPTY(pszQuote);

	if (!pszQuote || !*pszQuote) {
		Delete();	// any current string should be deleted no matter what
		return nullptr;
	}

	while (tt::iswhitespace(*pszQuote)) // step over any leading whitespace
		++pszQuote;

	switch (*pszQuote) {
		default:
		case '"':	// CH_QUOTE
			return GetString(pszQuote, CH_QUOTE, CH_QUOTE);

		case '\'':	// CH_SQUOTE
			return GetString(pszQuote, CH_SQUOTE, CH_SQUOTE);

		case '`':	// CH_START_QUOTE
			return GetString(pszQuote, CH_START_QUOTE, CH_END_QUOTE);

		case '<':
			return GetString(pszQuote, '<', '>');

		case '[':	// CH_LEFT_BRACKET
			return GetString(pszQuote, '[', ']');

		case '(':	// CH_OPEN_PAREN
			return GetString(pszQuote, '[', ']');
	}
}
