/////////////////////////////////////////////////////////////////////////////
// Name:		kstr.cpp
// Purpose:		Safe string functions that handle UTF8
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"		// precompiled header

#include "../include/kstr.h"	// kstr functions
#include "../include/asserts.h"	// ASSERTS

char* tt::nextchr(const char*psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz) {
		return nullptr;
	}
	ASSERT_MSG(*psz, "Empty string!");
	if (!*psz)
		return (char*) psz;
	size_t i = 0;
	(void) (isutf(psz[++i]) || isutf(psz[++i]) || isutf(psz[++i]));

	return (char*) psz + i;
}

// We use our own "safe" string handling instead of strsafe.h. Rather then returning an error, we try to do
// the "right" thing that will allow the program to continue, but without a buffer overun, or GPF caused by
// NULL pointer. Note also that we have a significantly smaller max string length (16,777,215 versus 2,147,483,647)

size_t tt::strlen(const char* psz)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (psz) {
		size_t cch = strlen(psz);
		ASSERT_MSG(cch < _KSTRMAX, "String is too long!");
		if (cch > _KSTRMAX)	{
			cch = _KSTRMAX;
		}
		return cch;
	}
	return 0;
}

size_t tt::strlen(const wchar_t* pwsz)
{
	ASSERT_MSG(pwsz, "NULL pointer!");
	if (pwsz) {
		size_t cch = wcslen(pwsz);
		ASSERT_MSG(cch < _KSTRMAX, "String is too long!");
		if (cch > _KSTRMAX)	{
#if wxUSE_EXCEPTIONS
			// if a truncated length is returned and the caller allocates memory based on the returned length
			// and then does a strcpy, it will result in a buffer overflow.
			throw std::length_error("String is too long!");
#else
			cch = _KSTRMAX;
#endif
		}
		return cch;
	}
	return 0;
}

void tt::strcpy(char* pszDst, size_t cchDest, const char* pszSrc)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == NULL) {
#if wxUSE_EXCEPTIONS
		throw std::invalid_argument("NULL pointer!");
#endif
		return;
	}

	// we assume the caller can deal with an empty destination string if the src pointer is NULL, or the size
	// is negative

	if (pszSrc == NULL || cchDest > _KSTRMAX) {
		*pszDst = 0;
		return;
	}

	// Unlike std::strncpy, characters after null termination are not copied, and the destination
	// string is always null-terminated

	while (cchDest > 0 && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cchDest--;
	}
	*pszDst = 0;
}

void tt::strcpy(wchar_t* pwszDst, size_t cchDest, const wchar_t* pwszSrc)
{
	ASSERT_MSG(pwszDst, "NULL pointer!");
	ASSERT_MSG(pwszSrc, "NULL pointer!");

	if (pwszDst == NULL) {
#if wxUSE_EXCEPTIONS
		throw std::invalid_argument("NULL pointer!");
#endif
		return;
	}

	// we assume the caller can deal with an empty destination string if the src pointer is NULL, or the size
	// is negative

	if (pwszSrc == NULL || cchDest > _KSTRMAX) {
		*pwszDst = L'\0';
		return;
	}

	// Unlike std::strncpy, characters after null termination are not copied, and the destination
	// string is always null-terminated

	while (cchDest > 0 && (*pwszSrc != L'\0')) {
		*pwszDst++ = *pwszSrc++;
		cchDest--;
	}
	*pwszDst = L'\0';
}

void tt::strcat(char* pszDst, size_t cchDest, const char* pszSrc)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == NULL) {
#if wxUSE_EXCEPTIONS
		throw std::invalid_argument("NULL pointer!");
#endif
		return;
	}

	if (pszSrc == nullptr)
		return;	// do nothing if invalid pointer

	size_t cch = strlen(pszDst);
	ASSERT_MSG(cch < _KSTRMAX, "String is too long!");

	if (cch > _KSTRMAX) {
#if wxUSE_EXCEPTIONS
			// if a truncated length is returned and the caller allocates memory based on the returned length
			// and then does a strcpy, it will result in a buffer overflow.
			throw std::length_error("String is too long!");
#else
		cch = _KSTRMAX;
#endif
	}
	ASSERT_MSG(cch < cchDest, "Destination is too small");
	if (cch > cchDest)
		cch = cchDest;
	pszDst += cch;
	cchDest -= cch;
	while (cchDest && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cchDest--;
	}
	*pszDst = 0;
}

#ifdef	_MSC_VER
	#pragma warning(disable: 4706)	// assignment within conditional expression
#endif

void tt::strcat(char* pszDst, const char* pszSrc)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == NULL) {
#if wxUSE_EXCEPTIONS
		throw std::invalid_argument("NULL pointer!");
#endif
		return;
	}

	if (pszSrc == nullptr)
		return;	// do nothing if invalid pointer

	pszDst += strlen(pszDst);	// we use the "unsafe" version because we already know pszDst is a non-null pointer

	while ((*pszDst++ = *pszSrc++));
}

