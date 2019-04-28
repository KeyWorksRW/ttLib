/////////////////////////////////////////////////////////////////////////////
// Name:		ttutil.cpp
// Purpose:		Various utility functions
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// see ttutilw.cpp for UNICODE versions of the functions in this module

#include "pch.h"

#include "../include/ttutil.h"		// Various utility functions
#include "../include/ttdebug.h" 	// ttASSERTS
#include "../include/ttheap.h"		// ttCHeap

// Unlike strsafe.h, the string functions here try to do the "right" thing that will allow the program to continue, but
// without a buffer overun, or GPF caused by a NULL pointer. Note also that we have a significantly smaller max string
// length (16 megs versus 2 gigs).

const size_t TT_MAX_STRING_LEN = 0x00FFFFFF;	// strings limited to 16,777,215 bytes (16 megabytes)

int ttstrcat(char* pszDst, size_t cbDest, const char* pszSrc)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");		// We leave this assert because this is a serious problem for the caller
	// ttASSERT_MSG(pszSrc, "NULL pointer!");	// Issue #45--limit asserts when we handle a nullptr correctly

	if (pszDst == nullptr || pszSrc == nullptr)
		return EINVAL;
	if (!*pszSrc)
		return 0;

	int result = 0;

	size_t cbInUse = ttStrByteLen(pszDst);
	ttASSERT_MSG(cbInUse <= TT_MAX_STRING_LEN, "String is too long!");

	if (cbInUse > TT_MAX_STRING_LEN) {
		cbInUse = TT_MAX_STRING_LEN;
		result = EOVERFLOW;
	}

	ttASSERT_MSG(cbInUse < cbDest, "Destination is too small");
	if (cbInUse >= cbDest)
		return EOVERFLOW;	// we've already maxed out the destination buffer, so we can't add anything

	pszDst += (cbInUse - sizeof(char));
	cbDest -= cbInUse;
	while (cbDest && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cbDest -= sizeof(char);
	}
	*pszDst = 0;
	ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
	return (*pszSrc ? EOVERFLOW : result);
}

int ttstrcat(char* pszDst, const char* pszSrc)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");		// We leave this assert because this is a serious problem for the caller
	// ttASSERT_MSG(pszSrc, "NULL pointer!");	// Issue #45--limit asserts when we handle a nullptr correctly

	if (pszDst == nullptr || pszSrc == nullptr)
		return EINVAL;
	if (!*pszSrc)
		return 0;

	int result = 0;

	size_t cbInUse = ttStrByteLen(pszDst);
	ttASSERT_MSG(cbInUse <= TT_MAX_STRING_LEN, "String is too long!");

	if (cbInUse > TT_MAX_STRING_LEN) {
		cbInUse = TT_MAX_STRING_LEN;
		result = EOVERFLOW;
	}

	size_t cbDest = TT_MAX_STRING_LEN - cbInUse;
	ttASSERT_MSG(cbInUse < cbDest, "Destination is too small");
	if (cbInUse >= cbDest)
		return EOVERFLOW;	// we've already maxed out the destination buffer, so we can't add anything

	pszDst += (cbInUse - sizeof(char));
	while (cbDest && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cbDest -= sizeof(char);
	}
	*pszDst = 0;
	ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
	return (*pszSrc ? EOVERFLOW : result);
}

int ttstrcpy(char* pszDst, size_t cbDest, const char* pszSrc)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == nullptr)
		return EINVAL;

	if (pszSrc == nullptr) {
		*pszDst = 0;
		return EINVAL;
	}

	int result = 0;

	if (cbDest > TT_MAX_STRING_LEN) {
		cbDest = TT_MAX_STRING_LEN - sizeof(char);
		result = EOVERFLOW;
	}

	cbDest -= sizeof(char);		// leave room for trailing zero
	while (cbDest > 0 && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cbDest -= sizeof(char);
	}
	*pszDst = 0;
	ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
	return (*pszSrc ? EOVERFLOW : result);
}

