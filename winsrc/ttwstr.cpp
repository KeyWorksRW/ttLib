/////////////////////////////////////////////////////////////////////////////
// Name:      ttCWStr
// Purpose:   Wide-character string class
// Author:    Ralph Walden
// Copyright: Copyright (c) 2004-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#if defined(NDEBUG)
    #pragma comment(lib, "ttLibwin.lib")
#else
    #pragma comment(lib, "ttLibwinD.lib")
#endif

#include <direct.h>  // for _getcwd

#include "../include/ttdebug.h"  // for ttASSERTS
#include "../include/ttwstr.h"
#include "../include/ttstr.h"  // ttCStr

#ifndef _MAX_U64TOSTR_BASE10_COUNT
#define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

using namespace ttch;  // used for the CH_ constants

namespace ttpriv
{
    void AddCommasToNumber(wchar_t* pszNum, wchar_t* pszDst, size_t cbDst);
}

// Note that the limit here of 16 megs is smaller then the ttstr functions that use 16m

#define DEST_SIZE (ttSize(m_psz) - sizeof(wchar_t))

void ttCWStr::AppendFileName(const wchar_t* pszFile)
{
    assert(pszFile);

    if (!pszFile || !*pszFile)
        return;

    if (!m_psz)  // no folder or drive to append to, so leave as is without adding slash
    {
        m_psz = ttStrDup(pszFile);  // REVIEW: [ralphw - 06-03-2018] We could prefix this with ".\"
        return;
    }

    AddTrailingSlash();
    *this += pszFile;
}

wchar_t* ttCWStr::FindExt() const
{
    wchar_t* psz = ttStrChrR(m_psz, '.');
    if (!psz)
        return nullptr;
    if (psz == m_psz || *(psz - 1) == L'.' || psz[1] == L'\\' || psz[1] == L'/')  // ignore .file, ./file, and ../file
        return nullptr;
    return psz;
}

void ttCWStr::ChangeExtension(const wchar_t* pszExtension)
{
    assert(pszExtension);

    if (!pszExtension || !*pszExtension)
        return;

    if (!m_psz)
        m_psz = ttStrDup(L"");

    wchar_t* pszEnd = ttStrChrR(m_psz, L'.');
    // handle "./foo" -- don't assume the leading period is an extension if there's a folder seperator after it
    if (pszEnd && pszEnd[1] != CHW_FORWARDSLASH && pszEnd[1] != CHW_BACKSLASH)
        *pszEnd = 0;

    if (*pszExtension != L'.')
        *this += L".";
    *this += pszExtension;
}

void ttCWStr::RemoveExtension()
{
    if (m_psz)
    {
        wchar_t* psz = ttStrChrR(m_psz, L'.');
        if (psz)
        {
            if (psz == m_psz || *(psz - 1) == L'.' || psz[1] == L'\\' || psz[1] == L'/')  // ignore .file, ./file, and ../file
                return;
            *psz = 0;
        }
    }
}

void ttCWStr::AddTrailingSlash()
{
    if (!m_psz)
    {
        m_psz = ttStrDup(L"/");
        return;
    }
    wchar_t* pszLastSlash = FindLastSlash();
    if (!pszLastSlash || pszLastSlash[1])  // only add if there was no slash or there was something after the slash
        *this += L"/";
}

wchar_t* ttCWStr::FindLastSlash()
{
    assert(m_psz);

    if (!m_psz || !*m_psz)
        return nullptr;

    wchar_t* pszLastBackSlash = ttStrChrR(m_psz, L'\\');
    wchar_t* pszLastFwdSlash = ttStrChrR(m_psz, L'/');
    if (!pszLastBackSlash)
        return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
    else if (!pszLastFwdSlash)
        return pszLastBackSlash ? pszLastBackSlash : nullptr;
    else
        return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash : pszLastBackSlash;  // Impossible for them to be equal
}

wchar_t* ttCWStr::GetCWD()
{
#if defined(_WIN32)
    ReSize(MAX_PATH);
    DWORD cb = GetCurrentDirectoryW(MAX_PATH, m_psz);
    m_psz[cb] = 0;  // in case GetCurrentDirectory() failed
#else
    wxString    str = wxGetCwd();
    const char* psz = str.wc_str();
    if (!psz)
        m_psz = ttStrDup(L"./");  // in case getcwd() failed
    else
        m_psz = ttStrDup(psz);
#endif
    return m_psz;  // we leave the full buffer allocated in case you want to add a filename to the end
}

