/////////////////////////////////////////////////////////////////////////////
// Name:		ttCWStr
// Purpose:		SBCS string class
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <direct.h>		// for _getcwd

#include "../include/ttdebug.h" 	// for ttASSERTS
#include "../include/ttwstr.h"
#include "../include/ttstr.h"		// ttCStr

#ifndef _MAX_U64TOSTR_BASE10_COUNT
	#define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

namespace ttpriv {
	void AddCommasToNumber(wchar_t* pszNum, wchar_t* pszDst, size_t cbDst);
}

// Note that the limit here of 64k is smaller then the ttstr functions that use 16m

#define MAX_STRING (64 * 1024)	// Use this to limit the length of a single string as a security precaution
#define	DEST_SIZE (ttsize(m_psz) - sizeof(wchar_t))

void ttCWStr::AppendFileName(const wchar_t* pszFile)
{
	ttASSERT_NONEMPTY(pszFile);

	if (!pszFile || !*pszFile)
		return;

	if (!m_psz)	{	// no folder or drive to append to, so leave as is without adding slash
		m_psz = ttstrdup(pszFile);		// REVIEW: [ralphw - 06-03-2018] We could prefix this with ".\"
		return;
	}

	AddTrailingSlash();
	*this += pszFile;
}

void ttCWStr::ChangeExtension(const wchar_t* pszExtension)
{
	ttASSERT_NONEMPTY(pszExtension);

	if (!pszExtension || !*pszExtension)
		return;

	if (!m_psz)
		m_psz = ttstrdup(L"");

	wchar_t* pszEnd = tt::FindLastChar(m_psz, '.');
	if (pszEnd && pszEnd[1] != '/' && pszEnd[1] != '\\')	// handle "./foo" -- don't assume the leading period is an extension if there's a folder seperator after it
		*pszEnd = 0;

	if (*pszExtension != '.')
		*this += L".";
	*this += pszExtension;
}

