/////////////////////////////////////////////////////////////////////////////
// Name:		ttprintf.cpp
// Purpose:		Printing routines
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// Licence:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// See ttstr.h for a description of the formatting codes that vprintf supports

#include "pch.h"

#include "../include/ttheap.h"	// ttHeap
#include "../include/ttstr.h"	// where printf/vprintf are declared

class ttPrintfPtr
{
public:
	ttPrintfPtr(char** ppszDst) {
		ttASSERT_MSG(ppszDst, "NULL pointer!");
		m_ppszDst = ppszDst;
		if (m_ppszDst && *m_ppszDst)
			tt::free(*m_ppszDst);
		m_cAvail = 128;
		m_psz = (char*) tt::malloc(m_cAvail + 1);
		*m_psz = 0;
	}
	~ttPrintfPtr() {
		if (m_ppszDst)
			*m_ppszDst = m_psz;
	}

	void realloc(size_t cb) { m_psz = (char*) tt::realloc(m_psz, cb); m_cAvail = cb;}
	void Need(size_t cb);
	void strcat(const char* psz) {
		Need(tt::strbyte(m_psz) + tt::strbyte(psz));
		tt::strcat_s(m_psz, m_cAvail, psz);
	}

	operator char*()  { return (char*) m_psz; };

	char* m_psz;
	char** m_ppszDst;
	size_t m_cAvail;
};

void ttPrintfPtr::Need(size_t cb)
{
	if (cb > m_cAvail) {
		cb >>= 7;
		cb <<= 7;
		cb +=  0x80;	// round up allocation size to 128
		ttASSERT_MSG(cb < tt::MAX_STRING_LEN, "Attempting to printf to a string that is now larger than 16 megs!");
		realloc(cb);	// allow it even if >= MAX_STRING_LEN, but strcat() will no longer add to it.
		m_cAvail = cb - 1;
	}
}

namespace ttpriv {
	char*	ProcessKFmt(ttPrintfPtr& sptr, const char* pszEnd, va_list* pargList);
	void	AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst);
}

char* cdecl tt::printf(char** ppszDst, const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
 	tt::vprintf(ppszDst, pszFormat, argList);
	va_end(argList);
	return *ppszDst;
}

#define CB_MAX_FMT_WIDTH 20		// Largest formatted width we allow

#ifndef _MAX_U64TOSTR_BASE10_COUNT
	#define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

// Note that the limit here of 64k is smaller then the kstr functions that use 16m (_KSTRMAX)

#define MAX_STRING (64 * 1024)	// Use this to limit the length of a single string as a security precaution
#define	DEST_SIZE (tt::size(sptr) - sizeof(char))

void tt::vprintf(char** ppszDst, const char* pszFormat, va_list argList)
{
	ttASSERT_MSG(pszFormat, "NULL pointer!");
	ttASSERT_MSG(*pszFormat, "Empty format string!");
	if (!pszFormat || !*pszFormat)
		return;

	ttPrintfPtr sptr(ppszDst);

	const char* pszEnd = pszFormat;
	char szNumBuf[50];	// buffer used for converting numbers including plenty of room for commas

	while (*pszEnd) {
		if (*pszEnd != '%') {
			const char* pszBegin = pszEnd++;
			while (*pszEnd && *pszEnd != '%')
				pszEnd++;
			size_t cb = (pszEnd - pszBegin);
			if (!cb)
				return;	// empty format string
			cb += tt::strbyte(sptr);
			ttASSERT(cb <= MAX_STRING);
			if (cb > MAX_STRING) // empty or invalid string
				return;
			sptr.Need(cb);

			char* pszTmp = sptr + tt::strlen(sptr);
			while (pszBegin < pszEnd) {
				*pszTmp++ = *pszBegin++;
			}
			*pszTmp = '\0';

			if (!*pszEnd)
				return;
		}
		pszEnd++;
		if (*pszEnd == 'k') {	// special formatting
			pszEnd = ttpriv::ProcessKFmt(sptr, pszEnd + 1, &argList);
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
			sptr.strcat(szBuf);
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
			sptr.strcat(szBuf);
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
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strcat(szTmp);
				}
			}
			sptr.strcat(szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'u') {
			tt::utoa(va_arg(argList, unsigned int), szNumBuf, sizeof(szNumBuf));
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strcat(szTmp);
				}
			}
			sptr.strcat(szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'x') {
			tt::hextoa(va_arg(argList, int), szNumBuf, false);
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strcat(szTmp);
				}
			}
			sptr.strcat(szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'X') {
			ttASSERT_MSG(!bSize_t, "zX and IX not supported");
			tt::hextoa(va_arg(argList, int), szNumBuf, true);
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - tt::strlen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strcat(szTmp);
				}
			}
			sptr.strcat(szNumBuf);
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
			sptr.strcat(psz);
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

			sptr.strcat(psz);
			pszEnd++;
			tt::free(psz);
			continue;
		}
		else if (*pszEnd == '%') {
			sptr.strcat("%");
			pszEnd++;
			continue;
		}
		else {
			// This is a potential security risk since we don't know what size of argument to retrieve from va_arg(). We simply
			// print the rest of the format string and don't pop any arguments off.

			ttFAIL("Invalid format string for printf");
#ifdef _DEBUG
			sptr.strcat("Invalid format string: ");
#endif // _DEBUG
			sptr.strcat("%");
			sptr.strcat(pszEnd);
			break;
		}
	}

	// Now readjust the allocation to the actual size

	sptr.realloc(tt::strbyte(sptr));
}

char* ttpriv::ProcessKFmt(ttPrintfPtr& sptr, const char* pszEnd, va_list* pargList)
{
	char szBuf[64];
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
				ttStr cszRes;
				cszRes.getResource(va_arg(*pargList, int));
				sptr.strcat(cszRes);
			}
			break;

		case 'e':
			{
				char* pszMsg;

				if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL, va_arg(*pargList, int), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(char*) &pszMsg, 0, NULL) != 0) {
					sptr.strcat(pszMsg);
					LocalFree((HLOCAL) pszMsg);
				}
			}
			break;
#endif // _WINDOWS_

		case 'q':
			try {
				sptr.strcat("\042");
				sptr.strcat(va_arg(*pargList, const char*));
				sptr.strcat("\042");
			}
			catch (...) {
				ttFAIL("Exception in ProcessKFmt -- bad %%kq pointer");
			}
			break;
	}
	if (szBuf[0])
		sptr.strcat(szBuf);

	return (char*) (pszEnd + 1);
}

// pszNum and pszDst can be the same or different. They need to be different if the pszNum buffer is only large enough to
// hold the number and not the commas.

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
