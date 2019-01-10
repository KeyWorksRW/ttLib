/////////////////////////////////////////////////////////////////////////////
// Name:		CWStr
// Purpose:		SBCS string class
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include <direct.h>		// for _getcwd

#include "../include/cwstr.h"	// CWStr
#include "../include/cstr.h"	// CStr
#include "../include/kstr.h"	// various kstr() functions

#ifdef _WINDOWS_
	extern HINSTANCE	_hinstResources;
#endif

#ifndef _MAX_U64TOSTR_BASE10_COUNT
	#define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

// Note that the limit here of 64k is smaller then the kstr functions that use 16m (_KSTRMAX)

#define MAX_STRING (64 * 1024)	// Use this to limit the length of a single string as a security precaution
#define	DEST_SIZE (tt::size(m_psz) - sizeof(wchar_t))

void CWStr::AppendFileName(const wchar_t* pszFile)
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

void CWStr::ChangeExtension(const wchar_t* pszExtension)
{
	ASSERT_MSG(pszExtension, "NULL pointer!");
	ASSERT_MSG(*pszExtension, "Empty string!");

	if (!pszExtension || !*pszExtension)
		return;

	if (!m_psz)
		m_psz = tt::strdup(L"");

	wchar_t* pszEnd = kstrchrR(m_psz, '.');
	if (pszEnd && pszEnd[1] != '/' && pszEnd[1] != '\\')	// handle "./foo" -- don't assume the leading period is an extension if there's a folder seperator after it
		*pszEnd = 0;

	if (*pszExtension != '.')
		*this += L".";
	*this += pszExtension;
}

void CWStr::RemoveExtension()
{
	 if (m_psz) {
		wchar_t* psz = kstrchrR(m_psz, '.');
		if (psz) {
			if (psz == m_psz || *(psz - 1) == '.' || psz[1] == CH_BACKSLASH || psz[1] == CH_FORWARDSLASH)	// ignore .file, ./file, and ../file
				return;
			*psz = 0;
		}
	}
}

void CWStr::AddTrailingSlash()
{
	if (!m_psz) {
		m_psz = tt::strdup(L"/");
		return;
	}
	const wchar_t* pszLastSlash = FindLastSlash();
	if (!pszLastSlash || pszLastSlash[1])	// only add if there was no slash or there was something after the slash
		*this += L"/";
}

const wchar_t* CWStr::FindLastSlash()
{
	ASSERT_MSG(m_psz, "NULL pointer!");
	ASSERT_MSG(*m_psz, "Empty string!");

	if (!m_psz || !*m_psz)
		return nullptr;

	const wchar_t* pszLastBackSlash = kstrchrR(m_psz, CH_BACKSLASH);
	const wchar_t* pszLastFwdSlash	= kstrchrR(m_psz, CH_FORWARDSLASH);
	if (!pszLastBackSlash)
		return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
	else if (!pszLastFwdSlash)
		return pszLastBackSlash ? pszLastBackSlash : nullptr;
	else
		return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash : pszLastBackSlash;		// Impossible for them to be equal
}

void CWStr::GetCWD()
{
	char szCWD[1024];
	_getcwd(szCWD, sizeof(szCWD));
	CopyNarrow(szCWD);
}

#ifdef _WINDOWS_

void CWStr::GetFullPathName()
{
	ASSERT(m_psz);
	wchar_t szPath[MAX_PATH];
	::GetFullPathNameW(m_psz, sizeof(szPath), szPath, NULL);
	tt::free(m_psz);
	m_psz = tt::strdup(szPath);
}

const wchar_t* CWStr::GetListBoxText(HWND hwnd, size_t sel)
{
	if (m_psz)
		tt::free(m_psz);
	if (sel == (size_t) LB_ERR)
		m_psz = tt::strdup(L"");
	else {
		size_t cb = ::SendMessage(hwnd, LB_GETTEXTLEN, sel, 0);
		ASSERT(cb != (size_t) LB_ERR);
		if (cb != (size_t) LB_ERR) {
			m_psz = (wchar_t*) tt::malloc(cb + 1);
			::SendMessageA(hwnd, LB_GETTEXT, sel, (LPARAM) m_psz);
		}
		else {
			m_psz = tt::strdup(L"");
		}
	}
	return m_psz;
}

