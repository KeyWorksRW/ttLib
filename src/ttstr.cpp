/////////////////////////////////////////////////////////////////////////////
// Name:		ttstr.cpp
// Purpose:		various functions dealing with strings
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"		// precompiled header

#include <cstring>
#include <cwchar>
#include <cerrno>
#include <cctype>
#include <cwctype>

#include "../include/ttstr.h"
#include "../include/ttdebug.h"	// ttASSERTS

// We use our own "safe" string handling instead of strsafe.h. Rather then returning an error, we try to do
// the "right" thing that will allow the program to continue, but without a buffer overun, or GPF caused by
// NULL pointer. Note also that we have a significantly smaller max string length (16,777,215 versus 2,147,483,647)

size_t tt::strlen(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (psz) {
		size_t cch = std::strlen(psz);
		ttASSERT_MSG(cch < tt::MAX_STRING_LEN, "String is too long!");
		if (cch > tt::MAX_STRING_LEN)
			cch = tt::MAX_STRING_LEN;
		return cch;
	}
	return 0;
}

size_t tt::strlen(const wchar_t* pwsz)
{
	ttASSERT_MSG(pwsz, "NULL pointer!");
	if (pwsz) {
		size_t cch = std::wcslen(pwsz);
		ttASSERT_MSG(cch < tt::MAX_STRING_LEN, "String is too long!");
		if (cch > tt::MAX_STRING_LEN)
			cch = tt::MAX_STRING_LEN;
		return cch;
	}
	return 0;
}

int tt::strcpy_s(char* pszDst, size_t cchDest, const char* pszSrc)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == nullptr)
		return EINVAL;

	if (pszSrc == nullptr) {
		*pszDst = 0;
		return EINVAL;
	}

	ttASSERT_MSG(tt::strbyte(pszSrc) <= cchDest, "buffer overflow");

	int result = 0;
	if (cchDest >= tt::MAX_STRING_LEN) {
		cchDest = tt::MAX_STRING_LEN - sizeof(char);
		result = EOVERFLOW;
	}

	while (cchDest > 0 && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cchDest--;
	}
	*pszDst = 0;
	return result;
}

int tt::strcpy_s(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc)
{
	ttASSERT_MSG(pwszDst, "NULL pointer!");
	ttASSERT_MSG(pwszSrc, "NULL pointer!");

	if (pwszDst == nullptr)
		return EINVAL;

	// we assume the caller can deal with an empty destination string if the src pointer is NULL, or the size
	// is negative

	if (pwszSrc == NULL) {
		*pwszDst = L'\0';
		return EINVAL;
	}

	ttASSERT_MSG(tt::strbyte(pwszSrc) <= cchDest, "buffer overflow");

	int result = 0;
	if (cchDest >= tt::MAX_STRING_LEN) {
		cchDest = tt::MAX_STRING_LEN - sizeof(wchar_t);
		result = EOVERFLOW;
	}

	while (cchDest > 0 && (*pwszSrc != L'\0')) {
		*pwszDst++ = *pwszSrc++;
		cchDest--;
	}
	*pwszDst = L'\0';
	return result;
}

int tt::strcat_s(char* pszDst, size_t cchDest, const char* pszSrc)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == nullptr || pszSrc == nullptr)
		return EINVAL;

	size_t cch = std::strlen(pszDst);
	ttASSERT_MSG(cch < tt::MAX_STRING_LEN, "String is too long!");

	int result = 0;

	if (cch > tt::MAX_STRING_LEN) {
		cch = tt::MAX_STRING_LEN;
		result = EOVERFLOW;
	}
	ttASSERT_MSG(cch < cchDest, "Destination is too small");
	if (cch > cchDest) {
		cch = cchDest;
		result = EOVERFLOW;
	}
	pszDst += cch;
	cchDest -= cch;
	while (cchDest && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cchDest--;
	}
	*pszDst = 0;
	return result;
}

int tt::strcat_s(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == nullptr || pszSrc == nullptr)
		return EINVAL;

	int result = 0;

	size_t cch = tt::strlen(pszDst);
	ttASSERT_MSG(cch < tt::MAX_STRING_LEN, "String is too long!");

	if (cch > tt::MAX_STRING_LEN) {
		cch = tt::MAX_STRING_LEN;
		result = EOVERFLOW;
	}
	ttASSERT_MSG(cch < cchDest, "Destination is too small");
	if (cch > cchDest) {
		cch = cchDest;
		result = EOVERFLOW;
	}
	pszDst += cch;
	cchDest -= cch;
	while (cchDest && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cchDest--;
	}
	*pszDst = L'\0';
	return result;
}

