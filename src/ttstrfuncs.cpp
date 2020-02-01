/////////////////////////////////////////////////////////////////////////////
// Name:      ttstr.cpp
// Purpose:   various functions dealing with strings
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"  // precompiled header

#include <cassert>

#include "../include/ttheap.h"  // ttCHeap
#include "../include/ttstr.h"   // ttCStr

using namespace ttch;  // CH_constants

// We use our own "safe" string handling instead of strsafe.h. Rather then returning an error, we try to do
// the "right" thing that will allow the program to continue, but without a buffer overun, or GPF caused by
// a NULL pointer. Note also that we have a significantly smaller max string length (16 megs versus 2 gigs).

size_t ttStrLen(const char* psz)
{
    assert(psz);
    return psz ? ::strlen(psz) : 0;
}

size_t ttStrLen(const wchar_t* pwsz)
{
    assert(pwsz);
    return pwsz ? wcslen(pwsz) : 0;
}

int ttStrCpy(char* pszDst, const char* pszSrc)
{
    return ttStrCpy(pszDst, tt::MAX_STRING_LEN, pszSrc);
}
int ttStrCpy(wchar_t* pszDst, const wchar_t* pszSrc)
{
    return ttStrCpy(pszDst, tt::MAX_STRING_LEN / sizeof(wchar_t), pszSrc);
}

int ttStrCpy(char* pszDst, size_t cbDest, const char* pszSrc)
{
    assert(pszDst);
    assert(pszSrc);

    if (pszDst == nullptr)
        return EINVAL;

    if (pszSrc == nullptr)
    {
        *pszDst = 0;
        return EINVAL;
    }

#if !defined(NDEBUG)  // Starts debug section.
    if (ttStrByteLen(pszSrc) > cbDest)
    {
        ttCStr cszMsg;
        cszMsg.printf("Buffer overflow in strCopy_s:\n\tcbDest = %ku, pszSrc = %ku", cbDest, ttStrByteLen(pszSrc));
        ttFAIL(cszMsg);
    }
#endif

    int result = 0;

    if (cbDest > tt::MAX_STRING_LEN)
    {
        cbDest = tt::MAX_STRING_LEN - sizeof(char);
        result = EOVERFLOW;
    }

    cbDest -= sizeof(char);  // leave room for trailing zero
    while (cbDest > 0 && (*pszSrc != 0))
    {
        *pszDst++ = *pszSrc++;
        cbDest -= sizeof(char);
    }
    *pszDst = 0;
    assert(!*pszSrc);  // Buffer overflow!
    return (*pszSrc ? EOVERFLOW : result);
}

int ttStrCpy(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc)
{
    assert(pszDst);
    assert(pszSrc);

    if (pszDst == nullptr)
        return EINVAL;

    if (pszSrc == nullptr)
    {
        *pszDst = 0;
        return EINVAL;
    }

    assert(ttStrByteLen(pszSrc) <= cbDest);  // buffer overflow

    int result = 0;

    if (cbDest > tt::MAX_STRING_LEN / sizeof(wchar_t))
    {
        cbDest = tt::MAX_STRING_LEN / sizeof(wchar_t) - sizeof(wchar_t);
        result = EOVERFLOW;
    }

    cbDest -= sizeof(char);  // leave room for trailing zero
    while (cbDest > 0 && (*pszSrc != 0))
    {
        *pszDst++ = *pszSrc++;
        cbDest -= sizeof(wchar_t);
    }
    *pszDst = 0;
    assert(!*pszSrc);  // Buffer overflow!
    return (*pszSrc ? EOVERFLOW : result);
}

int ttStrCat(char* pszDst, const char* pszSrc)
{
    return ttStrCat(pszDst, tt::MAX_STRING_LEN, pszSrc);
}
int ttStrCat(wchar_t* pszDst, const wchar_t* pszSrc)
{
    return ttStrCat(pszDst, tt::MAX_STRING_LEN / sizeof(wchar_t), pszSrc);
}

