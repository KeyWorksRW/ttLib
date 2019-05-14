/////////////////////////////////////////////////////////////////////////////
// Name:		ttprintf.cpp
// Purpose:		Printing routines
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// Licence:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// See ttstr.h for a description of the formatting codes that vprintf supports

#include "pch.h"

#include "../include/ttdebug.h" 	// for ttASSERTS
#include "../include/ttheap.h"		// ttCHeap
#include "../include/ttstr.h"	// ttCStr

class ttPrintfPtr
{
public:
	ttPrintfPtr(char** ppszDst) {
		ttASSERT_MSG(ppszDst, "NULL pointer!");
		m_ppszDst = ppszDst;
		if (m_ppszDst && *m_ppszDst)
			ttFree(*m_ppszDst);
		m_cAvail = 128;
		m_psz = (char*) ttMalloc(m_cAvail + 1);
		*m_psz = 0;
	}
	~ttPrintfPtr() {
		if (m_ppszDst)
			*m_ppszDst = m_psz;
	}

	void ReAlloc(size_t cb) { m_psz = (char*) ttReAlloc(m_psz, cb); m_cAvail = cb;}
	void Need(size_t cb);
	void strCat(const char* psz) {
		Need(ttStrByteLen(m_psz) + ttStrByteLen(psz));
		ttStrCat(m_psz, m_cAvail, psz);
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
		ReAlloc(cb);	// allow it even if >= MAX_STRING_LEN, but strCat() will no longer add to it.
		m_cAvail = cb - 1;
	}
}

namespace ttpriv {
	char*	ProcessKFmt(ttPrintfPtr& sptr, const char* pszEnd, va_list* pargList, bool* pbPlural);
	void	AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst);
}

char* cdecl ttPrintf(char** ppszDst, const char* pszFormat, ...)
{
	va_list argList;
	va_start(argList, pszFormat);
 	ttVPrintf(ppszDst, pszFormat, argList);
	va_end(argList);
	return *ppszDst;
}

#define CB_MAX_FMT_WIDTH 20		// Largest formatted width we allow

#ifndef _MAX_U64TOSTR_BASE10_COUNT
	#define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

// Note that the limit here of 64k is smaller then the kstr functions that use 16m (_KSTRMAX)

#define MAX_STRING (64 * 1024)	// Use this to limit the length of a single string as a security precaution
#define	DEST_SIZE (ttsize(sptr) - sizeof(char))

void ttVPrintf(char** ppszDst, const char* pszFormat, va_list argList)
{
	ttASSERT_MSG(pszFormat, "NULL pointer!");
	ttASSERT_MSG(*pszFormat, "Empty format string!");
	if (!pszFormat || !*pszFormat)
		return;

	ttPrintfPtr sptr(ppszDst);

	const char* pszEnd = pszFormat;
	char szNumBuf[50];	// buffer used for converting numbers including plenty of room for commas

	bool bPlural = true;
	while (*pszEnd) {
		if (*pszEnd != '%') {
			const char* pszBegin = pszEnd++;
			while (*pszEnd && *pszEnd != '%')
				pszEnd++;
			size_t cb = (pszEnd - pszBegin);
			if (!cb)
				return;	// empty format string
			cb += ttStrByteLen(sptr);
			ttASSERT(cb <= MAX_STRING);
			if (cb > MAX_STRING) // empty or invalid string
				return;
			sptr.Need(cb);

			char* pszTmp = sptr + ttStrLen(sptr);
			while (pszBegin < pszEnd) {
				*pszTmp++ = *pszBegin++;
			}
			*pszTmp = '\0';

			if (!*pszEnd)
				return;
		}
		pszEnd++;
		if (*pszEnd == 'k') {	// special formatting
			pszEnd = ttpriv::ProcessKFmt(sptr, pszEnd + 1, &argList, &bPlural);
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
		if (ttIsDigit(*pszEnd)) {
			cbMin = ttAtoi(pszEnd++);
			if (cbMin > CB_MAX_FMT_WIDTH)
				cbMin = CB_MAX_FMT_WIDTH;
			while (ttIsDigit(*pszEnd))
				pszEnd++;
		}

		if (*pszEnd == 'c') {
			char szBuf[2];
			szBuf[0] = (uint8_t) (va_arg (argList, int) & 0xFF);
			szBuf[1] = '\0';
			sptr.strCat(szBuf);
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
			sptr.strCat(szBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'd' || *pszEnd == 'i') {

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)	// TODO: [randalphwa - 08-30-2018] Need CLANG preprocessor flag
			// note that we don't have to do any special processing if not compiling 64-bit app, as size_t will be same as int
			if (bSize_t) {
				ttItoa(va_arg(argList, _int64), szNumBuf, sizeof(szNumBuf) - 1);
			}
			else {
				ttItoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
			}
#else	// not defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			ttItoa(va_arg(argList, int), szNumBuf, sizeof(szNumBuf) - 1);
#endif	// defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttStrLen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strCat(szTmp);
				}
			}
			bPlural = szNumBuf[0] != '1' ? true : false;
			sptr.strCat(szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'u') {
			ttUtoa(va_arg(argList, unsigned int), szNumBuf, sizeof(szNumBuf));
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttStrLen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strCat(szTmp);
				}
			}
			bPlural = szNumBuf[0] != '1' ? true : false;
			sptr.strCat(szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'x') {
			ttHextoa(va_arg(argList, int), szNumBuf, false);
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttStrLen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strCat(szTmp);
				}
			}
			bPlural = szNumBuf[0] != '1' ? true : false;
			sptr.strCat(szNumBuf);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'X') {
			ttASSERT_MSG(!bSize_t, "zX and IX not supported");
			ttHextoa(va_arg(argList, int), szNumBuf, true);
			if (cbMin >= 0) {
				char szTmp[CB_MAX_FMT_WIDTH + 1];
				size_t diff = cbMin - ttStrLen(szNumBuf);
				if (diff > 0) {
					szTmp[diff--] = 0;
					while (diff >= 0)
						szTmp[diff--] = chPad;
					sptr.strCat(szTmp);
				}
			}
			bPlural = szNumBuf[0] != '1' ? true : false;
			sptr.strCat(szNumBuf);
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
				ttASSERT_MSG(psz, "NULL pointer passed to ttCStr::printf(\"%s");
				psz = "(missing argument for %s)";
			}
			sptr.strCat(psz);
			pszEnd++;
			continue;
		}
		else if (*pszEnd == 'S') {
WideChar:
			const wchar_t* pwsz = (wchar_t*) va_arg(argList, wchar_t*);
			if (!pwsz)
				pwsz = L"(null)";

			size_t cb = ttStrLen(pwsz) * sizeof(wchar_t);
			ttASSERT(cb < MAX_STRING);
			if (cb <= 0 || cb > MAX_STRING) // empty or invalid string
				return;

			char* psz = (char*) ttMalloc(cb + 1);
			// BUGBUG: [ralphw - 07-14-2018] Following line is _WINDOWS_ only
			cb = WideCharToMultiByte(CP_ACP, 0, pwsz, (_int32) (cb / sizeof(wchar_t)), psz, (_int32) cb, nullptr, nullptr);
			psz[cb] = '\0';

			sptr.strCat(psz);
			pszEnd++;
			ttFree(psz);
			continue;
		}
		else if (*pszEnd == '%') {
			sptr.strCat("%");
			pszEnd++;
			continue;
		}
		else {
			// This is a potential security risk since we don't know what size of argument to retrieve from va_arg(). We simply
			// print the rest of the format string and don't pop any arguments off.

			ttFAIL("Invalid format string for printf");
#ifdef _DEBUG
			sptr.strCat("Invalid format string: ");
#endif // _DEBUG
			sptr.strCat("%");
			sptr.strCat(pszEnd);
			break;
		}
	}

	// Now readjust the allocation to the actual size

	sptr.ReAlloc(ttStrByteLen(sptr));
}