void ttCWStr::RemoveExtension()
{
	 if (m_psz) {
		wchar_t* psz = tt::FindLastChar(m_psz, '.');
		if (psz) {
			if (psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/')	// ignore .file, ./file, and ../file
				return;
			*psz = 0;
		}
	}
}

void ttCWStr::AddTrailingSlash()
{
	if (!m_psz) {
		m_psz = ttstrdup(L"/");
		return;
	}
	const wchar_t* pszLastSlash = FindLastSlash();
	if (!pszLastSlash || pszLastSlash[1])	// only add if there was no slash or there was something after the slash
		*this += L"/";
}

const wchar_t* ttCWStr::FindLastSlash()
{
	ttASSERT_NONEMPTY(m_psz);

	if (!m_psz || !*m_psz)
		return nullptr;

	const wchar_t* pszLastBackSlash = tt::FindLastChar(m_psz, '\\');
	const wchar_t* pszLastFwdSlash	= tt::FindLastChar(m_psz, '/');
	if (!pszLastBackSlash)
		return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
	else if (!pszLastFwdSlash)
		return pszLastBackSlash ? pszLastBackSlash : nullptr;
	else
		return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash : pszLastBackSlash;		// Impossible for them to be equal
}

void ttCWStr::GetCWD()
{
	char szCWD[1024];
	_getcwd(szCWD, sizeof(szCWD));
	CopyNarrow(szCWD);
}

#ifdef _WINDOWS_

void ttCWStr::GetFullPathName()
{
	ttASSERT(m_psz);
	wchar_t szPath[MAX_PATH];
	::GetFullPathNameW(m_psz, sizeof(szPath), szPath, NULL);
	ttfree(m_psz);
	m_psz = ttstrdup(szPath);
}

const wchar_t* ttCWStr::GetListBoxText(HWND hwnd, size_t sel)
{
	if (m_psz)
		ttfree(m_psz);
	if (sel == (size_t) LB_ERR)
		m_psz = ttstrdup(L"");
	else {
		size_t cb = ::SendMessage(hwnd, LB_GETTEXTLEN, sel, 0);
		ttASSERT(cb != (size_t) LB_ERR);
		if (cb != (size_t) LB_ERR) {
			m_psz = (wchar_t*) ttmalloc(cb + 1);
			::SendMessageA(hwnd, LB_GETTEXT, sel, (LPARAM) m_psz);
		}
		else {
			m_psz = ttstrdup(L"");
		}
	}
	return m_psz;
}

/*
	tt::hinstResources is typically set by InitCaller() and determines where to load resources from. If you need to load the resources
	from a DLL, then first call:

		tt::hinstResources = LoadLibrary("dll name");
*/

const wchar_t* ttCWStr::GetResString(size_t idString)
{
	static wchar_t szStringBuf[1024];

	if (LoadStringW(tt::hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0) {
		ttCStr cszMsg;
		cszMsg.printf("Invalid string id: %zu", idString);
		ttFAIL(cszMsg);
		if (m_psz)
			ttfree(m_psz);
		m_psz = ttstrdup(L"");
	}
	else {
		if (m_psz)
			ttfree(m_psz);
		m_psz = ttstrdup(szStringBuf);
	}
	return m_psz;
}

bool ttCWStr::GetWindowText(HWND hwnd)
{
	if (m_psz) {
		 ttfree(m_psz);
		 m_psz = nullptr;
	}

	ttASSERT_MSG(hwnd && IsWindow(hwnd), "Invalid window handle");
	if (!hwnd || !IsWindow(hwnd)) {
		m_psz = ttstrdup(L"");
		return false;
	}

	int cb = GetWindowTextLength(hwnd);
	ttASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	if (cb == 0 || cb > MAX_STRING) {
		m_psz = ttstrdup(L"");
		return false;
	}

	wchar_t* psz = (wchar_t*) ttmalloc(cb + sizeof(wchar_t));
	cb = ::GetWindowTextW(hwnd, psz, cb);
	if (cb == 0) {
		m_psz = ttstrdup(L"");
		ttfree(psz);
		return false;
	}
	else
		m_psz = psz;
	return true;
}

#endif	// _WINDOWS_

wchar_t* ttCWStr::GetQuotedString(wchar_t* pszQuote)
{
	ttASSERT_NONEMPTY(pszQuote);

	if (!pszQuote || !*pszQuote) {
		Delete();	// current string, if any, should be deleted no matter what
		return nullptr;
	}

	size_t cb = tt::StrByteLen(pszQuote);
	ttASSERT_MSG(cb <= MAX_STRING, "String is over 64k in size!");

	if (m_psz)
		ttfree(m_psz);

	if (cb == 0 || cb > MAX_STRING) {
		m_psz = nullptr;	// it was already FreeAllocd above
		return nullptr;
	}
	else {
		m_psz = (wchar_t*) ttmalloc(cb);		// this won't return if it fails, so you will never get a nullptr on return
		*m_psz = 0;
	}

	if (*pszQuote == '"') {
		pszQuote++;
		wchar_t* pszStart = pszQuote;
		while (*pszQuote != '"' && *pszQuote) {
			++pszQuote;
		}
		wcsncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else if (*pszQuote == '`' || *pszQuote == '\'') {
		pszQuote++;
		wchar_t* pszStart = pszQuote;
		while (*pszQuote != '\'' && *pszQuote) {
			++pszQuote;
		}
		wcsncpy_s(m_psz, DEST_SIZE, pszStart, pszQuote - pszStart);
		m_psz[pszQuote - pszStart] = 0;
	}
	else {
		ttstrcpy(m_psz, DEST_SIZE, pszQuote);
		pszQuote += cb;
	}

	// If there is a significant size difference, then ReAllocate the memory

	if (cb > 32) {	// don't bother if total allocation is 32 bytes or less
		size_t cbNew = tt::StrByteLen(m_psz);
		if (cbNew < cb - 32)
			m_psz = (wchar_t*) ttrealloc(m_psz, cbNew);
	}
	return (*pszQuote ? pszQuote + 1 : pszQuote);
}

bool ttCWStr::IsSameSubString(const wchar_t* pszSub)
{
	if (!m_psz || !pszSub)
		return false;
	return tt::IsSameSubStrI(m_psz, pszSub);
}

void ttCWStr::MakeLower()
{
	if (m_psz && *m_psz) {
		wchar_t* psz = m_psz;
		while (*psz) {
			*psz = (char) towlower(*psz);
			++psz;
		}
	}
}

void ttCWStr::MakeUpper()
{
	if (m_psz && *m_psz) {
		wchar_t* psz = m_psz;
		while (*psz) {
			*psz = (char) towupper(*psz);
			++psz;
		}
	}
}

bool ttCWStr::CopyNarrow(const char* psz)	// convert UTF8 to UNICODE and store it
{
	if (m_psz)
		 ttfree(m_psz);

	ttASSERT_NONEMPTY(psz);

	if (!psz || !*psz) {
		m_psz = ttstrdup(L"");
		return false;
	}

	size_t cch = ttstrlen(psz);
	ttASSERT_MSG(cch <= MAX_STRING, "String is over 64k in size!");

	// BUGBUG: [randalphwa - 09-09-2018]	MultiByteToWideChar() only works on Windows -- need a portable way to convert

	int cbNew = MultiByteToWideChar(CP_UTF8, 0, psz, (int) cch, nullptr, 0);
	if (cbNew) {
		m_psz = (wchar_t*) ttmalloc(cbNew * sizeof(wchar_t) + sizeof(wchar_t));
		cch = MultiByteToWideChar(CP_UTF8, 0, psz, (int) cch, m_psz, cbNew);
		if (cch == 0)
			ttfree(m_psz);
		else
			m_psz[cch] = 0;
	}
	if (cbNew == 0 || cch == 0) {
		m_psz = ttstrdup(L"");
		return false;
	}

	return true;
}

wchar_t* ttCWStr::Enlarge(size_t cbTotalSize)
{
	ttASSERT(cbTotalSize <= MAX_STRING);
	if (cbTotalSize > MAX_STRING)
		cbTotalSize = MAX_STRING;

	size_t curSize = m_psz ? ttsize(m_psz) : 0;
	if (cbTotalSize <= curSize)
		return m_psz;

	if (m_psz)
		m_psz = (wchar_t*) ttrealloc(m_psz, cbTotalSize);
	else
		m_psz = (wchar_t*) ttmalloc(cbTotalSize);
	return m_psz;
}

void ttCWStr::operator=(const char* psz)
{
	ttASSERT_MSG(psz, "null pointer!");
	ttASSERT_MSG(*psz, "empty string!");

	CopyNarrow(psz);
}

void ttCWStr::operator=(const wchar_t* psz)
{
	ttASSERT_NONEMPTY(psz);
	ttASSERT_MSG(m_psz != psz, "Attempt to assign ttCWStr to itself");

	if (m_psz && m_psz == psz)
		return;

	if (m_psz)
		ttfree(m_psz);

	m_psz = ttstrdup(psz ? psz : L"");
}

void ttCWStr::operator+=(const wchar_t* psz)
{
	ttASSERT_MSG(m_psz != psz, "Attempt to append string to itself!");
	if (m_psz && m_psz == psz)
		return;
	if (!m_psz)
		m_psz = ttstrdup(psz && *psz ? psz : L"");
	else if (!psz || !*psz)
		m_psz = ttstrdup(L"");
	else {
		size_t cbNew = tt::StrByteLen(psz);
		size_t cbOld = tt::StrByteLen(m_psz);
		ttASSERT_MSG(cbNew + cbOld <= MAX_STRING, "String is over 64k in size!");
		if (cbNew + cbOld > MAX_STRING)
			return;		// ignore it if it's too large
		m_psz = (wchar_t*) ttrealloc(m_psz, cbNew + cbOld);
		memcpy(m_psz + ((cbOld - sizeof(wchar_t)) / sizeof(wchar_t)), psz, cbNew);
	}
}

void ttCWStr::operator+=(wchar_t ch)
{
	wchar_t szTmp[2];
	szTmp[0] = ch;
	szTmp[1] = 0;
	if (!m_psz)
		m_psz = ttstrdup(szTmp);
	else {
		m_psz = (wchar_t*) ttrealloc(m_psz, tt::StrByteLen(m_psz));
		ttstrcat(m_psz, DEST_SIZE, szTmp);
	}
}

void ttCWStr::operator+=(ptrdiff_t val)
{
	wchar_t szNumBuf[_MAX_U64TOSTR_BASE10_COUNT];
	tt::Itoa(val, szNumBuf, sizeof(szNumBuf));
	*this += szNumBuf;
}

void ttCWStr::operator+=(ttCWStr csz)
{
	if (csz.IsNonEmpty())
		*this += csz;
}

wchar_t ttCWStr::operator[](int pos)
{
	if (!m_psz || pos > (int) ttstrlen(m_psz))
		return 0;
	else
		return m_psz[pos];
}

void __cdecl ttCWStr::printf(const wchar_t* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
	vprintf(pszFormat, argList);
	va_end(argList);
}

// Because we are using _alloca, we limit string size to 64k

#define CB_MAX_FMT_WIDTH 20		// Largest formatted width we allow

void ttCWStr::vprintf(const wchar_t* pszFormat, va_list argList)
{
	ttASSERT_MSG(pszFormat, "NULL pointer!");
	ttASSERT_MSG(*pszFormat, "Empty format string!");
	if (!pszFormat || !*pszFormat)
		return;

	size_t cAvail;

	if (m_psz) {
		cAvail = tt::StrByteLen(m_psz);
		cAvail >>=8;	// remove lower bits
		cAvail <<=8;
		cAvail += 0x100;	// allocate 256 byte blocks at a time
		m_psz = (wchar_t*) ttrealloc(m_psz, cAvail);
		cAvail--;	// don't include null-terminator
	}
	else {
		cAvail = 255;
		m_psz = (wchar_t*) ttmalloc(cAvail + 1);
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
			cb += tt::StrByteLen(m_psz);
			ttASSERT(cb <= MAX_STRING);
			if (cb > MAX_STRING) // empty or invalid string
				return;

			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb +=  0x80;	// round up allocation size to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - sizeof(wchar_t);
				ttASSERT(cAvail < 4096);
			}

			wchar_t* pszTmp = m_psz + tt::StrByteLen(m_psz);
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
		if (tt::IsDigit(*pszEnd)) {
			cbMin = tt::Atoi(pszEnd++);
			if (cbMin > CB_MAX_FMT_WIDTH)
				cbMin = CB_MAX_FMT_WIDTH;
			while (tt::IsDigit(*pszEnd))
				pszEnd++;
		}

		if (*pszEnd == 'c') {
			char szBuf[2];
			szBuf[0] = (uint8_t) (va_arg (argList, int) & 0xFF);
			szBuf[1] = '\0';
			size_t cb = tt::StrByteLen(m_psz);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			ttCWStr cwsz(szBuf);
			ttstrcat(m_psz, DEST_SIZE, cwsz);
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
			cb = tt::StrByteLen(m_psz) + tt::StrByteLen(szwBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			ttstrcat(m_psz, DEST_SIZE, szwBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'd' || *pszEnd == 'i') {

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)	// TODO: [randalphwa - 08-30-2018] Need CLANG preprocessor flag
			// note that we don't have to do any special processing if not compiling 64-bit app, as size_t will be same as int
			if (bSize_t) {
				tt::Itoa(va_arg(argList, _int64), szNumBuf, sizeof(szNumBuf) - 1);
			}
			else {
				tt::Itoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
			}
#else	// not defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			tt::Itoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
#endif	// defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			size_t cb = tt::StrByteLen(m_psz) + tt::StrByteLen(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttstrlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = tt::StrByteLen(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (wchar_t*) ttrealloc(m_psz, cb);
						cAvail = cb - 1;
						ttASSERT(cAvail < 4096);
					}
					ttCWStr cwsz(szTmp);
					ttstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			ttCWStr cwszNum(szNumBuf);
			ttstrcat(m_psz, DEST_SIZE, cwszNum);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'u') {
			tt::Utoa(va_arg(argList, unsigned int), szNumBuf, sizeof(szNumBuf));
			size_t cb = ttstrlen(m_psz) * sizeof(wchar_t) + ttstrlen(szNumBuf) * sizeof(wchar_t) + sizeof(wchar_t);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttstrlen(szNumBuf) * sizeof(wchar_t);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = tt::StrByteLen(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (wchar_t*) ttrealloc(m_psz, cb);
						cAvail = cb - sizeof(wchar_t);
						ttASSERT(cAvail < 4096);
					}
					ttCWStr cwsz(szTmp);
					ttstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			ttCWStr cwszNum(szNumBuf);
			ttstrcat(m_psz, DEST_SIZE, cwszNum);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'x') {
			tt::Hextoa(va_arg(argList, int), szNumBuf, false);
			size_t cb = tt::StrByteLen(m_psz) + tt::StrByteLen(szNumBuf);
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttstrlen(szNumBuf) * sizeof(wchar_t);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = ttstrlen(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (wchar_t*) ttrealloc(m_psz, cb);
						cAvail = cb - 1;
						ttASSERT(cAvail < 4096);
					}
					ttCWStr cwsz(szTmp);
					ttstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			ttCWStr cwszNum(szNumBuf);
			ttstrcat(m_psz, DEST_SIZE, cwszNum);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'X') {
			ttASSERT_MSG(!bSize_t, "zX and IX not supported");
			tt::Hextoa(va_arg(argList, int), szNumBuf, true);
			size_t cb = ttstrlen(m_psz) + ttstrlen(szNumBuf) + 1;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttstrlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					cb = ttstrlen(szTmp);
					if (cb > cAvail) {
						cb >>= 7;
						cb <<= 7;
						cb += 0x80;	// round up to 128
						m_psz = (wchar_t*) ttrealloc(m_psz, cb);
						cAvail = cb - 1;
						ttASSERT(cAvail < 4096);
					}
					ttCWStr cwsz(szTmp);
					ttstrcat(m_psz, DEST_SIZE, cwsz);
				}
			}
			ttCWStr cwszNum(szNumBuf);
			ttstrcat(m_psz, DEST_SIZE, cwszNum);
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

			size_t cb = ttstrlen(m_psz) + ttstrlen(psz) + 1;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			ttCWStr cwsz(psz);
			ttstrcat(m_psz, DEST_SIZE, cwsz);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'S') {
WideChar:
			const wchar_t* pwsz = (wchar_t*) va_arg(argList, wchar_t*);
			if (!pwsz)
				pwsz = L"(null)";

			size_t cb = ttstrlen(pwsz) * sizeof(wchar_t);
			ttASSERT(cb < MAX_STRING);
			if (cb <= 0 || cb > MAX_STRING) // empty or invalid string
				return;

			cb = ttstrlen(m_psz) + ttstrlen(pwsz) + 1;	// we use kstrLen(psz) in case it is a DBCS character (which could be 2 bytes)
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			ttstrcat(m_psz, DEST_SIZE, pwsz);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == '%') {
			size_t cb = ttstrlen(m_psz) + 2;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			ttstrcat(m_psz, DEST_SIZE, L"%");
			pszEnd++;
			continue;
		}
		else {
			// This is a potential security risk since we don't know what size of argument to retrieve from va_arg(). We simply
			// print the rest of the format string and don't pop any arguments off.

			ttFAIL("Invalid format string for printf");
			size_t cb;
#ifdef _DEBUG
			cb = ttstrlen(m_psz) + ttstrlen(L"Invalid format string: ") + 1;
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			ttstrcat(m_psz, DEST_SIZE, L"Invalid format string: ");
#endif // _DEBUG
			cb = ttstrlen(m_psz) + ttstrlen(pszEnd) + 2;	// make room for leading % character
			if (cb > cAvail) {
				cb >>= 7;
				cb <<= 7;
				cb += 0x80;	// round up to 128
				m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				cAvail = cb - 1;
				ttASSERT(cAvail < 4096);
			}
			ttstrcat(m_psz, DEST_SIZE, L"%");
			ttstrcat(m_psz, DEST_SIZE, pszEnd);
			break;
		}
	}

	// Now readjust the allocation to the actual size

	m_psz = (wchar_t*) ttrealloc(m_psz, tt::StrByteLen(m_psz));
}

