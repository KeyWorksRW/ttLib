/////////////////////////////////////////////////////////////////////////////
// Name:		CStr
// Purpose:		SBCS string class
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include <direct.h>		// for _getcwd

#include "../include/cstr.h"	// CStr
#include "../include/cwstr.h"	// CWStr
#include "../include/kstr.h"	// various kstr() functions

#ifdef _WINDOWS_
	extern HINSTANCE	_hinstResources;
#endif

#ifndef _MAX_U64TOSTR_BASE10_COUNT
	#define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

// Note that the limit here of 64k is smaller then the kstr functions that use 16m (_KSTRMAX)

#define MAX_STRING (64 * 1024)	// Use this to limit the length of a single string as a security precaution
#define	DEST_SIZE (tt::size(m_psz) - sizeof(char))

void CStr::AppendFileName(const char* pszFile)
{
	ASSERT_MSG(pszFile, "NULL pointer!");
	ASSERT_MSG(*pszFile, "Empty string!");

	if (!pszFile || !*pszFile)
		return;

	if (!m_psz)	{	// no folder or drive to append to, so leave as is without adding slash
		m_psz = tt::strdup(pszFile);		// REVIEW: [ralphw - 06-03-2018] We could prefix this with ".\"
		return;
	}

	AddTrailingSlash();
	*this += pszFile;
}

void CStr::ChangeExtension(const char* pszExtension)
{
	ASSERT_MSG(pszExtension, "NULL pointer!");
	ASSERT_MSG(*pszExtension, "Empty string!");

	if (!pszExtension || !*pszExtension)
		return;

	if (!m_psz)
		m_psz = tt::strdup("");

	char* psz = kstrchrR(m_psz, '.');
	if (psz && !(psz == m_psz || *(psz - 1) == '.' || psz[1] == CH_BACKSLASH || psz[1] == CH_FORWARDSLASH))	// ignore .file, ./file, and ../file
		*psz = 0;	// remove the extension if none of the above is true

	if (*pszExtension != '.')
		*this += ".";
	*this += pszExtension;
}

const char* CStr::FindExt() const
{
	const char* psz = kstrchrR(m_psz, '.');
	if (psz == m_psz || *(psz - 1) == '.' || psz[1] == CH_BACKSLASH || psz[1] == CH_FORWARDSLASH)	// ignore .file, ./file, and ../file
		return "";
	return psz;
}

void CStr::RemoveExtension()
{
	 if (m_psz) {
		char* psz = kstrchrR(m_psz, '.');
		if (psz) {
			if (psz == m_psz || *(psz - 1) == '.' || psz[1] == CH_BACKSLASH || psz[1] == CH_FORWARDSLASH)	// ignore .file, ./file, and ../file
				return;
			*psz = 0;
		}
	}
}

void CStr::AddTrailingSlash()
{
	if (!m_psz) {
		m_psz = tt::strdup("/");
		return;
	}
	const char* pszLastSlash = FindLastSlash();
	if (!pszLastSlash || pszLastSlash[1])	// only add if there was no slash or there was something after the slash
		*this += "/";
}

const char* CStr::FindLastSlash()
{
	ASSERT_MSG(m_psz, "NULL pointer!");

	if (!m_psz || !*m_psz)
		return nullptr;

	const char* pszLastBackSlash = kstrchrR(m_psz, CH_BACKSLASH);
	const char* pszLastFwdSlash	 = kstrchrR(m_psz, CH_FORWARDSLASH);
	if (!pszLastBackSlash)
		return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
	else if (!pszLastFwdSlash)
		return pszLastBackSlash ? pszLastBackSlash : nullptr;
	else
		return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash : pszLastBackSlash;		// Impossible for them to be equal
}

void CStr::GetCWD()
{
	if (m_psz)
		tt::free(m_psz);
	char szCWD[1024];
	_getcwd(szCWD, sizeof(szCWD));
	m_psz = tt::strdup(szCWD);
}

#ifdef _WINDOWS_

void CStr::GetFullPathName()
{
	ASSERT(m_psz);
	char szPath[MAX_PATH];
	::GetFullPathNameA(m_psz, sizeof(szPath), szPath, NULL);
	tt::free(m_psz);
	m_psz = tt::strdup(szPath);
}