/*
	_hinstResources is typically set by InitCaller() and determines where to load resources from. If you need to load the resources
	from a DLL, then first call:

		_hinstResources = LoadLibrary("dll name");
*/

const wchar_t* CWStr::GetResString(size_t idString)
{
	static wchar_t szStringBuf[1024];

	if (LoadStringW(_hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0) {
		CStr cszMsg;
		cszMsg.printf("Invalid string id: %zu", idString);
		FAIL(cszMsg);
		if (m_psz)
			tt::free(m_psz);
		m_psz = tt::strdup(L"");
	}
	else {
		if (m_psz)
			tt::free(m_psz);
		m_psz = tt::strdup(szStringBuf);
	}
	return m_psz;
}

bool CWStr::GetWindowText(HWND hwnd)
{
	if (m_psz) {
		 tt::free(m_psz);
		 m_psz = nullptr;
	}

	ASSERT_MSG(IsValidWindow(hwnd), "Invalid window handle");
	if (!IsValidWindow(hwnd)) {
		m_psz = tt::strdup(L"");
		return false;
	}

	int cb = GetWindowTextLength(hwnd);
	ASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	if (cb == 0 || cb > MAX_STRING) {
		m_psz = tt::strdup(L"");
		return false;
	}

	wchar_t* psz = (wchar_t*) tt::malloc(cb + sizeof(wchar_t));
	cb = ::GetWindowTextW(hwnd, psz, cb);
	if (cb == 0) {
		m_psz = tt::strdup(L"");
		tt::free(psz);
		return false;
	}
	else
		m_psz = psz;
	return true;
}

#endif	// _WINDOWS_

wchar_t* CWStr::GetQuotedString(wchar_t* pszQuote)
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
		m_psz = (wchar_t*) tt::malloc(cb);		// this won't return if it fails, so you will never get a nullptr on return
		*m_psz = 0;
	}

	if (*pszQuote == CH_QUOTE) {
		pszQuote++;
		wchar_t* pszStart = pszQuote;
		while (*pszQuote != CH_QUOTE && *pszQuote) {
			++pszQuote;
		}
		wcsncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else if (*pszQuote == CH_START_QUOTE || *pszQuote == CH_END_QUOTE) {
		pszQuote++;
		wchar_t* pszStart = pszQuote;
		while (*pszQuote != CH_END_QUOTE && *pszQuote) {
			++pszQuote;
		}
		wcsncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
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
			m_psz = (wchar_t*) tt::realloc(m_psz, cbNew);
	}
	return (*pszQuote ? pszQuote + 1 : pszQuote);
}

bool CWStr::IsSameSubString(const wchar_t* pszSub)
{
	if (!m_psz || !pszSub)
		return false;
	return ::IsSameSubString(m_psz, pszSub);
}

void CWStr::MakeLower()
{
	if (m_psz && *m_psz) {
		wchar_t* psz = m_psz;
		while (*psz) {
			*psz = (char) tolower(*psz);
			++psz;
		}
	}
}

void CWStr::MakeUpper()
{
	if (m_psz && *m_psz) {
		wchar_t* psz = m_psz;
		while (*psz) {
			*psz = (char) toupper(*psz);
			++psz;
		}
	}
}

bool CWStr::CopyNarrow(const char* psz)	// convert UTF8 to UNICODE and store it
{
	if (m_psz)
		 tt::free(m_psz);

	ASSERT_MSG(psz, "NULL pointer!");
	ASSERT_MSG(*psz, "empty string!");

	if (!psz || !*psz) {
		m_psz = tt::strdup(L"");
		return false;
	}

	size_t cch = kstrlen(psz);
	ASSERT_MSG(cch <= MAX_STRING, "String is over 64k in size!");

	// BUGBUG: [randalphwa - 09-09-2018]	MultiByteToWideChar() only works on Windows -- need a portable way to convert

	int cbNew = MultiByteToWideChar(CP_UTF8, 0, psz, (int) cch, nullptr, 0);
	if (cbNew) {
		m_psz = (wchar_t*) tt::malloc(cbNew * sizeof(wchar_t) + sizeof(wchar_t));
		cch = MultiByteToWideChar(CP_UTF8, 0, psz, (int) cch, m_psz, cbNew);
		if (cch == 0)
			tt::free(m_psz);
		else
			m_psz[cch] = 0;
	}
	if (cbNew == 0 || cch == 0) {
		m_psz = tt::strdup(L"");
		return false;
	}

	return true;
}