const wchar_t* ttCWStr::ProcessKFmt(const wchar_t* pszEnd, va_list* pargList)
{
	wchar_t szwBuf[256];
	szwBuf[0] = L'\0';
	switch (*pszEnd) {
		case 'n':	// 'n' is deprecated, 'd' should be used instead
		case 'd':
			tt::Itoa((int) va_arg(*pargList, int), szwBuf, sizeof(szwBuf));
			ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
			break;

		case 'I':	// 64-bit version of 'd' and 'u' that works in 32-bit builds
			if (tt::IsSameSubStrI(pszEnd, L"I64d"))
				tt::Itoa(va_arg(*pargList, int64_t), szwBuf, sizeof(szwBuf));
			else if (tt::IsSameSubStrI(pszEnd, L"I64u"))
				tt::Utoa(va_arg(*pargList, uint64_t), szwBuf, sizeof(szwBuf));
			ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
			pszEnd += 3;	// skip over I64 portion, then count normally
			break;

		case 't':	// use for size_t parameters, this will handle both 32 and 64 bit compilations
			tt::Utoa(va_arg(*pargList, size_t), szwBuf, sizeof(szwBuf));
			ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
			break;

		case 'u':
			tt::Utoa(va_arg(*pargList, unsigned int), szwBuf, sizeof(szwBuf));
			ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
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
				ttCWStr cszRes;
				cszRes.GetResString(va_arg(*pargList, int));
				ttstrcpy(szwBuf, sizeof(szwBuf), cszRes);
			}
			break;


		case 'e':
			{
				wchar_t* pszMsg;

				FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, va_arg(*pargList, int), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(wchar_t*) &pszMsg, 0, NULL);
				ttstrcpy(szwBuf, sizeof(szwBuf), pszMsg);
				LocalFree((HLOCAL) pszMsg);
			}
			break;
