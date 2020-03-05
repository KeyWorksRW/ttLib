/////////////////////////////////////////////////////////////////////////////
// Name:		ttutilw.cpp
// Purpose:		Wide-character versions of functions declared in ttutil.h
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ttutil.h"   // Master header file for ttLib
#include "ttdebug.h"  // ttASSERTS
#include "ttheap.h"   // ttCHeap

// Unlike strsafe.h, the string functions here try to do the "right" thing that will allow the program to continue, but
// without a buffer overun, or GPF caused by a NULL pointer. Note also that we have a significantly smaller max string
// length (16 megs versus 2 gigs).

const size_t TT_MAX_STRING_LEN = 0x00FFFFFF;  // strings limited to 16,777,215 bytes (16 megabytes)

int ttStrCat(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc)
{
    ttASSERT_MSG(pszDst, "NULL pointer!");

    if (pszDst == nullptr || pszSrc == nullptr)
        return EINVAL;
    if (!*pszSrc)
        return 0;

    int result = 0;

    size_t cbInUse = ttStrByteLen(pszDst);
    ttASSERT_MSG(cbInUse <= TT_MAX_STRING_LEN / sizeof(wchar_t), "String is too long!");

    if (cbInUse > TT_MAX_STRING_LEN / sizeof(wchar_t))
    {
        cbInUse = TT_MAX_STRING_LEN / sizeof(wchar_t);
        result = EOVERFLOW;
    }

    ttASSERT_MSG(cbInUse < cbDest, "Destination is too small");
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
    ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
    return (*pszSrc ? EOVERFLOW : result);
}

int ttStrCat(wchar_t* pszDst, const wchar_t* pszSrc)
{
    ttASSERT_MSG(pszDst, "NULL pointer!");

    if (pszDst == nullptr || pszSrc == nullptr)
        return EINVAL;
    if (!*pszSrc)
        return 0;

    int result = 0;

    size_t cbInUse = ttStrByteLen(pszDst);
    ttASSERT_MSG(cbInUse <= TT_MAX_STRING_LEN / sizeof(wchar_t), "String is too long!");

    if (cbInUse > TT_MAX_STRING_LEN / sizeof(wchar_t))
    {
        cbInUse = TT_MAX_STRING_LEN / sizeof(wchar_t);
        result = EOVERFLOW;
    }

    size_t cbDest = (TT_MAX_STRING_LEN / sizeof(wchar_t)) - cbInUse;
    ttASSERT_MSG(cbInUse < cbDest, "Destination is too small");
    if (cbInUse >= cbDest)
        return EOVERFLOW;  // we've already maxed out the destination buffer, so we can't add anything

    pszDst += (cbInUse - sizeof(wchar_t));
    while (cbDest && (*pszSrc != 0))
    {
        *pszDst++ = *pszSrc++;
        cbDest -= sizeof(wchar_t);
    }
    *pszDst = 0;
    ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
    return (*pszSrc ? EOVERFLOW : result);
}

int ttStrCpy(wchar_t* pszDst, size_t cbDest, const wchar_t* pszSrc)
{
    ttASSERT_MSG(pszDst, "NULL pointer!");
    ttASSERT_MSG(pszSrc, "NULL pointer!");

    if (pszDst == nullptr)
        return EINVAL;

    if (pszSrc == nullptr)
    {
        *pszDst = 0;
        return EINVAL;
    }

    int result = 0;

    if (cbDest > TT_MAX_STRING_LEN / sizeof(wchar_t))
    {
        cbDest = TT_MAX_STRING_LEN / sizeof(wchar_t) - sizeof(wchar_t);
        result = EOVERFLOW;
    }

    cbDest -= sizeof(wchar_t);  // leave room for trailing zero
    while (cbDest > 0 && (*pszSrc != 0))
    {
        *pszDst++ = *pszSrc++;
        cbDest -= sizeof(wchar_t);
    }
    *pszDst = 0;
    ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
    return (*pszSrc ? EOVERFLOW : result);
}

int ttStrCpy(wchar_t* pszDst, const wchar_t* pszSrc)
{
    ttASSERT_MSG(pszDst, "NULL pointer!");
    ttASSERT_MSG(pszSrc, "NULL pointer!");

    if (pszDst == nullptr)
        return EINVAL;

    if (pszSrc == nullptr)
    {
        *pszDst = 0;
        return EINVAL;
    }

    int result = 0;

    size_t cbDest = TT_MAX_STRING_LEN - sizeof(wchar_t);
    while (cbDest > 0 && (*pszSrc != 0))
    {
        *pszDst++ = *pszSrc++;
        cbDest -= sizeof(wchar_t);
    }
    *pszDst = 0;
    ttASSERT_MSG(!*pszSrc, "Buffer overflow!");
    return (*pszSrc ? EOVERFLOW : result);
}