wchar_t* CWStr::Enlarge(size_t cbTotalSize)
{
	ASSERT(cbTotalSize <= MAX_STRING);
	if (cbTotalSize > MAX_STRING)
		cbTotalSize = MAX_STRING;

	size_t curSize = m_psz ? tt::size(m_psz) : 0;
	if (cbTotalSize <= curSize)
		return m_psz;

	if (m_psz)
		m_psz = (wchar_t*) tt::realloc(m_psz, cbTotalSize);
	else
		m_psz = (wchar_t*) tt::malloc(cbTotalSize);
	return m_psz;
}

void CWStr::operator=(const char* psz)
{
	ASSERT_MSG(psz, "null pointer!");
	ASSERT_MSG(*psz, "empty string!");

	CopyNarrow(psz);
}

void CWStr::operator=(const wchar_t* psz)
{
	ASSERT_MSG(psz, "null pointer!");
	ASSERT_MSG(*psz, "empty string!");
	ASSERT_MSG(m_psz != psz, "Attempt to assign CStr to itself");

	if (m_psz && m_psz == psz)
		return;

	if (m_psz)
		tt::free(m_psz);

	m_psz = tt::strdup(psz ? psz : L"");
}

void CWStr::operator+=(const wchar_t* psz)
{
	ASSERT_MSG(m_psz != psz, "Attempt to append string to itself!");
	if (m_psz && m_psz == psz)
		return;
	if (!m_psz)
		m_psz = tt::strdup(psz && *psz ? psz : L"");
	else if (!psz || !*psz)
		m_psz = tt::strdup(L"");
	else {
		size_t cbNew = kstrbyte(psz);
		size_t cbOld = kstrbyte(m_psz);
		ASSERT_MSG(cbNew + cbOld <= MAX_STRING, "String is over 64k in size!");
		if (cbNew + cbOld > MAX_STRING)
			return;		// ignore it if it's too large
		m_psz = (wchar_t*) tt::realloc(m_psz, cbNew + cbOld);
		memcpy(m_psz + ((cbOld - sizeof(wchar_t)) / sizeof(wchar_t)), psz, cbNew);
	}
}

void CWStr::operator+=(wchar_t ch)
{
	wchar_t szTmp[2];
	szTmp[0] = ch;
	szTmp[1] = 0;
	if (!m_psz)
		m_psz = tt::strdup(szTmp);
	else {
		m_psz = (wchar_t*) tt::realloc(m_psz, kstrbyte(m_psz));
		kstrcat(m_psz, DEST_SIZE, szTmp);
	}
}

void CWStr::operator+=(ptrdiff_t val)
{
	wchar_t szNumBuf[_MAX_U64TOSTR_BASE10_COUNT];
	Itoa(val, szNumBuf, sizeof(szNumBuf));
	*this += szNumBuf;
}

void CWStr::operator+=(CWStr csz)
{
	if (csz.IsNonEmpty())
		*this += csz;
}

