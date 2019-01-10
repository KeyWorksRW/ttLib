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

const char* ttString::FindExt() const
{
	const char* psz = tt::strchrR(m_psz, '.');
	if (psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/')	// ignore .file, ./file, and ../file
		return "";
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

const char* ttString::FindLastSlash()
{
	ttASSERT_MSG(m_psz, "NULL pointer!");

	if (!m_psz || !*m_psz)
		return nullptr;

	const char* pszLastBackSlash = tt::strchrR(m_psz, '\\');
	const char* pszLastFwdSlash	 = tt::strchrR(m_psz, '/');
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

const char*	 ttString::GetListBoxText(HWND hwnd, size_t sel)
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

const char* ttString::GetResString(size_t idString)
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

const char* __cdecl ttString::printfAppend(const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	vprintf(pszFormat, argList);
	va_end(argList);
	return m_psz;
}

const char* __cdecl ttString::printf(const char* pszFormat, ...)
{
	if (m_psz) {
		tt::free(m_psz);
		m_psz = nullptr;
	}

	va_list argList;
	va_start(argList, pszFormat);
	vprintf(pszFormat, argList);
	va_end(argList);
	return m_psz;
}

const char* __cdecl ttString::printf(size_t idFmtString, ...)
{
	if (m_psz) {
		tt::free(m_psz);
		m_psz = nullptr;
	}

	ttMem szStringBuf(1024);

	if (LoadStringA(tt::hinstResources, (UINT) idFmtString, szStringBuf, 1024) == 0) {
#ifdef _DEBUG
		ttString strMsg;
		strMsg.printf("Invalid string id: %zu", idFmtString);
		ttFAIL(strMsg);
#endif
		if (m_psz)
			tt::free(m_psz);
		m_psz = tt::strdup("");
		return m_psz;
	}

	va_list argList;
	va_start(argList, idFmtString);
	vprintf(szStringBuf, argList);
	va_end(argList);
	return m_psz;
}

#define CB_MAX_FMT_WIDTH 20		// Largest formatted width we allow

void ttString::vprintf(const char* pszFormat, va_list argList)
{
	ttASSERT_MSG(pszFormat, "NULL pointer!");
	ttASSERT_MSG(*pszFormat, "Empty format string!");
	if (!pszFormat || !*pszFormat) {
		*this += "";
		return;
	}

	size_t cAvail;

	if (m_psz) {
		cAvail = tt::strbyte(m_psz);
		cAvail >>=8;	// remove lower bits
		cAvail <<=8;
		cAvail += 0x100;	// allocate 256 byte blocks at a time
		m_psz = (char*) tt::realloc(m_psz, cAvail);
		cAvail--;	// don't include null-terminator
	}
	else {
		cAvail = 255;
		m_psz = (char*) tt::malloc(cAvail + 1);
		*m_psz = 0;
	}

	const char* pszEnd = pszFormat;
	char szNumBuf[50];	// buffer used for converting numbers including plenty of room for commas

	while (*pszEnd) {
		if (*pszEnd != '%') {
			const char* pszBegin = pszEnd++;
			while (*pszEnd && *pszEnd != '%') {
				pszEnd++;
			}
			size_t cb = (pszEnd - pszBegin);
			if (!cb)
				return;	// empty format string
			cb += tt::strbyte(m_psz);
			ttASSERT(cb <= MAX_STRING);
			if (cb > MAX_STRING) // empty or invalid string
				return;

			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb +=  0x80;	// round up allocation size to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}

			char* pszTmp = m_psz + tt::strlen(m_psz);
			while (pszBegin < pszEnd) {
				*pszTmp++ = *pszBegin++;
			}
			*pszTmp = '\0';

			if (!*pszEnd)
				return;
		}
		pszEnd++;
		if (*pszEnd == 'k') {	// Special KeyWorks formatting functionality
			pszEnd = ProcessKFmt(pszEnd + 1, &argList);
			continue;
		}

		bool bLarge = false;
		bool bSize_t = false;
		if (*pszEnd == 'l')	{
			bLarge = true;
			pszEnd++;	// ignore the prefix
		}
		if (*pszEnd == 'z' || *pszEnd == 'I')	{	// use to specify size_t argument with width dependent on _W64 definition
			bSize_t = true;
			pszEnd++;	// ignore the prefix
		}
		else if ((pszEnd[1] == 'i' || pszEnd[1] == 'd' || pszEnd[1] == 'u' || pszEnd[1] == 'x' || pszEnd[1] == 'X')) {
			pszEnd++;	// ignore the prefix
		}

		char chPad = ' ';
		ptrdiff_t cbMin = -1;

		if (*pszEnd == '0') {
			chPad = 0;
			pszEnd++;
		}
		if (tt::isdigit(*pszEnd)) {
			cbMin = tt::atoi(pszEnd++);
			if (cbMin > CB_MAX_FMT_WIDTH)
				cbMin = CB_MAX_FMT_WIDTH;
			while (tt::isdigit(*pszEnd))
				pszEnd++;
		}

		if (*pszEnd == 'c') {
			char szBuf[2];
			szBuf[0] = (uint8_t) (va_arg (argList, int) & 0xFF);
			szBuf[1] = '\0';
			size_t cb = tt::strbyte(m_psz) + sizeof(char);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			tt::strcat_s(m_psz, DEST_SIZE, szBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'C') {
			wchar_t szwBuf[2];
			szwBuf[0] = (wchar_t) (va_arg (argList, int) & 0xFFFF);
			szwBuf[1] = '\0';
			char szBuf[4];

			size_t cb = WideCharToMultiByte(CP_ACP, 0, szwBuf, sizeof(szwBuf), szBuf, sizeof(szBuf), NULL, NULL);
			ttASSERT(cb < sizeof(szBuf));
			szBuf[cb] = '\0';
			cb = tt::strbyte(m_psz) + tt::strbyte(szBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			tt::strcat_s(m_psz, DEST_SIZE, szBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'd' || *pszEnd == 'i') {

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)	// TODO: [randalphwa - 08-30-2018] Need CLANG preprocessor flag
			// note that we don't have to do any special processing if not compiling 64-bit app, as size_t will be same as int
			if (bSize_t) {
				tt::itoa(va_arg(argList, _int64), szNumBuf, sizeof(szNumBuf) - 1);
			}
			else {
				tt::itoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
			}
#else	// not defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			tt::itoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
#endif	// defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			size_t cb = tt::strbyte(m_psz) + tt::strbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = tt::strbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ttASSERT(cAvail < 4096);
					}
					tt::strcat_s(m_psz, DEST_SIZE, szTmp);
				}
			}
			tt::strcat_s(m_psz, DEST_SIZE, szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'u') {
			tt::utoa(va_arg(argList, unsigned int), szNumBuf, sizeof(szNumBuf));
			size_t cb = tt::strbyte(m_psz) + tt::strbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = tt::strbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ttASSERT(cAvail < 4096);
					}
					tt::strcat_s(m_psz, DEST_SIZE, szTmp);
				}
			}
			tt::strcat_s(m_psz, DEST_SIZE, szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'x') {
			tt::hextoa(va_arg(argList, int), szNumBuf, false);
			size_t cb = tt::strbyte(m_psz) + tt::strbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = tt::strbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ttASSERT(cAvail < 4096);
					}
					tt::strcat_s(m_psz, DEST_SIZE, szTmp);
				}
			}
			tt::strcat_s(m_psz, DEST_SIZE, szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'X') {
			ttASSERT_MSG(!bSize_t, "zX and IX not supported");
			tt::hextoa(va_arg(argList, int), szNumBuf, true);
			size_t cb = tt::strbyte(m_psz) + tt::strbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = tt::strbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ttASSERT(cAvail < 4096);
					}
					tt::strcat_s(m_psz, DEST_SIZE, szTmp);
				}
			}
			tt::strcat_s(m_psz, DEST_SIZE, szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 's') {
			if (bLarge)
				goto WideChar;
			const char* psz = va_arg(argList, char*);

			if (!psz || psz <= (const char*) 0xFFFF
#ifdef _WINDOWS_
					|| IsBadReadPtr(psz, 1)	// IsBadReadPtr() is technically obsolete, but it prevents a crash if caller forgets to supply enough parameters.
#endif
					) {
				ttASSERT_MSG(psz, "NULL pointer passed to ttString::printf(\"%s");
				psz = "(missing argument for %s)";
			}
			size_t cb = tt::strbyte(m_psz) + tt::strbyte(psz);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			tt::strcat_s(m_psz, DEST_SIZE, psz);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'S') {
WideChar:
			const wchar_t* pwsz = (wchar_t*) va_arg(argList, wchar_t*);
			if (!pwsz)
				pwsz = L"(null)";

			size_t cb = tt::strlen(pwsz) * sizeof(wchar_t);
			ttASSERT(cb < MAX_STRING);
			if (cb <= 0 || cb > MAX_STRING) // empty or invalid string
				return;

			char* psz = (char*) tt::malloc(cb + 1);
			// BUGBUG: [ralphw - 07-14-2018] Following line is _WINDOWS_ only
			cb = WideCharToMultiByte(CP_ACP, 0, pwsz, (_int32) (cb / sizeof(wchar_t)), psz, (_int32) cb, nullptr, nullptr);
			psz[cb] = '\0';

			cb = tt::strbyte(m_psz) + tt::strbyte(psz);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			tt::strcat_s(m_psz, DEST_SIZE, psz);
			pszEnd++;
			tt::free(psz);
			continue;
		}
		else if (*pszEnd == '%') {
			size_t cb = tt::strbyte(m_psz) + sizeof(char);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			tt::strcat_s(m_psz, DEST_SIZE, "%");
			pszEnd++;
			continue;
		}
		else {
			// This is a potential security risk since we don't know what size of argument to retrieve from va_arg(). We simply
			// print the rest of the format string and don't pop any arguments off.

			ttFAIL("Invalid format string for printf");
			size_t cb;
#ifdef _DEBUG
			cb = tt::strbyte(m_psz) + tt::strbyte("Invalid format string: ");
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			tt::strcat_s(m_psz, DEST_SIZE, "Invalid format string: ");
#endif // _DEBUG
			cb = tt::strbyte(m_psz) + tt::strbyte(pszEnd) + sizeof(char);	// make room for leading % character
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			tt::strcat_s(m_psz, DEST_SIZE, "%");
			tt::strcat_s(m_psz, DEST_SIZE, pszEnd);
			break;
		}
	}

	// Now readjust the allocation to the actual size

	m_psz = (char*) tt::realloc(m_psz, tt::strbyte(m_psz));
}