#if defined(_WIN32)

void ttCWStr::FullPathName()
{
    ttASSERT(m_psz);
    wchar_t szPath[MAX_PATH];
    ::GetFullPathNameW(m_psz, sizeof(szPath), szPath, NULL);
    ttFree(m_psz);
    m_psz = ttStrDup(szPath);
}

wchar_t* ttCWStr::GetListBoxText(HWND hwnd, size_t sel)
{
    if (m_psz)
        ttFree(m_psz);
    if (sel == (size_t) LB_ERR)
        m_psz = ttStrDup(L"");
    else
    {
        size_t cb = ::SendMessageW(hwnd, LB_GETTEXTLEN, sel, 0);
        ttASSERT(cb != (size_t) LB_ERR);
        if (cb != (size_t) LB_ERR)
        {
            m_psz = (wchar_t*) ttMalloc(cb + 1);
            ::SendMessageW(hwnd, LB_GETTEXT, sel, (LPARAM) m_psz);
        }
        else
        {
            m_psz = ttStrDup(L"");
        }
    }
    return m_psz;
}

wchar_t* ttCWStr::GetComboLBText(HWND hwnd, size_t sel)
{
    if (m_psz)
        ttFree(m_psz);
    if (sel == (size_t) LB_ERR)
        m_psz = ttStrDup(L"");
    else
    {
        size_t cb = ::SendMessageW(hwnd, CB_GETLBTEXTLEN, sel, 0);
        ttASSERT(cb != (size_t) CB_ERR);
        if (cb != (size_t) CB_ERR)
        {
            m_psz = (wchar_t*) ttMalloc(cb + 1);
            ::SendMessageW(hwnd, CB_GETLBTEXT, sel, (LPARAM) m_psz);
        }
        else
        {
            m_psz = ttStrDup(L"");
        }
    }
    return m_psz;
}

/*
    tt::hinstResources is typically set by InitCaller() and determines where to load resources from. If you need to
    load the resources from a DLL, then first call:

        tt::hinstResources = LoadLibrary("dll name");
*/