wchar_t CWStr::operator[](int pos)
{
	if (!m_psz || pos > (int) kstrlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

void __cdecl CWStr::printf(const wchar_t* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	vprintf(pszFormat, argList);
	va_end(argList);
}

// Because we are using _alloca, we limit string size to 64k

#define CB_MAX_FMT_WIDTH 20		// Largest formatted width we allow

void CWStr::vprintf(const wchar_t* pszFormat, va_list argList)
{
	ASSERT_MSG(pszFormat, "NULL pointer!");
	ASSERT_MSG(*pszFormat, "Empty format string!");
	if (!pszFormat || !*pszFormat)
		return;

	size_t cAvail;

	if (m_psz) {
		cAvail = kstrbyte(m_psz);
		cAvail >>=8;	// remove lower bits
		cAvail <<=8;
		cAvail += 0x100;	// allocate 256 byte blocks at a time
		m_psz = (wchar_t*) tt::realloc(m_psz, cAvail);
		cAvail--;	// don't include null-terminator
	}
	else {
		cAvail = 255;
		m_psz = (wchar_t*) tt::malloc(cAvail + 1);
		*m_psz = 0;
	}

	const wchar_t* pszEnd = pszFormat;
	char szNumBuf[50];	// buffer used for converting numbers including plenty of room for commas

	while (*pszEnd) {
		if (*pszEnd != '%') {
			const wchar_t* pszBegin = pszEnd++;
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
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - sizeof(wchar_t);
				ASSERT(cAvail < 4096);
			}

			wchar_t* pszTmp = m_psz + kstrbyte(m_psz);
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
			size_t cb = kstrbyte(m_psz);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			CWStr cwsz(szBuf);
			kstrcat(m_psz, DEST_SIZE, cwsz);
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
			cb = kstrbyte(m_psz) + kstrbyte(szwBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, szwBuf);
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
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
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
						m_psz = (wchar_t*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ASSERT(cAvail < 4096);
					}
					CWStr cwsz(szTmp);
					kstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			CWStr cwszNum(szNumBuf);
			kstrcat(m_psz, DEST_SIZE, cwszNum);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'u') {
			Utoa(va_arg(argList, unsigned int), szNumBuf, sizeof(szNumBuf));
			size_t cb = kstrlen(m_psz) * sizeof(wchar_t) + kstrlen(szNumBuf) * sizeof(wchar_t) + sizeof(wchar_t);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - kstrlen(szNumBuf) * sizeof(wchar_t);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = kstrbyte(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (wchar_t*) tt::realloc(m_psz, cb);
						cAvail = cb - sizeof(wchar_t);
						ASSERT(cAvail < 4096);
					}
					CWStr cwsz(szTmp);
					kstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			CWStr cwszNum(szNumBuf);
			kstrcat(m_psz, DEST_SIZE, cwszNum);
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
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - kstrlen(szNumBuf) * sizeof(wchar_t);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = kstrlen(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (wchar_t*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ASSERT(cAvail < 4096);
					}
					CWStr cwsz(szTmp);
					kstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			CWStr cwszNum(szNumBuf);
			kstrcat(m_psz, DEST_SIZE, cwszNum);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'X') {
			ASSERT_MSG(!bSize_t, "zX and IX not supported");
			Hextoa(va_arg(argList, int), szNumBuf, true);
			size_t cb = kstrlen(m_psz) + kstrlen(szNumBuf) + 1;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
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
					cb = kstrlen(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (wchar_t*) tt::realloc(m_psz, cb);
						cAvail = cb - 1;
						ASSERT(cAvail < 4096);
					}
					CWStr cwsz(szTmp);
					kstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			CWStr cwszNum(szNumBuf);
			kstrcat(m_psz, DEST_SIZE, cwszNum);
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
					)
				psz = "(missing argument for %s)";

			size_t cb = kstrlen(m_psz) + kstrlen(psz) + 1;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			CWStr cwsz(psz);
			kstrcat(m_psz, DEST_SIZE, cwsz);
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

			cb = kstrlen(m_psz) + kstrlen(pwsz) + 1;	// we use kstrlen(psz) in case it is a DBCS character (which could be 2 bytes)
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, pwsz);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == '%') {
			size_t cb = kstrlen(m_psz) + 2;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, L"%");
			pszEnd++;
			continue;
		}
		else {
			// This is a potential security risk since we don't know what size of argument to retrieve from va_arg(). We simply
			// print the rest of the format string and don't pop any arguments off.

			FAIL("Invalid format string for printf");
			size_t cb;
#ifdef _DEBUG
			cb = kstrlen(m_psz) + kstrlen(L"Invalid format string: ") + 1;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, L"Invalid format string: ");
#endif // _DEBUG
			cb = kstrlen(m_psz) + kstrlen(pszEnd) + 2;	// make room for leading % character
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				cAvail = cb - 1;
				ASSERT(cAvail < 4096);
			}
			kstrcat(m_psz, DEST_SIZE, L"%");
			kstrcat(m_psz, DEST_SIZE, pszEnd);
			break;
		}
	}

	// Now readjust the allocation to the actual size

	m_psz = (wchar_t*) tt::realloc(m_psz, kstrbyte(m_psz));
}