int ttStrCat(char* pszDst, size_t cbDest, const char* pszSrc)
{
    assert(pszDst);  // We leave this assert because this is a serious problem for the caller

    if (pszDst == nullptr || pszSrc == nullptr)
        return EINVAL;
    if (!*pszSrc)
        return 0;

    int result = 0;

    size_t cbInUse = ttStrByteLen(pszDst);
    assert(cbInUse <= tt::MAX_STRING_LEN);

    if (cbInUse > tt::MAX_STRING_LEN)
    {
        cbInUse = tt::MAX_STRING_LEN;
        result = EOVERFLOW;
    }

    assert(cbInUse < cbDest);  // Destination is too small
    if (cbInUse >= cbDest)
        return EOVERFLOW;  // we've already maxed out the destination buffer, so we can't add anything

    pszDst += (cbInUse - sizeof(char));
    cbDest -= cbInUse;
    while (cbDest && (*pszSrc != 0))
    {
        *pszDst++ = *pszSrc++;
        cbDest -= sizeof(char);
    }
    *pszDst = 0;
    assert(!*pszSrc);  // Buffer overflow!
    return (*pszSrc ? EOVERFLOW : result);
}

int ttStrCat(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc)
{
    assert(pszDst);

    if (pszDst == nullptr || pszSrc == nullptr)
        return EINVAL;
    if (!*pszSrc)
        return 0;

    int result = 0;

    size_t cbInUse = ttStrByteLen(pszDst);
    assert(cbInUse <= tt::MAX_STRING_LEN / sizeof(wchar_t));

    if (cbInUse > tt::MAX_STRING_LEN / sizeof(wchar_t))
    {
        cbInUse = tt::MAX_STRING_LEN / sizeof(wchar_t);
        result = EOVERFLOW;
    }

    assert(cbInUse < cbDest);  // Destination is too small
    if (cbInUse >= cbDest)
        return EOVERFLOW;  // we've already maxed out the destination buffer, so we can't add anything

    pszDst += (cbInUse - sizeof(wchar_t));
    cbDest -= cbInUse;
    while (cbDest && (*pszSrc != 0))
    {
        *pszDst++ = *pszSrc++;
        cbDest -= sizeof(wchar_t);
    }
    *pszDst = 0;
    assert(!*pszSrc);  // Buffer overflow!
    return (*pszSrc ? EOVERFLOW : result);
}

char* ttStrChr(const char* psz, char ch)
{
    assert(psz);
    if (!psz)
        return nullptr;
    while (*psz && *psz != ch)
        psz = ttNextChar(psz);
    return (*psz ? (char*) psz : nullptr);
}

char* ttStrChrR(const char* psz, char ch)
{
    assert(psz);
    if (!psz)
        return nullptr;

    const char* pszLastFound = ttStrChr(psz, ch);
    if (pszLastFound)
    {
        for (;;)
        {
            psz = ttStrChr(ttNextChar(pszLastFound), ch);
            if (psz)
                pszLastFound = psz;
            else
                break;
        }
    }
    return (char*) pszLastFound;
}

wchar_t* ttStrChr(const wchar_t* psz, wchar_t ch)
{
    assert(psz);
    if (!psz)
        return nullptr;
    while (*psz && *psz != ch)
        psz++;
    return (*psz ? (wchar_t*) psz : nullptr);
}

wchar_t* ttStrChrR(const wchar_t* psz, wchar_t ch)
{
    assert(psz);
    if (!psz)
        return nullptr;

    wchar_t* pszLastFound = ttStrChr(psz, ch);
    if (pszLastFound)
    {
        for (;;)
        {
            psz = ttStrChr(pszLastFound + 1, ch);
            if (psz)
                pszLastFound = (wchar_t*) psz;
            else
                break;
        }
    }
    return (wchar_t*) pszLastFound;
}