const char* ttString::ProcessKFmt(const char* pszEnd, va_list* pargList)
{
	char szBuf[256];
	szBuf[0] = '\0';
	switch (*pszEnd) {
		case 'n':	// 'n' is deprecated, 'd' should be used instead
		case 'd':
			tt::itoa((int) va_arg(*pargList, int), szBuf, sizeof(szBuf));
			ttpriv::AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			break;

		case 'I':	// 64-bit version of 'd' and 'u' that works in 32-bit builds
			if (tt::samesubstri(pszEnd, "I64d"))
				tt::itoa(va_arg(*pargList, int64_t), szBuf, sizeof(szBuf));
			else if (tt::samesubstri(pszEnd, "I64u"))
				tt::utoa(va_arg(*pargList, uint64_t), szBuf, sizeof(szBuf));
			ttpriv::AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			pszEnd += 3;	// skip over I64 portion, then count normally
			break;

		case 't':	// use for size_t parameters, this will handle both 32 and 64 bit compilations
			tt::utoa(va_arg(*pargList, size_t), szBuf, sizeof(szBuf));
			ttpriv::AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			break;

		case 'u':
			tt::utoa(va_arg(*pargList, unsigned int), szBuf, sizeof(szBuf));
			ttpriv::AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			break;

		case 's':
			if (va_arg(*pargList, int) != 1) {
				szBuf[0] = 's';
				szBuf[1] = '\0';
			}
			break;

		case 'S':
			if (va_arg(*pargList, _int64) != 1) {
				szBuf[0] = 's';
				szBuf[1] = '\0';
			}
			break;

#ifdef _WINDOWS_
		case 'r':
			{
				ttString cszRes;
				cszRes.GetResString(va_arg(*pargList, int));
				tt::strcpy_s(szBuf, sizeof(szBuf), cszRes);
			}
			break;


		case 'e':
			{
				char* pszMsg;

				FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, va_arg(*pargList, int), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(char*) &pszMsg, 0, NULL);
				tt::strcpy_s(szBuf, sizeof(szBuf), pszMsg);
				LocalFree((HLOCAL) pszMsg);
			}
			break;
#endif // _WINDOWS_

		case 'q':
			try {
				const char* psz = va_arg(*pargList, const char*);
				size_t cb = tt::strbyte(m_psz) + tt::strbyte(psz) + (2 * sizeof(char));	// add room for both quotes
				if (cb > tt::size(m_psz)) {
					cb >>= 7;
					cb <<= 7;
					cb += 0x80;	// round up to 128
					m_psz = (char*) tt::realloc(m_psz, cb);
				}
				tt::strcat(m_psz, "\042");
				tt::strcat(m_psz, psz);
				tt::strcat(m_psz, "\042");
			}
			catch (...) {
				ttFAIL("Exception in printf -- bad %%kq pointer");
			}
			break;
	}
	if (szBuf[0]) {
		size_t cbCur = tt::strbyte(m_psz);
		size_t cb = cbCur + tt::strbyte(szBuf) + sizeof(char);
		if (cb > tt::size(m_psz)) {
			cb >>= 7;
			cb <<= 7;
			cb += 0x80;	// round up to 128
			m_psz = (char*) tt::realloc(m_psz, cb);
		}
		tt::strcat_s(m_psz, DEST_SIZE - cbCur, szBuf);
	}
	return pszEnd + 1;
}