const wchar_t* CWStr::ProcessKFmt(const wchar_t* pszEnd, va_list* pargList)
{
	wchar_t szwBuf[256];
	szwBuf[0] = L'\0';
	switch (*pszEnd) {
		case 'n':	// 'n' is deprecated, 'd' should be used instead
		case 'd':
			Itoa((int) va_arg(*pargList, int), szwBuf, sizeof(szwBuf));
			AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
			break;

		case 'I':	// 64-bit version of 'd' and 'u' that works in 32-bit builds
			if (::IsSameSubString(pszEnd, L"I64d"))
				Itoa(va_arg(*pargList, int64_t), szwBuf, sizeof(szwBuf));
			else if (::IsSameSubString(pszEnd, L"I64u"))
				Utoa(va_arg(*pargList, uint64_t), szwBuf, sizeof(szwBuf));
			AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
			pszEnd += 3;	// skip over I64 portion, then count normally
			break;

		case 't':	// use for size_t parameters, this will handle both 32 and 64 bit compilations
			Utoa(va_arg(*pargList, size_t), szwBuf, sizeof(szwBuf));
			AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
			break;

		case 'u':
			Utoa(va_arg(*pargList, unsigned int), szwBuf, sizeof(szwBuf));
			AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
			break;

		case 's':
			if (va_arg(*pargList, int) != 1) {
				szwBuf[0] = 's';
				szwBuf[1] = '\0';
			}
			break;

		case 'S':
			if (va_arg(*pargList, _int64) != 1) {
				szwBuf[0] = 's';
				szwBuf[1] = '\0';
			}
			break;

#ifdef _WINDOWS_
		case 'r':
			{
				CWStr cszRes;
				cszRes.GetResString(va_arg(*pargList, int));
				kstrcpy(szwBuf, sizeof(szwBuf), cszRes);
			}
			break;


		case 'e':
			{
				wchar_t* pszMsg;

				FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, va_arg(*pargList, int), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(wchar_t*) &pszMsg, 0, NULL);
				kstrcpy(szwBuf, sizeof(szwBuf), pszMsg);
				LocalFree((HLOCAL) pszMsg);
			}
			break;
#endif // _WINDOWS_

		case 'q':
			try {
				const wchar_t* psz = va_arg(*pargList, const wchar_t*);
				size_t cb = kstrlen(m_psz) + kstrlen(psz) + (3 * sizeof(wchar_t));
				if (cb > tt::size(m_psz)) {
					cb >>= 7;
					cb <<= 7;
					cb += 0x80;	// round up to 128
					m_psz = (wchar_t*) tt::realloc(m_psz, cb);
				}
				kstrcat(m_psz, L"\042");
				kstrcat(m_psz, psz);
				kstrcat(m_psz, L"\042");
			}
			catch (...) {
				FAIL("Exception in printf -- bad %%kq pointer");
			}
			break;
	}
	if (szwBuf[0]) {
		size_t cbCur = kstrbyte(m_psz);
		size_t cb = cbCur + kstrbyte(szwBuf);
		if (cb > tt::size(m_psz)) {
			cb >>= 7;
			cb <<= 7;
			cb += 0x80;	// round up to 128
			m_psz = (wchar_t*) tt::realloc(m_psz, cb);
		}
		kstrcat(m_psz, DEST_SIZE - cbCur, szwBuf);
	}
	return pszEnd + 1;
}

// The following functions are used by CWStr, so included here rather then in a seperate module. The assumption is that
// is not likely for a project that links with ttLib to need the number functions below and not use CWStr

wchar_t* Utoa(uint32_t val, wchar_t* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	wchar_t* pszRet = pszDst;
	wchar_t* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

	*pszDst-- = '\0';

	// The number was converted starting with the lowest digit first, so we need to flip it

	do {
		wchar_t temp = *pszDst;
		*pszDst = *firstdig;
		*firstdig = temp;
		--pszDst;
		++firstdig;
	} while (firstdig < pszDst);
	return pszRet;
}