const wchar_t* ttCWStr::GetResString(size_t idString)
{
    static wchar_t szStringBuf[1024];

    if (LoadStringW(tt::hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0)
    {
        ttCStr cszMsg;
        cszMsg.printf("Invalid string id: %zu", idString);
        ttFAIL(cszMsg);
        if (m_psz)
            ttFree(m_psz);
        m_psz = ttStrDup(L"");
    }
    else
    {
        if (m_psz)
            ttFree(m_psz);
        m_psz = ttStrDup(szStringBuf);
    }
    return m_psz;
}

bool ttCWStr::GetWndText(HWND hwnd)
{
    if (m_psz)
    {
        ttFree(m_psz);
        m_psz = nullptr;
    }

    ttASSERT_MSG(hwnd && IsWindow(hwnd), "Invalid window handle");
    if (!hwnd || !IsWindow(hwnd))
    {
        m_psz = ttStrDup(L"");
        return false;
    }

    int cb = GetWindowTextLengthW(hwnd);
    ttASSERT_MSG(cb <= (int) tt::MAX_STRING_LEN, "String is over 16 megs in size!");

    if (cb == 0 || cb > (int) tt::MAX_STRING_LEN)
    {
        m_psz = ttStrDup(L"");
        return false;
    }

    wchar_t* psz = (wchar_t*) ttMalloc(cb + sizeof(wchar_t));
    cb = ::GetWindowTextW(hwnd, psz, cb);
    if (cb == 0)
    {
        m_psz = ttStrDup(L"");
        ttFree(psz);
        return false;
    }
    else
        m_psz = psz;
    return true;
}

#endif  // defined(_WIN32)

void ttCWStr::MakeLower()
{
    if (m_psz && *m_psz)
    {
        wchar_t* psz = m_psz;
        while (*psz)
        {
            *psz = (char) towlower(*psz);
            ++psz;
        }
    }
}

void ttCWStr::MakeUpper()
{
    if (m_psz && *m_psz)
    {
        wchar_t* psz = m_psz;
        while (*psz)
        {
            *psz = (char) towupper(*psz);
            ++psz;
        }
    }
}

bool ttCWStr::CopyNarrow(const char* psz)
{
    if (m_psz)
        ttFree(m_psz);

    assert(psz);

    if (!psz || !*psz)
    {
        m_psz = ttStrDup(L"");
        return false;
    }

    size_t cch = ttStrLen(psz);
    ttASSERT_MSG(cch <= tt::MAX_STRING_LEN, "String is over 16 megs in size!");

#if defined(_WIN32)
    int cbNew = MultiByteToWideChar(CP_UTF8, 0, psz, (int) cch, nullptr, 0);
    if (cbNew)
    {
        m_psz = (wchar_t*) ttMalloc(cbNew * sizeof(wchar_t) + sizeof(wchar_t));
        cch = MultiByteToWideChar(CP_UTF8, 0, psz, (int) cch, m_psz, cbNew);
        if (cch == 0)
            ttFree(m_psz);
        else
            m_psz[cch] = 0;
    }
    if (cbNew == 0 || cch == 0)
    {
        m_psz = ttStrDup(L"");
        return false;
    }
#else   // not defined(_WIN32)
    wxString str(psz);
    m_psz = ttStrDup(str.wc_str());
#endif  // defined(_WIN32)

    return true;
}

void ttCWStr::ReSize(size_t cbNew)
{
    ttASSERT(cbNew <= tt::MAX_STRING_LEN);
    if (cbNew > tt::MAX_STRING_LEN)
        cbNew = tt::MAX_STRING_LEN;

    size_t curSize = m_psz ? ttSize(m_psz) : 0;
    if (cbNew != curSize)
        m_psz = m_psz ? (wchar_t*) ttReAlloc(m_psz, cbNew) : (wchar_t*) ttMalloc(cbNew);
}

void ttCWStr::operator=(const char* psz)
{
    ttASSERT_MSG(psz, "null pointer!");
    ttASSERT_MSG(*psz, "empty string!");

    CopyNarrow(psz);
}

void ttCWStr::operator=(const wchar_t* psz)
{
    assert(psz);
    ttASSERT_MSG(m_psz != psz, "Attempt to assign ttCWStr to itself");

    if (m_psz && m_psz == psz)
        return;

    if (m_psz)
        ttFree(m_psz);

    m_psz = ttStrDup(psz ? psz : L"");
}

void ttCWStr::operator+=(const wchar_t* psz)
{
    ttASSERT_MSG(m_psz != psz, "Attempt to append string to itself!");
    if (m_psz && m_psz == psz)
        return;
    if (!m_psz)
        m_psz = ttStrDup(psz && *psz ? psz : L"");
    else if (!psz || !*psz)
        m_psz = ttStrDup(L"");
    else
    {
        size_t cbNew = ttStrByteLen(psz);
        size_t cbOld = ttStrByteLen(m_psz);
        ttASSERT_MSG(cbNew + cbOld <= tt::MAX_STRING_LEN, "String is over 16 megs in size!");
        if (cbNew + cbOld > tt::MAX_STRING_LEN)
            return;  // ignore it if it's too large
        m_psz = (wchar_t*) ttReAlloc(m_psz, cbNew + cbOld);
        memcpy(m_psz + ((cbOld - sizeof(wchar_t)) / sizeof(wchar_t)), psz, cbNew);
    }
}

void ttCWStr::operator+=(wchar_t ch)
{
    wchar_t szTmp[2];
    szTmp[0] = ch;
    szTmp[1] = 0;
    if (!m_psz)
        m_psz = ttStrDup(szTmp);
    else
    {
        m_psz = (wchar_t*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
        ttStrCat(m_psz, DEST_SIZE, szTmp);
    }
}

void ttCWStr::operator+=(ptrdiff_t val)
{
    wchar_t szNumBuf[_MAX_U64TOSTR_BASE10_COUNT];
    ttItoa(val, szNumBuf, sizeof(szNumBuf));
    *this += szNumBuf;
}

wchar_t ttCWStr::operator[](int pos)
{
    if (!m_psz || pos > (int) ttStrLen(m_psz))
        return 0;
    else
        return m_psz[pos];
}

wchar_t ttCWStr::operator[](size_t pos)
{
    if (!m_psz || pos > ttStrLen(m_psz))
        return 0;
    else
        return m_psz[pos];
}

void __cdecl ttCWStr::printf(const wchar_t* pszFormat, ...)
{
    ttCStr cszFormat(pszFormat), cszResult;

    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(cszResult.GetPPtr(), (char*) cszFormat, argList);
    va_end(argList);

    CopyNarrow(cszResult);
}

wchar_t* cdecl ttCWStr::printfAppend(const wchar_t* pszFormat, ...)
{
    ttCStr cszFormat(pszFormat), cszResult;

    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(cszResult.GetPPtr(), (char*) cszFormat, argList);
    va_end(argList);

    if (!m_psz)
        CopyNarrow(cszResult);
    else
    {
        ttCWStr cszWide(cszResult);
        *this += cszWide;
    }

    return m_psz;
}

const wchar_t* ttCWStr::ProcessKFmt(const wchar_t* pszEnd, va_list* pargList)
{
    wchar_t szwBuf[256];
    szwBuf[0] = L'\0';
    switch (*pszEnd)
    {
        case 'n':  // 'n' is deprecated, 'd' should be used instead
        case 'd':
            ttItoa((int) va_arg(*pargList, int), szwBuf, sizeof(szwBuf));
            ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
            break;

        case 'I':  // 64-bit version of 'd' and 'u' that works in 32-bit builds
            if (ttIsSameSubStrI(pszEnd, L"I64d"))
                ttItoa(va_arg(*pargList, int64_t), szwBuf, sizeof(szwBuf));
            else if (ttIsSameSubStrI(pszEnd, L"I64u"))
                ttUtoa(va_arg(*pargList, uint64_t), szwBuf, sizeof(szwBuf));
            ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
            pszEnd += 3;  // skip over I64 portion, then count normally
            break;

        case 't':  // use for size_t parameters, this will handle both 32 and 64 bit compilations
            ttUtoa(va_arg(*pargList, size_t), szwBuf, sizeof(szwBuf));
            ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
            break;

        case 'u':
            ttUtoa(va_arg(*pargList, unsigned int), szwBuf, sizeof(szwBuf));
            ttpriv::AddCommasToNumber(szwBuf, szwBuf, sizeof(szwBuf));
            break;

        case 's':
            if (va_arg(*pargList, int) != 1)
            {
                szwBuf[0] = 's';
                szwBuf[1] = '\0';
            }
            break;

        case 'S':
            if (va_arg(*pargList, _int64) != 1)
            {
                szwBuf[0] = 's';
                szwBuf[1] = '\0';
            }
            break;

#if defined(_WIN32)
        case 'r':
        {
            ttCWStr cszRes;
            cszRes.GetResString(va_arg(*pargList, int));
            ttStrCpy(szwBuf, sizeof(szwBuf), cszRes);
        }
        break;

        case 'e':
        {
            wchar_t* pszMsg;

            FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                           NULL, va_arg(*pargList, int), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (wchar_t*) &pszMsg, 0, NULL);
            ttStrCpy(szwBuf, sizeof(szwBuf), pszMsg);
            LocalFree((HLOCAL) pszMsg);
        }
        break;
#endif  // defined(_WIN32)

        case 'q':
            try
            {
                const wchar_t* psz = va_arg(*pargList, const wchar_t*);
                size_t         cb = ttStrLen(m_psz) + ttStrLen(psz) + (3 * sizeof(wchar_t));
                if (cb > ttSize(m_psz))
                {
                    cb >>= 7;
                    cb <<= 7;
                    cb += 0x80;  // round up to 128
                    m_psz = (wchar_t*) ttReAlloc(m_psz, cb);
                }
                ttStrCat(m_psz, L"\042");
                ttStrCat(m_psz, psz);
                ttStrCat(m_psz, L"\042");
            }
            catch (...)
            {
                ttFAIL("Exception in printf -- bad %%kq pointer");
            }
            break;
    }
    if (szwBuf[0])
    {
        size_t cbCur = ttStrByteLen(m_psz);
        size_t cb = cbCur + ttStrByteLen(szwBuf);
        if (cb > ttSize(m_psz))
        {
            cb >>= 7;
            cb <<= 7;
            cb += 0x80;  // round up to 128
            m_psz = (wchar_t*) ttReAlloc(m_psz, cb);
        }
        ttStrCat(m_psz, DEST_SIZE - cbCur, szwBuf);
    }
    return pszEnd + 1;
}

// We allow for pszNum and pszDst to be different in case the pszNum buffer is only large
// enough to hold the number and not the commas

void ttpriv::AddCommasToNumber(wchar_t* pszNum, wchar_t* pszDst, size_t cbDst)
{
    size_t cchDst = cbDst / sizeof(wchar_t);
    if (pszDst != pszNum)
        ttStrCpy(pszDst, cbDst, pszNum);  // copy the number, performa all additional work in-place in the destination buffer

    size_t cchNum = ttStrLen(pszDst);  // needs to be signed because it can go negative
    if (cchNum < 4)
    {
        ttASSERT(cchNum * sizeof(wchar_t) < cbDst);
        return;
    }
    ttASSERT(cchNum + (cchNum / 3) < cchDst);
    if (cchNum + (cchNum / 3) >= cchDst)
        return;

    if (*pszDst == '-')
    {
        --cchNum;
        if (cchNum < 4)
            return;
    }

    size_t cchStart = cchNum % 3;
    if (cchStart == 0)
        cchStart += 3;
    while (cchStart < cchNum)
    {
        memmove(pszDst + cchStart + 1, pszDst + cchStart, ttStrByteLen(pszDst + cchStart) + sizeof(wchar_t));  // make space for a comma
        pszDst[cchStart] = ',';
        ++cchNum;       // track that we added a comma for loop comparison
        cchStart += 4;  // 3 numbers plus the comma
    }
}

int ttCWStr::StrCat(const wchar_t* psz)
{
    ttASSERT_MSG(psz, "NULL pointer!");

    if (psz == nullptr)
        return EINVAL;

    if (!m_psz)
        m_psz = ttStrDup(psz);
    else
    {
        size_t cbNew = ttStrByteLen(psz);
        size_t cbOld = ttStrByteLen(m_psz);
        ttASSERT_MSG(cbNew + cbOld <= tt::MAX_STRING_LEN, "String is over 16 megs in size!");
        if (cbNew + cbOld > tt::MAX_STRING_LEN)
            return EOVERFLOW;  // ignore it if it's too large
        m_psz = (wchar_t*) ttReAlloc(m_psz, cbNew + cbOld);
        ttStrCat(m_psz, cbNew + cbOld, psz);
    }
    return 0;
}

int ttCWStr::StrCopy(const wchar_t* psz)
{
    ttASSERT_MSG(psz, "NULL pointer!");

    if (psz == nullptr)
        return EINVAL;

    if (!m_psz)
        m_psz = ttStrDup(psz);
    else
    {
        size_t cbNew = ttStrByteLen(psz);
        size_t cbOld = ttSize(m_psz);
        ttASSERT_MSG(cbNew + cbOld <= tt::MAX_STRING_LEN, "String is over 16 megs in size!");
        if (cbNew + cbOld > tt::MAX_STRING_LEN)
            return EOVERFLOW;  // ignore it if it's too large
        ttStrDup(psz, &m_psz);
    }
    return 0;
}

wchar_t* ttCWStr::Itoa(int32_t val)
{
    wchar_t szNum[32];
    ttItoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

wchar_t* ttCWStr::Itoa(int64_t val)
{
    wchar_t szNum[32];
    ttItoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

wchar_t* ttCWStr::Utoa(uint32_t val)
{
    wchar_t szNum[32];
    ttUtoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

wchar_t* ttCWStr::Utoa(uint64_t val)
{
    wchar_t szNum[32];
    ttUtoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

wchar_t* ttCWStr::Hextoa(size_t val, bool bUpperCase)
{
    wchar_t szNum[32];
    ttHextoa(val, szNum, bUpperCase);
    return ttStrDup(szNum, &m_psz);
}

bool ttCWStr::ReplaceStr(const wchar_t* pszOldText, const wchar_t* pszNewText, bool bCaseSensitive)
{
    ttASSERT_MSG(pszOldText, "NULL pointer!");
    ttASSERT(*pszOldText);

    if (!pszOldText || !*pszOldText || !m_psz || !*m_psz)
        return false;
    if (!pszNewText)
        pszNewText = L"";

    wchar_t* pszPos = bCaseSensitive ? ttStrStr(m_psz, pszOldText) : ttStrStrI(m_psz, pszOldText);
    if (!pszPos)
        return false;

    size_t cbOld = ttStrLen(pszOldText);
    size_t cbNew = ttStrLen(pszNewText);

    if (cbNew == 0)  // delete the old text since new text is empty
    {
        wchar_t*  pszEnd = m_psz + ttStrByteLen(m_psz);
        ptrdiff_t cb = pszEnd - pszPos;
        memmove(pszPos, pszPos + cbOld, cb);
        m_psz = (wchar_t*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
    }
    else if (cbNew == cbOld)
    {
        while (*pszNewText)  // copy and return
            *pszPos++ = *pszNewText++;
    }
    else if (cbNew > cbOld)
    {
        while (cbOld--)  // replace the old, insert what's left
            *pszPos++ = *pszNewText++;

        ttCWStr cszTrail(pszPos);
        *pszPos = 0;
        m_psz = (wchar_t*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
        *this += pszNewText;
        *this += (wchar_t*) cszTrail;
    }
    else  // new text is shorter
    {
        cbOld -= cbNew;
        while (cbNew--)
            *pszPos++ = *pszNewText++;

        wchar_t*  pszEnd = m_psz + ttStrByteLen(m_psz);
        ptrdiff_t cb = pszEnd - pszPos;
        memmove(pszPos, pszPos + cbOld, cb);
        m_psz = (wchar_t*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
    }
    return true;
}

wchar_t* ttCWStr::GetString(const wchar_t* pszString, wchar_t chBegin, wchar_t chEnd)
{
    assert(pszString);

    Delete();  // current string, if any, should be deleted no matter what

    if (!pszString || !*pszString)
        return nullptr;

    size_t cb = ttStrByteLen(pszString);
    ttASSERT_MSG(cb <= tt::MAX_STRING_LEN, "String is over 16 megs in size!");

    if (cb == 0 || cb > tt::MAX_STRING_LEN)
        return nullptr;
    else
    {
        m_psz = (wchar_t*) ttMalloc(cb);  // this won't return if it fails, so you will never get a nullptr on return
        *m_psz = 0;
    }

    // step over any leading whitespace unless whitespace starts a separator
    if (!ttIsWhitespace(chBegin))
    {
        while (ttIsWhitespace(*pszString))
            ++pszString;
    }

    if (*pszString == chBegin)
    {
        pszString++;
        const wchar_t* pszStart = pszString;
        while (*pszString != chEnd && *pszString)
            ++pszString;
        wcsncpy(m_psz, pszStart, pszString - pszStart);
        m_psz[pszString - pszStart] = 0;  // make certain it is null terminated
    }
    else
    {  // if the string didn't start with chBegin, just copy the string
        ttStrCpy(m_psz, ttSize(m_psz), pszString);
        pszString += cb;
    }

    // If there is a significant size difference, then ReAllocate the memory

    if (cb > 32)  // don't bother ReAllocating if total allocation is 32 bytes or less
        m_psz = (wchar_t*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
    return m_psz;
}

wchar_t* ttCWStr::GetQuotedString(const wchar_t* pszQuote)
{
    assert(pszQuote);

    if (!pszQuote || !*pszQuote)
    {
        Delete();  // any current string should be deleted no matter what
        return nullptr;
    }

    while (ttIsWhitespace(*pszQuote))  // step over any leading whitespace
        ++pszQuote;

    switch (*pszQuote)
    {
        default:
        case CHW_QUOTE:
            return GetString(pszQuote, CHW_QUOTE, CHW_QUOTE);

        case L'\'':  // CH_SQUOTE
            return GetString(pszQuote, CH_SQUOTE, CH_SQUOTE);

        case L'`':  // CH_START_QUOTE
            return GetString(pszQuote, CH_START_QUOTE, CH_END_QUOTE);

        case L'<':
            return GetString(pszQuote, '<', '>');

        case L'[':  // CH_LEFT_BRACKET
            return GetString(pszQuote, '[', ']');

        case L'(':  // CH_OPEN_PAREN
            return GetString(pszQuote, '[', ']');
    }
}