size_t ttStrLen(const wchar_t* pwsz)
{
    if (pwsz)
    {
        size_t cch = wcslen(pwsz);
        // We use MAX_STRING_LEN as a max buffer size, so we need to divide by the size of wchar_t
        ttASSERT_MSG(cch < TT_MAX_STRING_LEN / sizeof(wchar_t), "String is too long!");
        if (cch > TT_MAX_STRING_LEN / sizeof(wchar_t))
            cch = TT_MAX_STRING_LEN / sizeof(wchar_t);
        return cch;
    }
    return 0;
}

wchar_t* ttstrchr(const wchar_t* psz, wchar_t ch)
{
    ttASSERT_MSG(psz, "NULL pointer!");
    if (!psz)
        return nullptr;
    while (*psz && *psz != ch)
        psz++;
    return (*psz ? (wchar_t*) psz : nullptr);
}

wchar_t* ttstrrchr(const wchar_t* psz, wchar_t ch)
{
    ttASSERT_MSG(psz, "NULL pointer!");
    if (!psz)
        return nullptr;

    wchar_t* pszLastFound = ttstrchr(psz, ch);
    if (pszLastFound)
    {
        for (;;)
        {
            psz = ttstrchr(pszLastFound + 1, ch);
            if (psz)
                pszLastFound = (wchar_t*) psz;
            else
                break;
        }
    }
    return (wchar_t*) pszLastFound;
}

wchar_t* ttstrstr(const wchar_t* pszMain, const wchar_t* pszSub)
{
    if (!pszMain || !pszSub)
        return nullptr;
    if (!*pszSub)
        return (wchar_t*) pszMain;  // matches what strstr does

    // We keep the first character of pszSub in first_ch. First we loop trying to find a match for this character. Once
    // we have found a match, we start with the second character of both pszMain and pszSub and walk through both strings
    // If we make it all the way through pszSub with matches, then we bail with a pointer to the string's location in
    // pszMain.

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
                if (!*pszTmp2)
                {  // end of substring, means we found a match
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

bool ttIsSameStrI(const wchar_t* psz1, const wchar_t* psz2)
{
    if (!psz1 || !psz2)
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

wchar_t* ttFindSpace(const wchar_t* psz)
{
    if (!psz)
        return nullptr;
    while (*psz && !ttIsWhitespace(*psz))
        psz++;
    return (wchar_t*) psz;
}

wchar_t* ttFindNonSpace(const wchar_t* psz)
{
    if (!psz)
        return nullptr;
    while (ttIsWhitespace(*psz))
        psz++;
    return (wchar_t*) psz;
}

wchar_t* ttFindExt(const wchar_t* pszPath, const wchar_t* pszExt)
{
    if (!pszPath || !pszExt)
        return nullptr;

    wchar_t* psz = ttstrrchr(pszPath, '.');
    return (psz && ttIsSameStrI(psz, pszExt)) ? psz : nullptr;
}

wchar_t* ttStepOver(const wchar_t* psz)
{
    if (!psz)
        return nullptr;
    while (*psz && *psz != L' ' && *psz != L'\t' && *psz != L'\r' && *psz != L'\n' && *psz != L'\f')  // step over all non whitespace
        ++psz;
    while (*psz == L' ' || *psz == L'\t' || *psz == L'\r' || *psz == L'\n' || *psz == L'\f')  // step over all whitespace
        ++psz;
    return (wchar_t*) psz;
}

wchar_t* ttItoa(int32_t val, wchar_t* pszDst, size_t cbDst)
{
    ttASSERT_MSG(pszDst, "NULL pointer!");
    ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
    if (!pszDst || cbDst < 3)
        return nullptr;

    wchar_t* pszRet = pszDst;
    if (val < 0)
    {
        *pszDst++ = '-';
        cbDst--;
    }

    tt::Utoa((uint32_t) val, pszDst, cbDst);
    return pszRet;
}

wchar_t* ttItoa(int64_t val, wchar_t* pszDst, size_t cbDst)
{
    ttASSERT_MSG(pszDst, "NULL pointer!");
    ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
    if (!pszDst || cbDst < 3)
        return nullptr;

    if (val < 0)
    {
        *pszDst++ = '-';
        cbDst--;
    }

    wchar_t* pszRet = pszDst;
    tt::Utoa((uint64_t) val, pszDst, cbDst);
    return pszRet;
}

wchar_t* ttUtoa(uint32_t val, wchar_t* pszDst, size_t cbDst)
{
    ttASSERT_MSG(pszDst, "NULL pointer!");
    ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
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
    ttASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

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
    ttASSERT_MSG(pszDst, "NULL pointer!");
    ttASSERT_MSG(cbDst > 2, "Buffer is too small!");
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
    ttASSERT_MSG(cbDst > 0, "Buffer supplied to Itoa is too small for the supplied integer!");

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

wchar_t* ttHextoa(size_t val, wchar_t* pszDst, bool bUpperCase)
{
    static wchar_t* szBuf = NULL;
    if (!pszDst)
    {
        if (!szBuf)
        {
            szBuf = (wchar_t*) ttMalloc(sizeof(size_t) * sizeof(wchar_t) + sizeof(wchar_t) * 4);  // extra room for null termination and general paranoia
        }
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
    ttASSERT_MSG(psz, "NULL pointer!");
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