bool ttIsSamePath(const char* psz1, const char* psz2)
{
    if (!psz1 || !psz2)
        return false;

    if (ttStrLen(psz1) != ttStrLen(psz2))
        return false;
    for (;;)
    {
        if (*psz1 != *psz2)
        {
            if (*psz1 == CH_BACKSLASH && *psz2 == CH_FORWARDSLASH)
                continue;  // consider them the same
            else if (*psz1 == CH_FORWARDSLASH && *psz2 == CH_BACKSLASH)
                continue;  // consider them the same
            else if (tolower(*psz1) != tolower(*psz2))
                return false;  // doesn't match even when case is made the same
        }
        if (!*psz1)
            return true;
        psz1 = ttNextChar(psz1);
        psz2 = ttNextChar(psz2);
    }
}

bool ttIsSamePath(const wchar_t* psz1, const wchar_t* psz2)
{
    if (!psz1 || !psz2)
        return false;

    if (ttStrLen(psz1) != ttStrLen(psz2))
        return false;
    for (;;)
    {
        if (*psz1 != *psz2)
        {
            if (*psz1 == L'\\' && *psz2 == L'/')
                continue;  // consider them the same
            else if (*psz1 == L'/' && *psz2 == L'\\')
                continue;  // consider them the same
            if (towlower(*psz1) != towlower(*psz2))
                return false;  // doesn't match even when case is made the same
        }
        if (!*psz1)
            return true;
        ++psz1;
        ++psz2;
    }
}