wchar_t* Utoa(uint64_t val, wchar_t* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	wchar_t* pszRet = pszDst;
	wchar_t* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

	*pszDst-- = '\0';

	// The number was converted starting with the lowest digit first, so we need to flip it

	do {
		wchar_t temp = *pszDst;
		*pszDst = *firstdig;
		*firstdig = temp;
		--pszDst;
		++firstdig;
	} while (firstdig < pszDst);
	return pszRet;
}

wchar_t* Itoa(int32_t val, wchar_t* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	wchar_t* pszRet = pszDst;
	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	Utoa((uint32_t) val, pszDst, cbDst);
	return pszRet;
}

wchar_t* Itoa(int64_t val, wchar_t* pszDst, size_t cbDst)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	wchar_t* pszRet = pszDst;
	Utoa((uint64_t) val, pszDst, cbDst);
	return pszRet;
}

// We allow for pszNum and pszDst to be different in case the pszNum buffer is only large
// enough to hold the number and not the commas

void AddCommasToNumber(wchar_t* pszNum, wchar_t* pszDst, size_t cbDst)
{
	size_t cchDst = cbDst / sizeof(wchar_t);
	if (pszDst != pszNum)
		kstrcpy(pszDst, cbDst, pszNum);	// copy the number, performa all additional work in-place in the destination buffer

	size_t cchNum = kstrlen(pszDst);	// needs to be signed because it can go negative
	if (cchNum < 4) {
		ASSERT(cchNum * sizeof(wchar_t) < cbDst);
		return;
	}
	ASSERT(cchNum + (cchNum / 3) < cchDst);
	if (cchNum + (cchNum / 3) >= cchDst)
		return;

	if (*pszDst == '-')	{
		--cchNum;
		if (cchNum < 4)
			return;
	}

	size_t cchStart = cchNum % 3;
	if (cchStart == 0)
		cchStart += 3;
	while (cchStart < cchNum) {
		memmove(pszDst + cchStart + 1, pszDst + cchStart, kstrbyte(pszDst + cchStart) + sizeof(wchar_t));	// make space for a comma
		pszDst[cchStart] = ',';
		++cchNum;		// track that we added a comma for loop comparison
		cchStart += 4;	// 3 numbers plus the comma
	}
}

wchar_t* Hextoa(size_t val, wchar_t* pszDst, bool bUpperCase)
{
	static wchar_t* szBuf = NULL;
	if (!pszDst) {
		if (!szBuf) {
			szBuf = (wchar_t*) tt::malloc(sizeof(size_t) * sizeof(wchar_t) + sizeof(wchar_t) * 4);	// extra room for null termination and general paranoia
		}
		pszDst = szBuf;
	}
	wchar_t* pszRet = pszDst;
	wchar_t* psz = pszDst;
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
		wchar_t temp = *psz;
		*psz = *pszDst;
		*pszDst = temp;
		--psz;
		++pszDst;
	} while (pszDst < psz); // repeat until halfway
	return pszRet;
}

ptrdiff_t Atoi(const wchar_t* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return 0;

	// Skip over leading white space

	while (*psz == L' ' || *psz == L'\t')
		psz++;
	if (!*psz)
		return 0;

	ptrdiff_t total = 0;

	if (psz[0] == L'0' && (psz[1] == L'x' || psz[1] ==L'X')) {
		psz += 2;	// skip over 0x prefix in hexadecimal strings

		for(;;) {
			ptrdiff_t c = (ptrdiff_t) *psz++;
			if (c >= L'0' && c <= L'9')
				total = 16 * total + (c - L'0');
			else if (c >= L'a' && c <= L'f')
				total = total * 16 + c - L'a' + 10;
			else if (c >= L'A' && c <= L'F')
				total = total * 16 + c - L'A' + 10;
			else
				return total;
		}
	}

	ptrdiff_t c = (ptrdiff_t) *psz++;
	ptrdiff_t sign = c;
	if (c == L'-' || c == L'+')
		c = (ptrdiff_t) *psz++;

	while (c >= L'0' && c <= L'9') {
		total = 10 * total + (c - L'0');
		c = (ptrdiff_t) *psz++;
	}

	if (sign == L'-')
		return -total;
	else
		return total;
}