#endif // _WINDOWS_

		case 'q':
			try {
				const wchar_t* psz = va_arg(*pargList, const wchar_t*);
				size_t cb = ttstrlen(m_psz) + ttstrlen(psz) + (3 * sizeof(wchar_t));
				if (cb > ttsize(m_psz)) {
					cb >>= 7;
					cb <<= 7;
					cb += 0x80;	// round up to 128
					m_psz = (wchar_t*) ttrealloc(m_psz, cb);
				}
				ttstrcat(m_psz, L"\042");
				ttstrcat(m_psz, psz);
				ttstrcat(m_psz, L"\042");
			}
			catch (...) {
				ttFAIL("Exception in printf -- bad %%kq pointer");
			}
			break;
	}
	if (szwBuf[0]) {
		size_t cbCur = tt::StrByteLen(m_psz);
		size_t cb = cbCur + tt::StrByteLen(szwBuf);
		if (cb > ttsize(m_psz)) {
			cb >>= 7;
			cb <<= 7;
			cb += 0x80;	// round up to 128
			m_psz = (wchar_t*) ttrealloc(m_psz, cb);
		}
		ttstrcat(m_psz, DEST_SIZE - cbCur, szwBuf);
	}
	return pszEnd + 1;
}

// We allow for pszNum and pszDst to be different in case the pszNum buffer is only large
// enough to hold the number and not the commas

void ttpriv::AddCommasToNumber(wchar_t* pszNum, wchar_t* pszDst, size_t cbDst)
{
	size_t cchDst = cbDst / sizeof(wchar_t);
	if (pszDst != pszNum)
		ttstrcpy(pszDst, cbDst, pszNum);	// copy the number, performa all additional work in-place in the destination buffer

	size_t cchNum = ttstrlen(pszDst);	// needs to be signed because it can go negative
	if (cchNum < 4) {
		ttASSERT(cchNum * sizeof(wchar_t) < cbDst);
		return;
	}
	ttASSERT(cchNum + (cchNum / 3) < cchDst);
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
		memmove(pszDst + cchStart + 1, pszDst + cchStart, tt::StrByteLen(pszDst + cchStart) + sizeof(wchar_t));	// make space for a comma
		pszDst[cchStart] = ',';
		++cchNum;		// track that we added a comma for loop comparison
		cchStart += 4;	// 3 numbers plus the comma
	}
}