char* tt::strchr(const char* psz, char ch)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;
	while (*psz && *psz != ch)
		psz = tt::nextchr(psz);
	return (*psz ? (char*) psz : nullptr);
}

wchar_t* tt::strchr(const wchar_t* psz, wchar_t ch)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;
	while (*psz && *psz != ch)
		psz++;
	return (*psz ? (wchar_t*) psz : nullptr);
}

// Windows StrRChr doesn't use codepages, so won't correctly handle SBCS UTF8 string

char* tt::strchrR(const char* psz, char ch)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;

	const char* pszLastFound = tt::strchr(psz, ch);
	if (pszLastFound) {
		for (;;) {
			psz = tt::strchr(tt::nextchr(pszLastFound), ch);
			if (psz)
				pszLastFound = psz;
			else
				break;
		}
	}
	return (char*) pszLastFound;
}

wchar_t* tt::strchrR(const wchar_t* psz, wchar_t ch)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;

	const wchar_t* pszLastFound = tt::strchr(psz, ch);
	if (pszLastFound) {
		for (;;) {
			psz = tt::strchr(pszLastFound + 1, ch);
			if (psz)
				pszLastFound = psz;
			else
				break;
		}
	}
	return (wchar_t*) pszLastFound;
}

bool tt::samestr(const char* psz1, const char* psz2)
{
	ttASSERT_MSG(psz1, "NULL pointer!");
	ttASSERT_MSG(psz2, "NULL pointer!");
	if (!psz1 || !psz2)
		return false;

	while (*psz1 == *psz2) {
		if (!*psz1)
			return true;
		psz1++;
		psz2++;
	}
	return false;
}

bool tt::samestr(const wchar_t* psz1, const wchar_t* psz2)
{
	ttASSERT_MSG(psz1, "NULL pointer!");
	ttASSERT_MSG(psz2, "NULL pointer!");
	if (!psz1 || !psz2)
		return false;

	while (*psz1 == *psz2) {
		if (!*psz1)
			return true;
		psz1++;
		psz2++;
	}
	return false;
}

bool tt::samestri(const char* psz1, const char* psz2)
{
	if (!psz1 || !psz2)
		return false;

	if (tt::strlen(psz1) != tt::strlen(psz2))
		return false;
	for (;;) {
		if (*psz1 != *psz2)	{
			if (std::tolower(*psz1) != std::tolower(*psz2))
				return false;	// doesn't match even when case is made the same
		}
		if (!*psz1)
			return true;
		psz1 = tt::nextchr(psz1);
		psz2 = tt::nextchr(psz2);
	}
}

bool tt::samestri(const wchar_t* psz1, const wchar_t* psz2)
{
	if (!psz1 || !psz2)
		return false;

	if (tt::strlen(psz1) != tt::strlen(psz2))
		return false;
	for (;;) {
		if (*psz1 != *psz2)	{
			if (std::towlower(*psz1) != std::towlower(*psz2))
				return false;	// doesn't match even when case is made the same
		}
		if (!*psz1)
			return true;
		++psz1;
		++psz2;
	}
}

bool tt::samesubstr(const char* pszMain, const char* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub)
			return false;	// doesn't match even when case is made the same
		pszMain = tt::nextchr(pszMain);
		pszSub	= tt::nextchr(pszSub);
	}
	return true;
}

bool tt::samesubstr(const wchar_t* pszMain, const wchar_t* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub)
			return false;	// doesn't match even when case is made the same
		++pszMain;
		++pszSub;
	}
	return true;
}

bool tt::samesubstri(const char* pszMain, const char* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub) {
			if (std::tolower(*pszMain) != std::tolower(*pszSub))
				return false;	// doesn't match even when case is made the same
		}
		pszMain = tt::nextchr(pszMain);
		pszSub	= tt::nextchr(pszSub);
	}
	return true;
}

bool tt::samesubstri(const wchar_t* pszMain, const wchar_t* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub) {
			if (std::towlower(*pszMain) != std::towlower(*pszSub))
				return false;	// doesn't match even when case is made the same
		}
		++pszMain;
		++pszSub;
	}
	return true;
}

// find a case-insensitive extension in a path string

const char* tt::strext(const char* pszPath, const char* pszExt)
{
	ttASSERT_MSG(pszPath, "NULL pointer!");
	ttASSERT_MSG(pszExt, "NULL pointer!");
	if (!pszPath || !pszExt)
		return nullptr;

	char* psz = tt::strchrR(pszPath, '.');
	if (!psz)
		return pszExt;

#ifdef _WX_WX_H_
	wxString s(psz);
	if (s.CmpNoCase(pszExt) == 0)
		return psz;
#else
	if (lstrcmpi(psz, pszExt) == 0)	// may not handle UTF8 correctly
		return psz;
#endif
	return nullptr;
}