int ttstrcpy(char* pszDst, const char* pszSrc)
{
	ttASSERT_MSG(pszDst, "NULL pointer!");
	ttASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == nullptr)
		return EINVAL;

	if (pszSrc == nullptr) {
		*pszDst = 0;
		return EINVAL;
	}

	int result = 0;

	size_t cbDest = TT_MAX_STRING_LEN - sizeof(char);
	while (cbDest > 0 && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cbDest -= sizeof(char);
	}
	*pszDst = 0;
	ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
	return (*pszSrc ? EOVERFLOW : result);
}

size_t ttstrlen(const char* psz)
{
	if (psz) {
		size_t cch = ::strlen(psz); 	// now that we know it's not a null pointer, call the standard version of strLen
		ttASSERT_MSG(cch < TT_MAX_STRING_LEN, "String is too long!");
		if (cch > TT_MAX_STRING_LEN)	// make certain the string length fits within out standard limits for string length
			cch = TT_MAX_STRING_LEN;
		return cch;
	}
	return 0;
}

char* ttstrchr(const char* psz, char ch)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;
	while (*psz && *psz != ch)
		psz = ttNextChar(psz);
	return (*psz ? (char*) psz : nullptr);
}

char* ttstrrchr(const char* psz, char ch)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;

	const char* pszLastFound = ttstrchr(psz, ch);
	if (pszLastFound) {
		for (;;) {
			psz = ttstrchr(ttNextChar(pszLastFound), ch);
			if (psz)
				pszLastFound = psz;
			else
				break;
		}
	}
	return (char*) pszLastFound;
}

char* ttstrstr(const char* pszMain, const char* pszSub)
{
	if (!pszMain || !pszSub)
		return nullptr;
	if (!*pszSub)
		return (char*) pszMain;	// this is what std::strstr does

	// We keep the first character of pszSub in first_ch. First we loop trying to find a match for this character. Once
	// we have found a match, we start with the second character of both pszMain and pszSub and walk through both strings
	// If we make it all the way through pszSub with matches, then we bail with a pointer to the string's location in
	// pszMain.

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

char* ttstristr(const char* pszMain, const char* pszSub)
{
	if (!pszMain || !pszSub)
		return nullptr;
	if (!*pszSub)
		return (char*) pszMain;	// matches what strstr does

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
}

// Similar to strcmp, only this returns bool instead of int

bool ttIsSameStr(const char* psz1, const char* psz2)
{
	if (!psz1 || !psz2)		// note that we return false even if both strings are a null pointer
		return false;

	while (*psz1 == *psz2) {
		if (!*psz1)
			return true;
		psz1++;
		psz2++;
	}
	return false;
}

bool ttIsSameStrI(const char* psz1, const char* psz2)
{
	if (!psz1 || !psz2)
		return false;

	for (;;) {
		if (*psz1 != *psz2)	{
			if (tolower(*psz1) != tolower(*psz2))
				return false;	// doesn't match even when case is made the same
		}
		if (!*psz1)
			return true;
		psz1 = ttNextChar(psz1);
		psz2 = ttNextChar(psz2);
	}
}

bool ttIsSameSubStr(const char* pszMain, const char* pszSub)
{
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub)
			return false;	// doesn't match even when case is made the same
		pszMain = ttNextChar(pszMain);
		pszSub	= ttNextChar(pszSub);
	}
	return true;
}

bool ttIsSameSubStrI(const char* pszMain, const char* pszSub)
{
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub) {
			if (tolower(*pszMain) != tolower(*pszSub))
				return false;	// doesn't match even when case is made the same
		}
		pszMain = ttNextChar(pszMain);
		pszSub	= ttNextChar(pszSub);
	}
	return true;
}

char* ttFindSpace(const char* psz)
{
	if (!psz)
		return nullptr;
	while (*psz && !ttIsWhitespace(*psz))
		psz++;
	return (char*) psz;
}