// We allow for pszNum and pszDst to be different in case the pszNum buffer is only large
// enough to hold the number and not the commas

void ttpriv::AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst)
{
	if (pszDst != pszNum)
		tt::strcpy_s(pszDst, cbDst, pszNum);	// copy the number, performa all additional work in-place in the destination buffer

	ptrdiff_t cbNum = tt::strlen(pszDst);	// needs to be signed because it can go negative
	if (cbNum < 4) {
		ttASSERT(cbNum < (ptrdiff_t) cbDst);
		return;
	}
	ttASSERT(cbNum + (cbNum / 3) < (ptrdiff_t) cbDst);
	if (cbNum + (cbNum / 3) >= (ptrdiff_t) cbDst)
		return;

	if (*pszDst == '-')	{
		cbNum--;
		if (cbNum < 4)
			return;
	}

	ptrdiff_t cbStart = cbNum % 3;
	if (cbStart == 0)
		cbStart += 3;
	while (cbStart < cbNum) {
		memmove(pszDst + cbStart + 1, pszDst + cbStart, tt::strbyte(pszDst + cbStart));	// make space for a comma
		pszDst[cbStart] = ',';
		++cbNum;		// track that we added a comma for loop comparison
		cbStart += 4;	// 3 numbers plus the comma
	}
}