char* tt::stristr(const char* pszMain, const char* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return nullptr;

#ifdef _WX_WX_H_
	wxString sMain(pszMain);
	wxString sSub(pszSub);
	sMain.MakeLower();
	sSub.MakeLower();
	int iPos = sMain.find(sSub);
	if (iPos == wxNOT_FOUND)
		return nullptr;
	return (char*) pszMain + iPos;
#else	// not _WX_WX_H_
	char* pszTmp1;
	char* pszTmp2;
	char lowerch = (char) tolower(*pszSub);
	char upperch = (char) toupper(*pszSub);

	while (*pszMain) {
		if (*pszMain != lowerch && *pszMain != upperch) {
			pszMain++;
		}
		else {
			if (!pszSub[1])		// end of substring, means we found a match
				return (char*) pszMain;
			pszTmp1 = (char*) (pszMain + 1);
			if (!*pszTmp1) {
				return nullptr;
			}
			pszTmp2 = (char*) (pszSub + 1);
			while (*pszTmp1) {
				if (tolower(*pszTmp1++) != tolower(*pszTmp2++)) {
					pszMain++;	// increment main, go back to looking for first character match
					break;
				}
				if (!*pszTmp2) {	// end of substring, means we found a match
					return (char*) pszMain;
				}
				if (!*pszTmp1) {
					return nullptr;	// end of main string before end of sub string, so not possible to match
				}
			}
		}
	}
	return nullptr;	// end of main string
#endif // _WX_WX_H_
}

// Similar as C runtime strstr, only this one checks for NULL pointers and an empty sub string

char* tt::strstr(const char* pszMain, const char* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub || !*pszSub)
		return nullptr;

// We keep the first character of pszSub in first_ch. First we loop trying to find a match for this character.
// Once we have found a match, we start with the second character of both pszMain and pszSub and walk through
// both strings If we make it all the way through pszSub with matches, then we bail with a pointer to the
// string's location in pszMain.

	const char* pszTmp1;
	const char* pszTmp2;
	char  first_ch = *pszSub;

	while (*pszMain) {
		if (*pszMain != first_ch) {
			pszMain++;
		}
		else {
			if (!pszSub[1])		// end of substring, means we found a match
				return (char*) pszMain;
			pszTmp1 = (pszMain + 1);
			pszTmp2 = (pszSub + 1);
			while (*pszTmp1) {
				if (*pszTmp1++ != *pszTmp2++) {
					pszMain++;	// increment main, go back to looking for first character match
					break;
				}
				if (!*pszTmp2) {	// end of substring, means we found a match
					return (char*) pszMain;
				}
			}
			if (!*pszTmp1) {
				return nullptr;	// end of main string before end of sub string, so not possible to match
				break;
			}
		}
	}
	return nullptr;	// end of main string
}

wchar_t* tt::strstr(const wchar_t* pszMain, const wchar_t* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub || !*pszSub)
		return nullptr;

// We keep the first character of pszSub in first_ch. First we loop trying to find a match for this character.
// Once we have found a match, we start with the second character of both pszMain and pszSub and walk through
// both strings If we make it all the way through pszSub with matches, then we bail with a pointer to the
// string's location in pszMain.

	const wchar_t* pszTmp1;
	const wchar_t* pszTmp2;
	wchar_t	 first_ch = *pszSub;

	while (*pszMain) {
		if (*pszMain != first_ch) {
			pszMain++;
		}
		else {
			pszTmp1 = (pszMain + 1);
			pszTmp2 = (pszSub + 1);
			while (*pszTmp1) {
				if (*pszTmp1++ != *pszTmp2++) {
					pszMain++;	// increment main, go back to looking for first character match
					break;
				}
				if (!*pszTmp2) {	// end of substring, means we found a match
					return (wchar_t*) pszMain;
				}
				if (!*pszTmp1) {
					return nullptr;	// end of main string before end of sub string, so not possible to match
					break;
				}
			}
		}
	}
	return nullptr;	// end of main string
}

#ifdef _WX_WX_H_

wchar_t* tt::stristr(const wchar_t* pszMain, const wchar_t* pszSub)
{
	ttASSERT_MSG(pszMain, "NULL pointer!");
	ttASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return nullptr;

	wxString sMain(pszMain);
	wxString sSub(pszSub);
	sMain.Lower();
	sSub.Lower();
	int iPos = sMain.find(sSub);
	if (iPos == wxNOT_FOUND)
		return nullptr;
	return (wchar_t*) pszMain + iPos;
}