char* ttFindNonSpace(const char* psz)
{
	if (!psz)
		return nullptr;
	while (ttIsWhitespace(*psz))
		psz++;
	return (char*) psz;
}

// find a case-insensitive extension in a path string

char* ttFindExt(const char* pszPath, const char* pszExt)
{
	if (!pszPath || !pszExt)
		return nullptr;

	char* psz = ttstrrchr(pszPath, '.');
	return (psz && ttIsSameStrI(psz, pszExt)) ? psz : nullptr;
}

char* ttNextChar(const char*psz)
{
	ttASSERT_NONEMPTY(psz);		// This is a serious problem for the caller so we assert (see Issue #45 for discussion)
	if (!psz)
		return nullptr;
	if (!*psz)
		return (char*) psz;
	size_t i = 0;
	(void) (ttIsUTF8(psz[++i]) || ttIsUTF8(psz[++i]) || ttIsUTF8(psz[++i]));

	return (char*) psz + i;
}

char* ttStepOver(const char* psz)
{
	if (!psz)
		return nullptr;
	while (*psz && *psz != ' ' && *psz != '\t' && *psz != '\r' && *psz != '\n' && *psz != '\f')	// step over all non whitespace
		++psz;
	while (*psz == ' ' || *psz == '\t' || *psz == '\r' || *psz == '\n' || *psz == '\f') 		// step over all whitespace
		++psz;
	return (char*) psz;
}

void ttTrimRight(char* psz)
{
	if (!psz || !*psz)
		return;

	char* pszEnd = psz + ttstrlen(psz) - 1;
	while ((*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\r' || *pszEnd == '\n' || *pszEnd == '\f')) {
		pszEnd--;
		if (pszEnd == psz) {
			if ((*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\r' || *pszEnd == '\n' || *pszEnd == '\f'))
				*pszEnd = 0;
			else
				pszEnd[1] = '\0';
			return;
		}
	}
	pszEnd[1] = '\0';
}

char* ttUtoa(uint32_t val, char* pszDst, size_t cbDst)
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
	ttASSERT_MSG(cbDst > 0, "Buffer supplied to tt::Itoa is too small for the supplied integer!");

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

char* ttUtoa(uint64_t val, char* pszDst, size_t cbDst)
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
	ttASSERT_MSG(cbDst > 0, "Buffer supplied to tt::Itoa is too small for the supplied integer!");

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

char* ttItoa(int32_t val, char* pszDst, size_t cbDst)
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

	tt::Utoa((uint32_t) val, pszDst, cbDst);
	return pszRet;
}

char* ttItoa(int64_t val, char* pszDst, size_t cbDst)
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
	tt::Utoa((uint64_t) val, pszDst, cbDst);
	return pszRet;
}

char* ttHextoa(size_t val, char* pszDst, bool bUpperCase)
{
	static char* szBuf = NULL;
	if (!pszDst) {
		if (!szBuf) {
			szBuf = (char*) ttmalloc(sizeof(size_t) * sizeof(char) + sizeof(char) * 4);	// extra room for null termination and general paranoia
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

ptrdiff_t ttAtoi(const char* psz)
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

char* ttFindLastSlash(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");

	if (!psz || !*psz)
		return nullptr;

	char* pszLastBackSlash = ttstrrchr(psz, '\\');
	char* pszLastFwdSlash	 = ttstrrchr(psz, '/');
	if (!pszLastBackSlash)
		return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
	else if (!pszLastFwdSlash)
		return pszLastBackSlash ? pszLastBackSlash : nullptr;
	else
		return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash : pszLastBackSlash;		// Impossible for them to be equal
}

void ttAddTrailingSlash(char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return;
	char* pszLastSlash = ttFindLastSlash(psz);
	if (!pszLastSlash || pszLastSlash[1])	// only add if there was no slash or there was something after the slash
		ttstrcat(psz, "/");
}