bool ttIsSameStr(const char* psz1, const char* psz2)
{
    if (!psz1 || !psz2)  // note that we return false even if both strings are a null pointer
        return false;

    while (*psz1 == *psz2)
    {
        if (!*psz1)
            return true;
        psz1++;
        psz2++;
    }
    return false;
}
bool ttIsSameStr(const wchar_t* psz1, const wchar_t* psz2)
{
    if (!psz1 || !psz2)
        return false;

    while (*psz1 == *psz2)
    {
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

    if (ttStrLen(psz1) != ttStrLen(psz2))
        return false;
    for (;;)
    {
        if (*psz1 != *psz2)
        {
            if (tolower(*psz1) != tolower(*psz2))
                return false;  // doesn't match even when case is made the same
        }
        if (!*psz1)
            return true;
        psz1 = ttNextChar(psz1);
        psz2 = ttNextChar(psz2);
    }
}

bool ttIsSameStrI(const wchar_t* psz1, const wchar_t* psz2)
{
    if (!psz1 || !psz2)
        return false;

    if (ttStrLen(psz1) != ttStrLen(psz2))
        return false;
    for (;;)
    {
        if (*psz1 != *psz2)
        {
            if (towlower(*psz1) != towlower(*psz2))
                return false;  // doesn't match even when case is made the same
        }
        if (!*psz1)
            return true;
        ++psz1;
        ++psz2;
    }
}

bool ttIsSameSubStr(const char* pszMain, const char* pszSub)
{
    if (!pszMain || !pszSub)
        return false;

    while (*pszSub)
    {
        if (*pszMain != *pszSub)
            return false;  // doesn't match even when case is made the same
        pszMain = ttNextChar(pszMain);
        pszSub = ttNextChar(pszSub);
    }
    return true;
}

bool ttIsSameSubStr(const wchar_t* pszMain, const wchar_t* pszSub)
{
    if (!pszMain || !pszSub)
        return false;

    while (*pszSub)
    {
        if (*pszMain != *pszSub)
            return false;  // doesn't match even when case is made the same
        ++pszMain;
        ++pszSub;
    }
    return true;
}

bool ttIsSameSubStrI(const char* pszMain, const char* pszSub)
{
    if (!pszMain || !pszSub)
        return false;

    while (*pszSub)
    {
        if (*pszMain != *pszSub)
        {
            if (tolower(*pszMain) != tolower(*pszSub))
                return false;  // doesn't match even when case is made the same
        }
        pszMain = ttNextChar(pszMain);
        pszSub = ttNextChar(pszSub);
    }
    return true;
}

bool ttIsSameSubStrI(const wchar_t* pszMain, const wchar_t* pszSub)
{
    if (!pszMain || !pszSub)
        return false;

    while (*pszSub)
    {
        if (*pszMain != *pszSub)
        {
            if (towlower(*pszMain) != towlower(*pszSub))
                return false;  // doesn't match even when case is made the same
        }
        ++pszMain;
        ++pszSub;
    }
    return true;
}

// find a case-insensitive extension in a path string

char* ttFindExt(const char* pszPath, const char* pszExt)
{
    if (!pszPath || !pszExt)
        return nullptr;

    char* psz = ttStrChrR(pszPath, '.');
    return (psz && ttIsSameStrI(psz, pszExt)) ? psz : nullptr;
}

wchar_t* ttFindExt(const wchar_t* pszPath, const wchar_t* pszExt)
{
    if (!pszPath || !pszExt)
        return nullptr;

    wchar_t* psz = ttStrChrR(pszPath, '.');
    return (psz && ttIsSameStrI(psz, pszExt)) ? psz : nullptr;
}

char* ttStrStrI(const char* pszMain, const char* pszSub)
{
    if (!pszMain || !pszSub)
        return nullptr;
    if (!*pszSub)
        return (char*) pszMain;  // matches what strstr does
    char* pszTmp1;
    char* pszTmp2;
    char  lowerch = (char) tolower(*pszSub);
    char  upperch = (char) toupper(*pszSub);

    while (*pszMain)
    {
        if (*pszMain != lowerch && *pszMain != upperch)
        {
            pszMain++;
        }
        else
        {
            if (!pszSub[1])  // end of substring, means we found a match
                return (char*) pszMain;
            pszTmp1 = (char*) (pszMain + 1);
            if (!*pszTmp1)
                return nullptr;

            pszTmp2 = (char*) (pszSub + 1);
            while (*pszTmp1)
            {
                if (tolower(*pszTmp1++) != tolower(*pszTmp2++))
                {
                    pszMain++;  // increment main, go back to looking for first character match
                    break;
                }
                if (!*pszTmp2)  // end of substring, means we found a match
                {
                    return (char*) pszMain;
                }
                if (!*pszTmp1)
                {
                    return nullptr;  // end of main string before end of sub string, so not possible to match
                }
            }
        }
    }
    return nullptr;  // end of main string
}

wchar_t* ttStrStrI(const wchar_t* pszMain, const wchar_t* pszSub)
{
    if (!pszMain || !pszSub)
        return nullptr;
    if (!*pszSub)
        return (wchar_t*) pszMain;  // matches what strstr does
    wchar_t* pszTmp1;
    wchar_t* pszTmp2;
    wchar_t  lowerch = (char) towlower(*pszSub);
    wchar_t  upperch = (char) towupper(*pszSub);

    while (*pszMain)
    {
        if (*pszMain != lowerch && *pszMain != upperch)
        {
            pszMain++;
        }
        else
        {
            if (!pszSub[1])  // end of substring, means we found a match
                return (wchar_t*) pszMain;
            pszTmp1 = (wchar_t*) (pszMain + 1);
            if (!*pszTmp1)
                return nullptr;

            pszTmp2 = (wchar_t*) (pszSub + 1);
            while (*pszTmp1)
            {
                if (towlower(*pszTmp1++) != towlower(*pszTmp2++))
                {
                    pszMain++;  // increment main, go back to looking for first character match
                    break;
                }
                if (!*pszTmp2)  // end of substring, means we found a match
                {
                    return (wchar_t*) pszMain;
                }
                if (!*pszTmp1)
                {
                    return nullptr;  // end of main string before end of sub string, so not possible to match
                }
            }
        }
    }
    return nullptr;  // end of main string
}

// Similar as C runtime strstr, only this one checks for NULL pointers and an empty sub string

char* ttStrStr(const char* pszMain, const char* pszSub)
{
    if (!pszMain || !pszSub)
        return nullptr;
    if (!*pszSub)
        return (char*) pszMain;  // matches what strstr does

    // We keep the first character of pszSub in first_ch. First we loop trying to find a match for this character.
    // Once we have found a match, we start with the second character of both pszMain and pszSub and walk through
    // both strings If we make it all the way through pszSub with matches, then we bail with a pointer to the
    // string's location in pszMain.

    const char* pszTmp1;
    const char* pszTmp2;
    char        first_ch = *pszSub;

    while (*pszMain)
    {
        if (*pszMain != first_ch)
        {
            pszMain++;
        }
        else
        {
            if (!pszSub[1])  // end of substring, means we found a match
                return (char*) pszMain;
            pszTmp1 = (pszMain + 1);
            pszTmp2 = (pszSub + 1);
            while (*pszTmp1)
            {
                if (*pszTmp1++ != *pszTmp2++)
                {
                    pszMain++;  // increment main, go back to looking for first character match
                    break;
                }
                if (!*pszTmp2)  // end of substring, means we found a match
                {
                    return (char*) pszMain;
                }
            }
            if (!*pszTmp1)
            {
                return nullptr;  // end of main string before end of sub string, so not possible to match
                break;
            }
        }
    }
    return nullptr;  // end of main string
}

wchar_t* ttStrStr(const wchar_t* pszMain, const wchar_t* pszSub)
{
    if (!pszMain || !pszSub)
        return nullptr;
    if (!*pszSub)
        return (wchar_t*) pszMain;  // matches what strstr does

    // We keep the first character of pszSub in first_ch. First we loop trying to find a match for this character.
    // Once we have found a match, we start with the second character of both pszMain and pszSub and walk through
    // both strings If we make it all the way through pszSub with matches, then we bail with a pointer to the
    // string's location in pszMain.

    const wchar_t* pszTmp1;
    const wchar_t* pszTmp2;
    wchar_t        first_ch = *pszSub;

    while (*pszMain)
    {
        if (*pszMain != first_ch)
        {
            pszMain++;
        }
        else
        {
            pszTmp1 = (pszMain + 1);
            pszTmp2 = (pszSub + 1);
            while (*pszTmp1)
            {
                if (*pszTmp1++ != *pszTmp2++)
                {
                    pszMain++;  // increment main, go back to looking for first character match
                    break;
                }
                if (!*pszTmp2)  // end of substring, means we found a match
                {
                    return (wchar_t*) pszMain;
                }
                if (!*pszTmp1)
                {
                    return nullptr;  // end of main string before end of sub string, so not possible to match
                    break;
                }
            }
        }
    }
    return nullptr;  // end of main string
}

char* ttNextChar(const char* psz)
{
    assert(psz);  // This is a serious problem for the caller so we assert (see Issue #45 for discussion)
    if (!psz)
        return nullptr;
    if (!*psz)
        return (char*) psz;
    size_t i = 0;
    (void) (ttIsUTF8(psz[++i]) || ttIsUTF8(psz[++i]) || ttIsUTF8(psz[++i]));

    return (char*) psz + i;
}

char* ttFindNonSpace(const char* psz)
{
    if (!psz)
        return nullptr;
    while (ttIsWhitespace(*psz))
        psz++;
    return (char*) psz;
}

char* ttFindSpace(const char* psz)
{
    if (!psz)
        return nullptr;
    while (*psz && !ttIsWhitespace(*psz))
        psz++;
    return (char*) psz;
}

wchar_t* ttFindNonSpace(const wchar_t* psz)
{
    if (!psz)
        return nullptr;
    while (ttIsWhitespace(*psz))
        psz++;
    return (wchar_t*) psz;
}

wchar_t* ttFindSpace(const wchar_t* psz)
{
    if (!psz)
        return nullptr;
    while (*psz && !ttIsWhitespace(*psz))
        psz++;
    return (wchar_t*) psz;
}

char* ttStepOver(const char* psz)
{
    if (!psz)
        return nullptr;
    while (*psz && *psz != ' ' && *psz != '\t' && *psz != '\r' && *psz != '\n' &&
           *psz != '\f')  // step over all non whitespace
        ++psz;
    while (*psz == ' ' || *psz == '\t' || *psz == '\r' || *psz == '\n' ||
           *psz == '\f')  // step over all whitespace
        ++psz;
    return (char*) psz;
}

wchar_t* ttStepOver(const wchar_t* psz)
{
    if (!psz)
        return nullptr;
    while (*psz && *psz != L' ' && *psz != L'\t' && *psz != L'\r' && *psz != L'\n' &&
           *psz != L'\f')  // step over all non whitespace
        ++psz;
    while (*psz == L' ' || *psz == L'\t' || *psz == L'\r' || *psz == L'\n' ||
           *psz == L'\f')  // step over all whitespace
        ++psz;
    return (wchar_t*) psz;
}

void ttTrimRight(char* psz)
{
    if (!psz || !*psz)
        return;

    char* pszEnd = psz + ttStrLen(psz) - 1;
    while ((*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\r' || *pszEnd == '\n' || *pszEnd == '\f'))
    {
        pszEnd--;
        if (pszEnd == psz)
        {
            if ((*pszEnd == ' ' || *pszEnd == '\t' || *pszEnd == '\r' || *pszEnd == '\n' || *pszEnd == '\f'))
                *pszEnd = 0;
            else
                pszEnd[1] = '\0';
            return;
        }
    }
    pszEnd[1] = '\0';
}

void ttTrimRight(wchar_t* psz)
{
    if (!psz || !*psz)
        return;

    wchar_t* pszEnd = psz + ttStrLen(psz) - 1;
    while ((*pszEnd == L' ' || *pszEnd == L'\t' || *pszEnd == L'\r' || *pszEnd == L'\n' || *pszEnd == L'\f'))
    {
        pszEnd--;
        if (pszEnd == psz)
        {
            if ((*pszEnd == L' ' || *pszEnd == L'\t' || *pszEnd == L'\r' || *pszEnd == L'\n' || *pszEnd == L'\f'))
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
    assert(pszDst);
    assert(cbDst > 2);  // Buffer is too small!
    if (!pszDst || cbDst < 3)
        return nullptr;

    char* pszRet = pszDst;
    char* firstdig = pszDst;

    do
    {
        *pszDst++ = (char) ((val % 10) + '0');
        val /= 10;  // get next digit
        cbDst--;
    } while (cbDst > 0 && val > 0);
    assert(cbDst > 0);  // Buffer supplied to tt::Itoa is too small for the supplied integer!

    *pszDst-- = '\0';

    // The number was converted starting with the lowest digit first, so we need to flip it

    do
    {
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
    assert(pszDst);
    assert(cbDst > 2);
    if (!pszDst || cbDst < 3)
        return nullptr;

    char* pszRet = pszDst;
    char* firstdig = pszDst;

    do
    {
        *pszDst++ = (char) ((val % 10) + '0');
        val /= 10;  // get next digit
        cbDst--;
    } while (cbDst > 0 && val > 0);
    assert(cbDst > 0);  // Buffer supplied to tt::Itoa is too small for the supplied integer!

    *pszDst-- = '\0';

    // The number was converted starting with the lowest digit first, so we need to flip it

    do
    {
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
    assert(pszDst);
    assert(cbDst > 2);
    if (!pszDst || cbDst < 3)
        return nullptr;

    char* pszRet = pszDst;
    if (val < 0)
    {
        *pszDst++ = '-';
        cbDst--;
    }

    ttUtoa((uint32_t) abs(val), pszDst, cbDst);
    return pszRet;
}

char* ttItoa(int64_t val, char* pszDst, size_t cbDst)
{
    assert(pszDst);
    assert(cbDst > 2);
    if (!pszDst || cbDst < 3)
        return nullptr;

    if (val < 0)
    {
        *pszDst++ = '-';
        cbDst--;
    }

    char* pszRet = pszDst;
    ttUtoa((uint64_t) abs(val), pszDst, cbDst);
    return pszRet;
}

wchar_t* ttItoa(int32_t val, wchar_t* pszDst, size_t cbDst)
{
    assert(pszDst);
    assert(cbDst > 2);  // Buffer is too small!
    if (!pszDst || cbDst < 3)
        return nullptr;

    wchar_t* pszRet = pszDst;
    if (val < 0)
    {
        *pszDst++ = '-';
        cbDst--;
    }

    ttUtoa((uint32_t) val, pszDst, cbDst);
    return pszRet;
}

wchar_t* ttItoa(int64_t val, wchar_t* pszDst, size_t cbDst)
{
    assert(pszDst);
    assert(cbDst > 2);  // Buffer is too small!
    if (!pszDst || cbDst < 3)
        return nullptr;

    if (val < 0)
    {
        *pszDst++ = '-';
        cbDst--;
    }

    wchar_t* pszRet = pszDst;
    ttUtoa((uint64_t) val, pszDst, cbDst);
    return pszRet;
}

char* ttHextoa(size_t val, char* pszDst, bool bUpperCase)
{
    static char* szBuf = NULL;
    if (!pszDst)
    {
        if (!szBuf)
            // extra room for null termination and general paranoia
            szBuf = (char*) ttMalloc(sizeof(size_t) * sizeof(char) + sizeof(char) * 4);
        pszDst = szBuf;
    }
    char* pszRet = pszDst;
    char* psz = pszDst;
    do
    {
        size_t digval = (val % 16);
        val /= 16;  // get next digit

        if (digval > 9)
            *psz++ = (char) (digval - 10 + (bUpperCase ? 'A' : 'a'));
        else
            *psz++ = (char) (digval + '0');
    } while (val > 0);

    *psz-- = '\0';  // terminate string; p points to last digit

    do
    {
        char temp = *psz;
        *psz = *pszDst;
        *pszDst = temp;
        --psz;
        ++pszDst;
    } while (pszDst < psz);  // repeat until halfway
    return pszRet;
}

ptrdiff_t ttAtoi(const char* psz)
{
    assert(psz);
    if (!psz)
        return 0;

    // Skip over leading white space

    while (*psz == ' ' || *psz == '\t')
        psz++;
    if (!*psz)
        return 0;

    ptrdiff_t total = 0;

    if (psz[0] == '0' && (psz[1] == 'x' || psz[1] == 'X'))
    {
        psz += 2;  // skip over 0x prefix in hexadecimal strings

        for (;;)
        {
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

    while (c >= '0' && c <= '9')
    {
        total = 10 * total + (c - '0');
        c = (ptrdiff_t) *psz++;
    }

    if (sign == '-')
        return -total;
    else
        return total;
}

wchar_t* ttHextoa(size_t val, wchar_t* pszDst, bool bUpperCase)
{
    static wchar_t* szBuf = NULL;
    if (!pszDst)
    {
        if (!szBuf)
            // extra room for null termination and general paranoia
            szBuf = (wchar_t*) ttMalloc(sizeof(size_t) * sizeof(wchar_t) + sizeof(wchar_t) * 4);
        pszDst = szBuf;
    }
    wchar_t* pszRet = pszDst;
    wchar_t* psz = pszDst;
    do
    {
        size_t digval = (val % 16);
        val /= 16;  // get next digit

        if (digval > 9)
            *psz++ = (char) (digval - 10 + (bUpperCase ? 'A' : 'a'));
        else
            *psz++ = (char) (digval + '0');
    } while (val > 0);

    *psz-- = '\0';  // terminate string; p points to last digit

    do
    {
        wchar_t temp = *psz;
        *psz = *pszDst;
        *pszDst = temp;
        --psz;
        ++pszDst;
    } while (pszDst < psz);  // repeat until halfway
    return pszRet;
}

ptrdiff_t ttAtoi(const wchar_t* psz)
{
    assert(psz);
    if (!psz)
        return 0;

    // Skip over leading white space

    while (*psz == L' ' || *psz == L'\t')
        psz++;
    if (!*psz)
        return 0;

    ptrdiff_t total = 0;

    if (psz[0] == L'0' && (psz[1] == L'x' || psz[1] == L'X'))
    {
        psz += 2;  // skip over 0x prefix in hexadecimal strings

        for (;;)
        {
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

    while (c >= L'0' && c <= L'9')
    {
        total = 10 * total + (c - L'0');
        c = (ptrdiff_t) *psz++;
    }

    if (sign == L'-')
        return -total;
    else
        return total;
}

wchar_t* ttUtoa(uint32_t val, wchar_t* pszDst, size_t cbDst)
{
    assert(pszDst);
    assert(cbDst > 2);  // Buffer is too small!
    if (!pszDst || cbDst < 3)
        return nullptr;

    wchar_t* pszRet = pszDst;
    wchar_t* firstdig = pszDst;

    do
    {
        *pszDst++ = (char) ((val % 10) + '0');
        val /= 10;  // get next digit
        cbDst--;
    } while (cbDst > 0 && val > 0);
    assert(cbDst > 0);  // Buffer supplied to Itoa is too small for the supplied integer!

    *pszDst-- = '\0';

    // The number was converted starting with the lowest digit first, so we need to flip it

    do
    {
        wchar_t temp = *pszDst;
        *pszDst = *firstdig;
        *firstdig = temp;
        --pszDst;
        ++firstdig;
    } while (firstdig < pszDst);
    return pszRet;
}

wchar_t* ttUtoa(uint64_t val, wchar_t* pszDst, size_t cbDst)
{
    assert(pszDst);
    assert(cbDst > 2);
    if (!pszDst || cbDst < 3)
        return nullptr;

    wchar_t* pszRet = pszDst;
    wchar_t* firstdig = pszDst;

    do
    {
        *pszDst++ = (char) ((val % 10) + '0');
        val /= 10;  // get next digit
        cbDst--;
    } while (cbDst > 0 && val > 0);
    assert(cbDst > 0);  // Buffer supplied to Itoa is too small for the supplied integer!

    *pszDst-- = '\0';

    // The number was converted starting with the lowest digit first, so we need to flip it

    do
    {
        wchar_t temp = *pszDst;
        *pszDst = *firstdig;
        *firstdig = temp;
        --pszDst;
        ++firstdig;
    } while (firstdig < pszDst);
    return pszRet;
}

char* ttFindLastSlash(const char* psz)
{
    assert(psz);

    if (!psz || !*psz)
        return nullptr;

    char* pszLastBackSlash = ttStrChrR(psz, '\\');
    char* pszLastFwdSlash = ttStrChrR(psz, '/');
    if (!pszLastBackSlash)
        return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
    else if (!pszLastFwdSlash)
        return pszLastBackSlash ? pszLastBackSlash : nullptr;
    else
        return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash :
                                                    pszLastBackSlash;  // Impossible for them to be equal
}

void ttAddTrailingSlash(char* psz)
{
    assert(psz);
    if (!psz)
        return;
    char* pszLastSlash = ttFindLastSlash(psz);
    if (!pszLastSlash || pszLastSlash[1])  // only add if there was no slash or there was something after the slash
        ttStrCat(psz, "/");
}