#endif //  _WX_WX_H_

const char* tt::nextchr(const char*psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz) {
		return nullptr;
	}
	ttASSERT_MSG(*psz, "Empty string!");
	if (!*psz)
		return psz;
	size_t i = 0;
	(void) (tt::isutf8(psz[++i]) || tt::isutf8(psz[++i]) || tt::isutf8(psz[++i]));

	return psz + i;
}

const char* tt::nextnonspace(const char* psz)
{
	if (!psz)
		return nullptr;
	while (tt::iswhitespace(*psz))
		psz++;
	return psz;
}

const char* tt::nextspace(const char* psz)
{
	if (!psz)
		return nullptr;
	while (*psz && !tt::iswhitespace(*psz))
		psz++;
	return psz;
}

const wchar_t* tt::nextnonspace(const wchar_t* psz)
{
	if (!psz)
		return nullptr;
	while (tt::iswhitespace(*psz))
		psz++;
	return psz;
}

const wchar_t* tt::nextspace(const wchar_t* psz)
{
	if (!psz)
		return nullptr;
	while (*psz && !tt::iswhitespace(*psz))
		psz++;
	return psz;
}

void tt::trim_right(char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz || !*psz)
		return;

	char* pszEnd = psz + tt::strlen(psz) - 1;
	while ((*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\r' || *pszEnd == '\n' || *pszEnd == '\f')) {
		pszEnd--;
		if (pszEnd == psz) {
			*pszEnd = 0;
			return;
		}
	}
	pszEnd[1] = '\0';
}

char* tt::utoa(uint32_t val, char* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	char* pszRet = pszDst;
	char* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ttASSERT_MSG(cbDst > 0, "Buffer supplied to tt::itoa is too small for the supplied integer!");

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

char* tt::utoa(uint64_t val, char* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	char* pszRet = pszDst;
	char* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ttASSERT_MSG(cbDst > 0, "Buffer supplied to tt::itoa is too small for the supplied integer!");

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

char* tt::itoa(int32_t val, char* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	char* pszRet = pszDst;
	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	tt::utoa((uint32_t) val, pszDst, cbDst);
	return pszRet;
}

char* tt::itoa(int64_t val, char* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	char* pszRet = pszDst;
	tt::utoa((uint64_t) val, pszDst, cbDst);
	return pszRet;
}

wchar_t* tt::itoa(int32_t val, wchar_t* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	wchar_t* pszRet = pszDst;
	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	tt::utoa((uint32_t) val, pszDst, cbDst);
	return pszRet;
}

wchar_t* tt::itoa(int64_t val, wchar_t* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	if (val < 0) {
		*pszDst++ = '-';
		cbDst--;
	}

	wchar_t* pszRet = pszDst;
	tt::utoa((uint64_t) val, pszDst, cbDst);
	return pszRet;
}

char* tt::hextoa(size_t val, char* pszDst, bool bUpperCase)
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

ptrdiff_t tt::atoi(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
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

wchar_t* tt::hextoa(size_t val, wchar_t* pszDst, bool bUpperCase)
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

ptrdiff_t tt::atoi(const wchar_t* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
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

wchar_t* tt::utoa(uint32_t val, wchar_t* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	wchar_t* pszRet = pszDst;
	wchar_t* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ttASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

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

wchar_t* tt::utoa(uint64_t val, wchar_t* pszDst, size_t cbDst)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
	if (!pszDst || cbDst < 3)
		return nullptr;

	wchar_t* pszRet = pszDst;
	wchar_t* firstdig = pszDst;

	do {
		*pszDst++ = (char) ((val % 10) + '0');
		val /= 10;	// get next digit
		cbDst--;
	} while (cbDst > 0 && val > 0);
	ttASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

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

const char* tt::FindLastSlash(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");

	if (!psz || !*psz)
		return nullptr;

	const char* pszLastBackSlash = tt::strchrR(psz, '\\');
	const char* pszLastFwdSlash	 = tt::strchrR(psz, '/');
	if (!pszLastBackSlash)
		return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
	else if (!pszLastFwdSlash)
		return pszLastBackSlash ? pszLastBackSlash : nullptr;
	else
		return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash : pszLastBackSlash;		// Impossible for them to be equal
}

void tt::AddTrailingSlash(char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;
	const char* pszLastSlash = tt::FindLastSlash(psz);
	if (!pszLastSlash || pszLastSlash[1])	// only add if there was no slash or there was something after the slash
		tt::strcat(psz, "/");
}