const char*	 CStr::GetListBoxText(HWND hwnd, size_t sel)
{
	if (m_psz)
		tt::free(m_psz);
	if (sel == (size_t) LB_ERR)
		m_psz = tt::strdup("");
	else {
		size_t cb = ::SendMessage(hwnd, LB_GETTEXTLEN, sel, 0);
		ASSERT(cb != (size_t) LB_ERR);
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
	_hinstResources is typically set by InitCaller() and determines where to load resources from. If you need to load the resources
	from a DLL, then first call:

		_hinstResources = LoadLibrary("dll name");
*/

const char* CStr::GetResString(size_t idString)
{
	char szStringBuf[1024];

	if (LoadStringA(_hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0) {
		CStr cszMsg;
		cszMsg.printf("Invalid string id: %zu", idString);
		FAIL(cszMsg);
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

bool CStr::GetWindowText(HWND hwnd)
{
	if (m_psz) {
		 tt::free(m_psz);
		 m_psz = nullptr;
	}

	ASSERT_MSG(IsValidWindow(hwnd), "Invalid window handle");
	if (!IsValidWindow(hwnd)) {
		m_psz = tt::strdup("");
		return false;
	}

	int cb = GetWindowTextLengthA(hwnd);
	ASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

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

char* CStr::GetQuotedString(const char* pszQuote)
{
	ASSERT_MSG(pszQuote, "NULL pointer!");
	ASSERT_MSG(*pszQuote, "Empty string!");

	if (!pszQuote || !*pszQuote) {
		Delete();	// current string, if any, should be deleted no matter what
		return nullptr;
	}

	size_t cb = kstrbyte(pszQuote);
	ASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

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

	if (*pszQuote == CH_QUOTE) {
		pszQuote++;
		const char* pszStart = pszQuote;
		while (*pszQuote != CH_QUOTE && *pszQuote) {
			pszQuote = knextchr(pszQuote);
		}
		strncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else if (*pszQuote == CH_START_QUOTE || *pszQuote == CH_END_QUOTE) {
		pszQuote++;
		const char* pszStart = pszQuote;
		while (*pszQuote != CH_END_QUOTE && *pszQuote) {
			pszQuote = knextchr(pszQuote);
		}
		strncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else if (*pszQuote == '<') {
		pszQuote++;
		const char* pszStart = pszQuote;
		while (*pszQuote != '>' && *pszQuote) {
			pszQuote = knextchr(pszQuote);
		}
		strncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else {
		kstrcpy(m_psz, DEST_SIZE, pszQuote);
		pszQuote += cb;
	}

	// If there is a significant size difference, then reallocate the memory

	if (cb > 32) {	// don't bother if total allocation is 32 bytes or less
		size_t cbNew = kstrbyte(m_psz);
		if (cbNew < cb - 32)
			m_psz = (char*) tt::realloc(m_psz, cbNew);
	}
	return m_psz;
}

bool CStr::IsSameSubString(const char* pszSub)
{
	if (!m_psz || !pszSub)
		return false;
	return ::IsSameSubString(m_psz, pszSub);
}

void CStr::MakeLower()
{
	if (m_psz && *m_psz) {
		char* psz = m_psz;
		while (*psz) {
			*psz = (char) tolower(*psz);
			psz = knextchr(psz);	// handles utf8
		}
	}
}

void CStr::MakeUpper()
{
	if (m_psz && *m_psz) {
		char* psz = m_psz;
		while (*psz) {
			*psz = (char) toupper(*psz);
			psz = knextchr(psz);	// handles utf8
		}
	}
}

bool CStr::CopyWide(const wchar_t* pwsz)	// convert UNICODE to UTF8 and store it
{
	if (m_psz) {
		tt::free(m_psz);
		m_psz = nullptr;
	}

	ASSERT_MSG(pwsz, "NULL pointer!");
	ASSERT_MSG(*pwsz, "empty string!");

	if (!pwsz || !*pwsz) {
		m_psz = tt::strdup("");
		return false;
	}

	size_t cb = wcslen(pwsz);
	ASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

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

char* CStr::Enlarge(size_t cbTotalSize)
{
	ASSERT(cbTotalSize <= MAX_STRING);
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

bool CStr::ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive)
{
	ASSERT_MSG(pszOldText, "NULL pointer!");
	ASSERT(*pszOldText);
	ASSERT(pszNewText);

	if (!pszOldText || !*pszOldText || !pszNewText || !m_psz || !*m_psz)
		return false;

	char* pszPos = bCaseSensitive ? kstrstr(m_psz, pszOldText) : kstristr(m_psz, pszOldText);
	if (!pszPos)
		return false;

	size_t cbOld = kstrlen(pszOldText);
	size_t cbNew = kstrlen(pszNewText);

	if (cbNew == 0) {	// delete the old text since new text is empty
		char* pszEnd = m_psz + kstrbyte(m_psz);
		ptrdiff_t cb = pszEnd - pszPos;
		memmove(pszPos, pszPos + cbOld, cb);
		m_psz = (char*) tt::realloc(m_psz, kstrbyte(m_psz));
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
		CStr cszTrail(pszPos);
		*pszPos = 0;
		m_psz = (char*) tt::realloc(m_psz, kstrbyte(m_psz));
		*this += pszNewText;
		*this += (char*) cszTrail;
	}
	else {	// new text is shorter
		cbOld -= cbNew;
		while (cbNew--) {
			*pszPos++ = *pszNewText++;
		}
		char* pszEnd = m_psz + kstrbyte(m_psz);
		ptrdiff_t cb = pszEnd - pszPos;
		memmove(pszPos, pszPos + cbOld, cb);
		m_psz = (char*) tt::realloc(m_psz, kstrbyte(m_psz));
	}
	return true;
}

void CStr::operator=(const char* psz)
{
	ASSERT_MSG(psz, "null pointer!");

	if (m_psz && m_psz == psz)	// This can happen when getting a point to CStr and then assigning it to the same CStr
		return;

	if (m_psz)
		tt::free(m_psz);

	m_psz = tt::strdup(psz ? psz : "");
}

void CStr::operator+=(const char* psz)
{
	ASSERT_MSG(m_psz != psz, "Attempt to append string to itself!");
	if (m_psz && m_psz == psz)
		return;
	if (!m_psz)
		m_psz = tt::strdup(psz && *psz ? psz : "");
	else if (!psz || !*psz)
		m_psz = tt::strdup("");
	else {
		size_t cbNew = kstrbyte(psz);
		size_t cbOld = kstrbyte(m_psz);
		ASSERT_MSG(cbNew + cbOld <= MAX_STRING, "String is over 64k in size!");
		if (cbNew + cbOld > MAX_STRING)
			return;		// ignore it if it's too large
		m_psz = (char*) tt::realloc(m_psz, cbNew + cbOld);
		kstrcat(m_psz, psz);
	}
}

void CStr::operator+=(char ch)
{
	char szTmp[2];
	szTmp[0] = ch;
	szTmp[1] = 0;
	if (!m_psz)
		m_psz = tt::strdup(szTmp);
	else {
		m_psz = (char*) tt::realloc(m_psz, kstrbyte(m_psz) + sizeof(char));	// include room for ch
		kstrcat(m_psz, DEST_SIZE, szTmp);
	}
}

void CStr::operator+=(ptrdiff_t val)
{
	char szNumBuf[_MAX_U64TOSTR_BASE10_COUNT];
	Itoa(val, szNumBuf, sizeof(szNumBuf));
	*this += szNumBuf;
}

char CStr::operator[](int pos)
{
	if (!m_psz || pos > (int) kstrlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

char CStr::operator[](size_t pos)
{
	if (!m_psz || pos > kstrlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

const char* __cdecl CStr::printfAppend(const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	vprintf(pszFormat, argList);
	va_end(argList);
	return m_psz;
}

const char* __cdecl CStr::printf(const char* pszFormat, ...)
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

const char* __cdecl CStr::printf(size_t idFmtString, ...)
{
	if (m_psz) {
		tt::free(m_psz);
		m_psz = nullptr;
	}

	CMem szStringBuf(1024);

	if (LoadStringA(_hinstResources, (UINT) idFmtString, szStringBuf, 1024) == 0) {
#ifdef _DEBUG
		CStr cszMsg;
		cszMsg.printf("Invalid string id: %zu", idFmtString);
		FAIL(cszMsg);
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

// Because we are using _alloca, we limit string size to 64k

#define CB_MAX_FMT_WIDTH 20		// Largest formatted width we allow

void CStr::vprintf(const char* pszFormat, va_list argList)
{
	ASSERT_MSG(pszFormat, "NULL pointer!");
	ASSERT_MSG(*pszFormat, "Empty format string!");
	if (!pszFormat || !*pszFormat) {
		*this += "";
		return;
	}

	size_t cAvail;

	if (m_psz) {
		cAvail = kstrbyte(m_psz);
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
			cb += kstrbyte(m_psz);
			ASSERT(cb <= MAX_STRING);
			if (cb > MAX_STRING) // empty or invalid string
				return;

			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb +=  0x80;	// round up allocation size to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}

			char* pszTmp = m_psz + kstrlen(m_psz);
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
		if (IsDigit(*pszEnd)) {
			cbMin = Atoi(pszEnd++);
			if (cbMin > CB_MAX_FMT_WIDTH)
				cbMin = CB_MAX_FMT_WIDTH;
			while (IsDigit(*pszEnd))
				pszEnd++;
		}

		if (*pszEnd == 'c') {
			char szBuf[2];
			szBuf[0] = (uint8_t) (va_arg (argList, int) & 0xFF);
			szBuf[1] = '\0';
			size_t cb = kstrbyte(m_psz) + sizeof(char);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, szBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'C') {
			wchar_t szwBuf[2];
			szwBuf[0] = (wchar_t) (va_arg (argList, int) & 0xFFFF);
			szwBuf[1] = '\0';
			char szBuf[4];

			size_t cb = WideCharToMultiByte(CP_ACP, 0, szwBuf, sizeof(szwBuf), szBuf, sizeof(szBuf), NULL, NULL);
			ASSERT(cb < sizeof(szBuf));
			szBuf[cb] = '\0';
			cb = kstrbyte(m_psz) + kstrbyte(szBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, szBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'd' || *pszEnd == 'i') {

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)	// TODO: [randalphwa - 08-30-2018] Need CLANG preprocessor flag
			// note that we don't have to do any special processing if not compiling 64-bit app, as size_t will be same as int
			if (bSize_t) {
				Itoa(va_arg(argList, _int64), szNumBuf, sizeof(szNumBuf) - 1);
			}
			else {
				Itoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
			}
#else	// not defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			Itoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
#endif	// defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			size_t cb = kstrbyte(m_psz) + kstrbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - kstrlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = kstrbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ASSERT(cAvail < 4096);
					}
					kstrcat(m_psz, DEST_SIZE, szTmp);
				}
			}
			kstrcat(m_psz, DEST_SIZE, szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'u') {
			Utoa(va_arg(argList, unsigned int), szNumBuf, sizeof(szNumBuf));
			size_t cb = kstrbyte(m_psz) + kstrbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - kstrlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = kstrbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ASSERT(cAvail < 4096);
					}
					kstrcat(m_psz, DEST_SIZE, szTmp);
				}
			}
			kstrcat(m_psz, DEST_SIZE, szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'x') {
			Hextoa(va_arg(argList, int), szNumBuf, false);
			size_t cb = kstrbyte(m_psz) + kstrbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - kstrlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = kstrbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ASSERT(cAvail < 4096);
					}
					kstrcat(m_psz, DEST_SIZE, szTmp);
				}
			}
			kstrcat(m_psz, DEST_SIZE, szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'X') {
			ASSERT_MSG(!bSize_t, "zX and IX not supported");
			Hextoa(va_arg(argList, int), szNumBuf, true);
			size_t cb = kstrbyte(m_psz) + kstrbyte(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - kstrlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = kstrbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (char*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ASSERT(cAvail < 4096);
					}
					kstrcat(m_psz, DEST_SIZE, szTmp);
				}
			}
			kstrcat(m_psz, DEST_SIZE, szNumBuf);
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
				ASSERT_MSG(psz, "NULL pointer passed to CStr::printf(\"%s");
				psz = "(missing argument for %s)";
			}
			size_t cb = kstrbyte(m_psz) + kstrbyte(psz);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, psz);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'S') {
WideChar:
			const wchar_t* pwsz = (wchar_t*) va_arg(argList, wchar_t*);
			if (!pwsz)
				pwsz = L"(null)";

			size_t cb = kstrlen(pwsz) * sizeof(wchar_t);
			ASSERT(cb < MAX_STRING);
			if (cb <= 0 || cb > MAX_STRING) // empty or invalid string
				return;

			char* psz = (char*) tt::malloc(cb + 1);
			// BUGBUG: [ralphw - 07-14-2018] Following line is _WINDOWS_ only
			cb = WideCharToMultiByte(CP_ACP, 0, pwsz, (_int32) (cb / sizeof(wchar_t)), psz, (_int32) cb, nullptr, nullptr);
			psz[cb] = '\0';

			cb = kstrbyte(m_psz) + kstrbyte(psz);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, psz);
			pszEnd++;
			tt::free(psz);
			continue;
		}
		else if (*pszEnd == '%') {
			size_t cb = kstrbyte(m_psz) + sizeof(char);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, "%");
			pszEnd++;
			continue;
		}
		else {
			// This is a potential security risk since we don't know what size of argument to retrieve from va_arg(). We simply
			// print the rest of the format string and don't pop any arguments off.

			FAIL("Invalid format string for printf");
			size_t cb;
#ifdef _DEBUG
			cb = kstrbyte(m_psz) + kstrbyte("Invalid format string: ");
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, "Invalid format string: ");
#endif // _DEBUG
			cb = kstrbyte(m_psz) + kstrbyte(pszEnd) + sizeof(char);	// make room for leading % character
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (char*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, "%");
			kstrcat(m_psz, DEST_SIZE, pszEnd);
			break;
		}
	}

	// Now readjust the allocation to the actual size

	m_psz = (char*) tt::realloc(m_psz, kstrbyte(m_psz));
}

const char* CStr::ProcessKFmt(const char* pszEnd, va_list* pargList)
{
	char szBuf[256];
	szBuf[0] = '\0';
	switch (*pszEnd) {
		case 'n':	// 'n' is deprecated, 'd' should be used instead
		case 'd':
			Itoa((int) va_arg(*pargList, int), szBuf, sizeof(szBuf));
			AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			break;

		case 'I':	// 64-bit version of 'd' and 'u' that works in 32-bit builds
			if (::IsSameSubString(pszEnd, "I64d"))
				Itoa(va_arg(*pargList, int64_t), szBuf, sizeof(szBuf));
			else if (::IsSameSubString(pszEnd, "I64u"))
				Utoa(va_arg(*pargList, uint64_t), szBuf, sizeof(szBuf));
			AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			pszEnd += 3;	// skip over I64 portion, then count normally
			break;

		case 't':	// use for size_t parameters, this will handle both 32 and 64 bit compilations
			Utoa(va_arg(*pargList, size_t), szBuf, sizeof(szBuf));
			AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			break;

		case 'u':
			Utoa(va_arg(*pargList, unsigned int), szBuf, sizeof(szBuf));
			AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
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
				CStr cszRes;
				cszRes.GetResString(va_arg(*pargList, int));
				kstrcpy(szBuf, sizeof(szBuf), cszRes);
			}
			break;


		case 'e':
			{
				char* pszMsg;

				FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, va_arg(*pargList, int), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(char*) &pszMsg, 0, NULL);
				kstrcpy(szBuf, sizeof(szBuf), pszMsg);
				LocalFree((HLOCAL) pszMsg);
			}
			break;
#endif // _WINDOWS_

		case 'q':
			try {
				const char* psz = va_arg(*pargList, const char*);
				size_t cb = kstrbyte(m_psz) + kstrbyte(psz) + (2 * sizeof(char));	// add room for both quotes
				if (cb > tt::size(m_psz)) {
					cb >>= 7;
					cb <<= 7;
					cb += 0x80;	// round up to 128
					m_psz = (char*) tt::realloc(m_psz, cb);
				}
				kstrcat(m_psz, "\042");
				kstrcat(m_psz, psz);
				kstrcat(m_psz, "\042");
			}
			catch (...) {
				FAIL("Exception in printf -- bad %%kq pointer");
			}
			break;
	}
	if (szBuf[0]) {
		size_t cbCur = kstrbyte(m_psz);
		size_t cb = cbCur + kstrbyte(szBuf) + sizeof(char);
		if (cb > tt::size(m_psz)) {
			cb >>= 7;
			cb <<= 7;
			cb += 0x80;	// round up to 128
			m_psz = (char*) tt::realloc(m_psz, cb);
		}
		kstrcat(m_psz, DEST_SIZE - cbCur, szBuf);
	}
	return pszEnd + 1;
}

// The following functions are used by CStr, so included here rather then in a seperate module. The assumption is that
// is not likely for a project that links with ttLib to need the number functions below and not use CStr

char* Utoa(uint32_t val, char* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	char* pszRet = pszDst;
	char* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

	*pszDst-- = '\0';

	// The number was converted starting with the lowest digit first, so we need to flip it

	do {
		char temp = *pszDst;
		*pszDst = *firstdig;
		*firstdig = temp;
		--pszDst;
		++firstdig;
	} while (firstdig < pszDst);
	return pszRet;
}

char* Utoa(uint64_t val, char* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	char* pszRet = pszDst;
	char* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

	*pszDst-- = '\0';

	// The number was converted starting with the lowest digit first, so we need to flip it

	do {
		char temp = *pszDst;
		*pszDst = *firstdig;
		*firstdig = temp;
		--pszDst;
		++firstdig;
	} while (firstdig < pszDst);
	return pszRet;
}

char* Itoa(int32_t val, char* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	char* pszRet = pszDst;
	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	Utoa((uint32_t) val, pszDst, cbDst);
	return pszRet;
}

char* Itoa(int64_t val, char* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	char* pszRet = pszDst;
	Utoa((uint64_t) val, pszDst, cbDst);
	return pszRet;
}

// We allow for pszNum and pszDst to be different in case the pszNum buffer is only large
// enough to hold the number and not the commas

void AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst)
{
	if (pszDst != pszNum)
		kstrcpy(pszDst, cbDst, pszNum);	// copy the number, performa all additional work in-place in the destination buffer

	ptrdiff_t cbNum = kstrlen(pszDst);	// needs to be signed because it can go negative
	if (cbNum < 4) {
		ASSERT(cbNum < (ptrdiff_t) cbDst);
		return;
	}
	ASSERT(cbNum + (cbNum / 3) < (ptrdiff_t) cbDst);
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
		memmove(pszDst + cbStart + 1, pszDst + cbStart, kstrbyte(pszDst + cbStart));	// make space for a comma
		pszDst[cbStart] = ',';
		++cbNum;		// track that we added a comma for loop comparison
		cbStart += 4;	// 3 numbers plus the comma
	}
}