char* ttpriv::ProcessKFmt(ttPrintfPtr& sptr, const char* pszEnd, va_list* pargList, bool* pbPlural)
{
	char szBuf[64];
	szBuf[0] = '\0';
	switch (*pszEnd) {
		case 'n':	// 'n' is deprecated, 'd' should be used instead
		case 'd':
			ttItoa((int) va_arg(*pargList, int), szBuf, sizeof(szBuf));
			*pbPlural = szBuf[0] != '1' ? true : false;
			ttpriv::AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			break;

		case 'I':	// 64-bit version of 'd' and 'u' that works in 32-bit builds
			if (ttIsSameSubStrI(pszEnd, "I64d"))
				ttItoa(va_arg(*pargList, int64_t), szBuf, sizeof(szBuf));
			else if (ttIsSameSubStrI(pszEnd, "I64u"))
				ttUtoa(va_arg(*pargList, uint64_t), szBuf, sizeof(szBuf));
			*pbPlural = szBuf[0] != '1' ? true : false;
			ttpriv::AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			pszEnd += 3;	// skip over I64 portion, then count normally
			break;

		case 't':	// use for size_t parameters, this will handle both 32 and 64 bit compilations
			ttUtoa(va_arg(*pargList, size_t), szBuf, sizeof(szBuf));
			*pbPlural = szBuf[0] != '1' ? true : false;
			ttpriv::AddCommasToNumber(szBuf, szBuf, sizeof(szBuf));
			break;

		case 'u':
			ttUtoa(va_arg(*pargList, unsigned int), szBuf, sizeof(szBuf));
			*pbPlural = szBuf[0] != '1' ? true : false;
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

		case 'l':
			if (pszEnd[1] == 's') {
				if (*pbPlural) {
					szBuf[0] = 's';
					szBuf[1] = '\0';
					sptr.strCat(szBuf);
				}
				return (char*) (pszEnd + 2);
			}
			break;

#ifdef _WINDOWS_
		case 'r':
			{
				ttCStr cszRes;
				cszRes.GetResString(va_arg(*pargList, int));
				sptr.strCat(cszRes);
			}
			break;

		case 'e':
			{
				char* pszMsg;

				if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL, va_arg(*pargList, int), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(char*) &pszMsg, 0, NULL) != 0) {
					sptr.strCat(pszMsg);
					LocalFree((HLOCAL) pszMsg);
				}
			}
			break;
#endif // _WINDOWS_

		case 'q':
			try {
				sptr.strCat("\042");
				sptr.strCat(va_arg(*pargList, const char*));
				sptr.strCat("\042");
			}
			catch (...) {
				ttFAIL("Exception in ProcessKFmt -- bad %%kq pointer");
			}
			break;
	}
	if (szBuf[0])
		sptr.strCat(szBuf);

	return (char*) (pszEnd + 1);
}

// pszNum and pszDst can be the same or different. They need to be different if the pszNum buffer is only large enough to
// hold the number and not the commas.

void ttpriv::AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst)
{
	if (pszDst != pszNum)
		ttStrCpy(pszDst, cbDst, pszNum);	// copy the number, performa all additional work in-place in the destination buffer

	ptrdiff_t cbNum = ttStrLen(pszDst);	// needs to be signed because it can go negative
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
		memmove(pszDst + cbStart + 1, pszDst + cbStart, ttStrByteLen(pszDst + cbStart));	// make space for a comma
		pszDst[cbStart] = ',';
		++cbNum;		// track that we added a comma for loop comparison
		cbStart += 4;	// 3 numbers plus the comma
	}
}
