/////////////////////////////////////////////////////////////////////////////
// Name:		ttString
// Purpose:		SBCS string class
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <direct.h>		// for getcwd
#include <cctype>		// for tolower
#include <cwchar>		// for wcslen

#include "../include/ttstring.h"	// ttString
#include "../include/ttstr.h"	// various kstr() functions
#include "../include/ttmem.h"	// ttMem, ttTMem

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

void ttString::AppendFileName(const char* pszFile)
{
	ttASSERT_MSG(pszFile, "NULL pointer!");
	ttASSERT_MSG(*pszFile, "Empty string!");

	if (!pszFile || !*pszFile)
		return;

	if (!m_psz)	{	// no folder or drive to append to, so leave as is without adding slash
		m_psz = tt::strdup(pszFile);		// REVIEW: [ralphw - 06-03-2018] We could prefix this with ".\"
		return;
	}

	AddTrailingSlash();
	*this += pszFile;
}

void ttString::ChangeExtension(const char* pszExtension)
{
	ttASSERT_MSG(pszExtension, "NULL pointer!");
	ttASSERT_MSG(*pszExtension, "Empty string!");

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

char* ttString::FindExt() const
{
	char* psz = tt::strchrR(m_psz, '.');
	if (psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/')	// ignore .file, ./file, and ../file
		return nullptr;
	return psz;
}

void ttString::RemoveExtension()
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

void ttString::AddTrailingSlash()
{
	if (!m_psz) {
		m_psz = tt::strdup("/");
		return;
	}
	const char* pszLastSlash = FindLastSlash();
	if (!pszLastSlash || pszLastSlash[1])	// only add if there was no slash or there was something after the slash
		*this += "/";
}

char* ttString::FindLastSlash()
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

void ttString::GetCWD()
{
	if (m_psz)
		tt::free(m_psz);
	char szCWD[1024];
	_getcwd(szCWD, sizeof(szCWD));
	m_psz = tt::strdup(szCWD);
}

#ifdef _WINDOWS_

void ttString::GetFullPathName()
{
	ttASSERT(m_psz);
	char szPath[MAX_PATH];
	::GetFullPathNameA(m_psz, sizeof(szPath), szPath, NULL);
	tt::free(m_psz);
	m_psz = tt::strdup(szPath);
}

char* ttString::GetListBoxText(HWND hwnd, size_t sel)
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

char* ttString::GetResString(size_t idString)
{
	char szStringBuf[1024];

	if (LoadStringA(tt::hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0) {
		ttString strMsg;
		strMsg.printf("Invalid string id: %zu", idString);
		ttFAIL(strMsg);
		if (m_psz)
			tt::free(m_psz);
		m_psz = tt::strdup("");
	}
	else {
		if (m_psz)
			tt::free(m_psz);
		m_psz = tt::strdup(szStringBuf);
	}
	return m_psz;
}

bool ttString::GetWindowText(HWND hwnd)
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

char* ttString::GetQuotedString(const char* pszQuote)
{
	ttASSERT_MSG(pszQuote, "NULL pointer!");
	ttASSERT_MSG(*pszQuote, "Empty string!");

	if (!pszQuote || !*pszQuote) {
		Delete();	// current string, if any, should be deleted no matter what
		return nullptr;
	}

	size_t cb = tt::strbyte(pszQuote);
	ttASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	if (m_psz)
		tt::free(m_psz);

	if (cb == 0 || cb > MAX_STRING) {
		m_psz = nullptr;	// it was already freed above
		return nullptr;
	}
	else {
		m_psz = (char*) tt::malloc(cb);		// this won't return if it fails, so you will never get a nullptr on return
		*m_psz = 0;
	}

	if (*pszQuote == '"') {
		pszQuote++;
		const char* pszStart = pszQuote;
		while (*pszQuote != '"' && *pszQuote) {
			pszQuote = tt::nextchr(pszQuote);
		}
		strncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else if (*pszQuote == CH_START_QUOTE || *pszQuote == CH_END_QUOTE) {
		pszQuote++;
		const char* pszStart = pszQuote;
		while (*pszQuote != CH_END_QUOTE && *pszQuote) {
			pszQuote = tt::nextchr(pszQuote);
		}
		strncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else if (*pszQuote == '<') {
		pszQuote++;
		const char* pszStart = pszQuote;
		while (*pszQuote != '>' && *pszQuote) {
			pszQuote = tt::nextchr(pszQuote);
		}
		strncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else {
		tt::strcpy_s(m_psz, DEST_SIZE, pszQuote);
		pszQuote += cb;
	}

	// If there is a significant size difference, then reallocate the memory

	if (cb > 32) {	// don't bother if total allocation is 32 bytes or less
		size_t cbNew = tt::strbyte(m_psz);
		if (cbNew < cb - 32)
			m_psz = (char*) tt::realloc(m_psz, cbNew);
	}
	return m_psz;
}

void ttString::MakeLower()
{
	if (m_psz && *m_psz) {
		char* psz = m_psz;
		while (*psz) {
			*psz = (char) std::tolower(*psz);
			psz = (char*) tt::nextchr(psz);	// handles utf8
		}
	}
}

void ttString::MakeUpper()
{
	if (m_psz && *m_psz) {
		char* psz = m_psz;
		while (*psz) {
			*psz = (char) std::toupper(*psz);
			psz = (char*) tt::nextchr(psz);	// handles utf8
		}
	}
}

bool ttString::CopyWide(const wchar_t* pwsz)	// convert UNICODE to UTF8 and store it
{
	if (m_psz) {
		tt::free(m_psz);
		m_psz = nullptr;
	}

	ttASSERT_MSG(pwsz, "NULL pointer!");
	ttASSERT_MSG(*pwsz, "empty string!");

	if (!pwsz || !*pwsz) {
		m_psz = tt::strdup("");
		return false;
	}

	size_t cb = std::wcslen(pwsz);
	ttASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	// BUGBUG: [randalphwa - 09-09-2018]	WideCharToMultiByte() only works on Windows -- need a portable way to convert

	int cbNew = WideCharToMultiByte(CP_UTF8, 0, pwsz, (int) cb, nullptr, 0, NULL, NULL);
	if (cbNew) {
		m_psz = (char*) tt::malloc(cbNew + sizeof(char));
		cb = WideCharToMultiByte(CP_UTF8, 0, pwsz, (int) cb, m_psz, cbNew, NULL, NULL);
		if (cb == 0)
			tt::free(m_psz);
		else
			m_psz[(cb * sizeof(char)) + sizeof(char)] = 0;	// sizeof(char) is 2 if compiled with UNICODE
	}
	if (cbNew == 0 || cb == 0) {
		m_psz = tt::strdup("");
		return false;
	}

	return true;
}

char* ttString::Enlarge(size_t cbTotalSize)
{
	ttASSERT(cbTotalSize <= MAX_STRING);
	if (cbTotalSize > MAX_STRING)
		cbTotalSize = MAX_STRING;

	size_t curSize = m_psz ? tt::size(m_psz) : 0;
	if (cbTotalSize <= curSize)
		return m_psz;

	if (m_psz)
		m_psz = (char*) tt::realloc(m_psz, cbTotalSize);
	else
		m_psz = (char*) tt::malloc(cbTotalSize);
	return m_psz;
}

bool ttString::ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive)
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
		ttString cszTrail(pszPos);
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

void ttString::operator=(const char* psz)
{
	ttASSERT_MSG(psz, "null pointer!");

	if (m_psz && m_psz == psz)	// This can happen when getting a point to ttString and then assigning it to the same ttString
		return;

	if (m_psz)
		tt::free(m_psz);

	m_psz = tt::strdup(psz ? psz : "");
}

void ttString::operator+=(const char* psz)
{
	ttASSERT_MSG(m_psz != psz, "Attempt to append string to itself!");
	if (m_psz && m_psz == psz)
		return;
	if (!m_psz)
		m_psz = tt::strdup(psz && *psz ? psz : "");
	else if (!psz || !*psz)
		m_psz = tt::strdup("");
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

void ttString::operator+=(char ch)
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

void ttString::operator+=(ptrdiff_t val)
{
	char szNumBuf[_MAX_U64TOSTR_BASE10_COUNT];
	tt::itoa(val, szNumBuf, sizeof(szNumBuf));
	*this += szNumBuf;
}

char ttString::operator[](int pos)
{
	if (!m_psz || pos > (int) tt::strlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

char ttString::operator[](size_t pos)
{
	if (!m_psz || pos > tt::strlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

char* cdecl ttString::printfAppend(const char* pszFormat, ...)
{
	ttStr csz;
	va_list argList;
	va_start(argList, pszFormat);
	tt::vprintf(&csz.m_psz, pszFormat, argList);
	va_end(argList);

	*this += csz;

	return m_psz;
}

char* cdecl ttString::printf(const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	tt::vprintf(&m_psz, pszFormat, argList);
	va_end(argList);
	return m_psz;
}

char* cdecl ttString::printf(size_t idFmtString, ...)
{
	ttStr cszTmp;
	cszTmp.GetResString(idFmtString);

	va_list argList;
	va_start(argList, idFmtString);
	tt::vprintf(&m_psz, cszTmp, argList);
	va_end(argList);
	return m_psz;
}