char* Hextoa(size_t val, char* pszDst, bool bUpperCase)
{
	static char* szBuf = NULL;
	if (!pszDst) {
		if (!szBuf) {
			szBuf = (char*) tt::malloc(sizeof(size_t) * sizeof(char) + sizeof(char) * 4);	// extra room for null termination and general paranoia
		}
		pszDst = szBuf;
	}
	char* pszRet = pszDst;
	char* psz = pszDst;
	do {
		size_t digval = (val % 16);
		val /= 16;		// get next digit

		if (digval > 9)
			*psz++ = (char) (digval - 10 + (bUpperCase ? 'A' : 'a'));
		else
			*psz++ = (char) (digval + '0');
	} while (val > 0);

	*psz-- = '\0';	// terminate string; p points to last digit

	do {
		char temp = *psz;
		*psz = *pszDst;
		*pszDst = temp;
		--psz;
		++pszDst;
	} while (pszDst < psz); // repeat until halfway
	return pszRet;
}

ptrdiff_t Atoi(const char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return 0;

	// Skip over leading white space

	while (*psz == ' ' || *psz == '\t')
		psz++;
	if (!*psz)
		return 0;

	ptrdiff_t total = 0;

	if (psz[0] == '0' && (psz[1] == 'x' || psz[1] =='X')) {
		psz += 2;	// skip over 0x prefix in hexadecimal strings

		for(;;) {
			ptrdiff_t c = (ptrdiff_t) *psz++;
			if (c >= '0' && c <= '9')
				total = 16 * total + (c - '0');
			else if (c >= 'a' && c <= 'f')
				total = total * 16 + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				total = total * 16 + c - 'A' + 10;
			else
				return total;
		}
	}

	ptrdiff_t c = (ptrdiff_t) *psz++;
	ptrdiff_t sign = c;
	if (c == '-' || c == '+')
		c = (ptrdiff_t) *psz++;

	while (c >= '0' && c <= '9') {
		total = 10 * total + (c - '0');
		c = (ptrdiff_t) *psz++;
	}

	if (sign == '-')
		return -total;
	else
		return total;
}