void tt::strcat(wchar_t* pszDst, size_t cchDest, const wchar_t* pszSrc)
{
	ASSERT_MSG(pszDst, "NULL pointer!");
	ASSERT_MSG(pszSrc, "NULL pointer!");

	if (pszDst == NULL) {
#if wxUSE_EXCEPTIONS
		throw std::invalid_argument("NULL pointer!");
#endif
		return;
	}

	if (pszSrc == nullptr)
		return;	// do nothing if invalid pointer

	size_t cch = tt::strlen(pszDst);
	ASSERT_MSG(cch < _KSTRMAX, "String is too long!");

	if (cch > _KSTRMAX) {
#if wxUSE_EXCEPTIONS
			// if a truncated length is returned and the caller allocates memory based on the returned length
			// and then does a strcpy, it will result in a buffer overflow.
			throw std::length_error("String is too long!");
#else
		cch = _KSTRMAX;
#endif
	}
	ASSERT_MSG(cch < cchDest, "Destination is too small");
	if (cch > cchDest)
		cch = cchDest;
	pszDst += cch;
	cchDest -= cch;
	while (cchDest && (*pszSrc != 0)) {
		*pszDst++ = *pszSrc++;
		cchDest--;
	}
	*pszDst = L'\0';
}

char* tt::strchr(const char* psz, char ch)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;
	while (*psz && *psz != ch)
		psz = tt::nextchr(psz);
	return (*psz ? (char*) psz : nullptr);
}

wchar_t* tt::strchr(const wchar_t* psz, wchar_t ch)
{
	ASSERT_MSG(psz, "NULL pointer!");
	if (!psz)
		return nullptr;
	while (*psz && *psz != ch)
		psz++;
	return (*psz ? (wchar_t*) psz : nullptr);
}

// Windows StrRChr doesn't use codepages, so won't correctly handle SBCS UTF8 string

char* tt::strchrR(const char* psz, char ch)
{
	ASSERT_MSG(psz, "NULL pointer!");
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
	ASSERT_MSG(psz, "NULL pointer!");
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

bool tt::strcmp(const char* psz1, const char* psz2)
{
	ASSERT_MSG(psz1, "NULL pointer!");
	ASSERT_MSG(psz2, "NULL pointer!");
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

bool tt::strcmp(const wchar_t* psz1, const wchar_t* psz2)
{
	ASSERT_MSG(psz1, "NULL pointer!");
	ASSERT_MSG(psz2, "NULL pointer!");
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

// find a case-insensitive extension in a path string

const char* tt::strext(const char* pszPath, const char* pszExt)
{
	ASSERT_MSG(pszPath, "NULL pointer!");
	ASSERT_MSG(pszExt, "NULL pointer!");
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
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
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
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
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
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
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
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
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

//////////////////// Extras ////////////////////////////

// Extra string handling functions. Since this code is normally used in a library, adding them here should
// not add any bloat to your project if you don't use them.

bool IsSameSubString(const char* pszMain, const char* pszSub)
{
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub) {
			if (tolower(*pszMain) != tolower(*pszSub))
				return false;	// doesn't match even when case is made the same
		}
		pszMain = tt::nextchr(pszMain);
		pszSub	= tt::nextchr(pszSub);
	}
	return true;
}

bool IsSameSubString(const wchar_t* pszMain, const wchar_t* pszSub)
{
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
	if (!pszMain || !pszSub)
		return false;

	while (*pszSub) {
		if (*pszMain != *pszSub) {
			if (tolower(*pszMain) != tolower(*pszSub))
				return false;	// doesn't match even when case is made the same
		}
		++pszMain;
		++pszSub;
	}
	return true;
}

bool IsCSSameSubString(const char* pszMain, const char* pszSub)
{
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
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

bool IsCSSameSubString(const wchar_t* pszMain, const wchar_t* pszSub)
{
	ASSERT_MSG(pszMain, "NULL pointer!");
	ASSERT_MSG(pszSub, "NULL pointer!");
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

bool IsSameString(const char* psz1, const char* psz2)
{
	if (!psz1 || !psz2)
		return false;

	if (tt::strlen(psz1) != tt::strlen(psz2))
		return false;
	for (;;) {
		if (*psz1 != *psz2)	{
			if (tolower(*psz1) != tolower(*psz2))
				return false;	// doesn't match even when case is made the same
		}
		if (!*psz1)
			return true;
		psz1 = tt::nextchr(psz1);
		psz2 = tt::nextchr(psz2);
	}
}

bool IsSameString(const wchar_t* psz1, const wchar_t* psz2)
{
	if (!psz1 || !psz2)
		return false;

	if (tt::strlen(psz1) != tt::strlen(psz2))
		return false;
	for (;;) {
		if (*psz1 != *psz2)	{
			if (tolower(*psz1) != tolower(*psz2))
				return false;	// doesn't match even when case is made the same
		}
		if (!*psz1)
			return true;
		++psz1;
		++psz2;
	}
}

char* FindNonSpace(const char* psz)
{
	if (!psz)
		return nullptr;
	while (IsWhiteSpace(*psz))
		psz++;
	return (char*) psz;
}

char* FindNextSpace(const char* psz)
{
	if (!psz)
		return nullptr;
	while (*psz && !IsWhiteSpace(*psz))
		psz++;
	return (char*) psz;
}
