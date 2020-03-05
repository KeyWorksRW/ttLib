/////////////////////////////////////////////////////////////////////////////
// Name:      ttCStr
// Purpose:   SBCS string class
// Author:    Ralph Walden
// Copyright: Copyright (c) 2004-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <cassert>
#include <string>

#include "ttlibwin.h"    // Master header file for ttLibwin.lib
#include "ttdebug.h"  // ttASSERT macros
#include "ttstr.h"    // ttCStr

#if __cplusplus >= 201703L
    #include <filesystem>
#endif

#if defined(wxGUI)
    #include <wx/msgdlg.h>
#endif

using namespace ttch;  // used for the CH_ constants

namespace ttpriv
{
    void AddCommasToNumber(char* pszNum, char* pszDst, size_t cbDst);
}

#ifndef _MAX_U64TOSTR_BASE10_COUNT
    #define _MAX_U64TOSTR_BASE10_COUNT (20 + 1)
#endif

#define DEST_SIZE (ttSize(m_psz) - sizeof(char))

char* ttCStr::append(const char* psz)
{
    if (psz == nullptr)
        return m_psz;

    if (!m_psz)
        m_psz = ttStrDup(psz);
    else
    {
        size_t cbNew = std::strlen(psz) + sizeof(char);
        size_t cbOld = std::strlen(m_psz) + sizeof(char);
        m_psz = (char*) ttReAlloc(m_psz, cbNew + cbOld);
        std::strcpy(m_psz + (cbOld - 1), psz);
    }
    return m_psz;
}

size_t ttCStr::find(const char* psz) const
{
    if (!m_psz || !psz)
        return tt::npos;
    return tt::findstr_pos(m_psz, psz);
}

bool ttCStr::starts_with(const char* pszSub) const
{
    if (!m_psz || !pszSub)
        return false;
    return tt::issamesubstr(m_psz, pszSub);
}

char* ttCStr::AppendFileName(const char* pszFile)
{
    assert(pszFile);

    if (!pszFile || !*pszFile)
        return m_psz;

    if (!m_psz)  // no folder or drive to append to, so leave as is without adding slash
    {
        m_psz = ttStrDup(pszFile);  // REVIEW: [ralphw - 06-03-2018] We could prefix this with ".\"
        return m_psz;
    }

    if (*m_psz)
        AddTrailingSlash();
    *this += pszFile;
    return m_psz;
}

char* ttCStr::ReplaceFilename(const char* pszFile)
{
    assert(pszFile);

    if (!pszFile || !*pszFile)
        return m_psz;

    if (m_psz)
    {
        char* pszOld = ttFindFilePortion(m_psz);
        if (pszOld)
        {
            size_t cbNew = ttStrByteLen(pszFile);
            if (ttStrLen(pszOld) >= cbNew)
            {
                ttStrCpy(pszOld, pszFile);
                return m_psz;
            }

            *pszOld = 0;
            m_psz = (char*) ttReAlloc(m_psz, cbNew + ttStrByteLen(m_psz));
            // realloc may have changed the m_psz pointer, so we can't just copy to pszOld
            ttStrCat(m_psz, pszFile);
            return m_psz;
        }
        return AppendFileName(pszFile);  // Since we didn't have a filename, append to the end
    }
    else
    {
        m_psz = ttStrDup(pszFile);  // REVIEW: [ralphw - 06-03-2018] We could prefix this with ".\"
        return m_psz;
    }
}

void ttCStr::ChangeExtension(const char* pszExtension)
{
    assert(pszExtension);

    if (!pszExtension || !*pszExtension)
        return;

    if (!m_psz)
        m_psz = ttStrDup("");

    char* psz = ttStrChrR(m_psz, '.');
    if (psz && !(psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' ||
                 psz[1] == '/'))  // ignore .file, ./file, and ../file
        *psz = 0;                 // remove the extension if none of the above is true

    if (*pszExtension != '.')
        *this += ".";
    *this += pszExtension;
}

char* ttCStr::FindExt() const
{
    char* psz = ttStrChrR(m_psz, '.');
    if (!psz)
        return nullptr;
    if (psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' || psz[1] == '/')  // ignore .file, ./file, and ../file
        return nullptr;
    return psz;
}

void ttCStr::RemoveExtension()
{
    if (m_psz)
    {
        char* psz = ttStrChrR(m_psz, '.');
        if (psz)
        {
            if (psz == m_psz || *(psz - 1) == '.' || psz[1] == '\\' ||
                psz[1] == '/')  // ignore .file, ./file, and ../file
                return;
            *psz = 0;
        }
    }
}

void ttCStr::AddTrailingSlash()
{
    if (!m_psz)
    {
        m_psz = ttStrDup("/");
        return;
    }
    const char* pszLastSlash = FindLastSlash();
    if (!pszLastSlash || pszLastSlash[1])  // only add if there was no slash or there was something after the slash
        *this += "/";
}

char* ttCStr::FindLastSlash()
{
    assert(m_psz);

    if (!m_psz || !*m_psz)
        return nullptr;

    char* pszLastBackSlash = ttStrChrR(m_psz, '\\');
    char* pszLastFwdSlash = ttStrChrR(m_psz, '/');
    if (!pszLastBackSlash)
        return pszLastFwdSlash ? pszLastFwdSlash : nullptr;
    else if (!pszLastFwdSlash)
        return pszLastBackSlash ? pszLastBackSlash : nullptr;
    else
        return pszLastFwdSlash > pszLastBackSlash ? pszLastFwdSlash :
                                                    pszLastBackSlash;  // Impossible for them to be equal
}

char* ttCStr::GetCWD()
{
#if defined(_WIN32) && !defined(wxGUI)
    ReSize(MAX_PATH);
    DWORD cb = GetCurrentDirectoryA(MAX_PATH, m_psz);
    m_psz[cb] = 0;  // in case GetCurrentDirectory() failed
#else
    wxString str = wxGetCwd();
    const char* psz = str.utf8_str();
    if (!psz)
        m_psz = ttStrDup("./");  // in case getcwd() failed
    else
        m_psz = ttStrDup(psz);
#endif
    return m_psz;  // we leave the full buffer allocated in case you want to add a filename to the end
}

void ttCStr::FullPathName()
{
    assert(m_psz);
#if (defined(_WIN32)) && !defined(wxGUI)
    char szPath[MAX_PATH];
    ::GetFullPathNameA(m_psz, sizeof(szPath), szPath, NULL);
    ttStrDup(szPath, &m_psz);
#elif __cplusplus >= 201703L  // the following code requires C++17 or later
    std::filesystem::path path = (const char*) m_psz;
    std::filesystem::path fullPath = std::filesystem::absolute(path);
    ttFree(m_psz);
    wxString str(fullPath.c_str());
    m_psz = ttStrDup(str.utf8_str());
#endif                        // __cplusplus >= 201703L
}

void ttCStr::MakeLower()
{
    if (m_psz && *m_psz)
    {
        char* psz = m_psz;
        while (*psz)
        {
            *psz = (char) tolower(*psz);
            psz = (char*) ttNextChar(psz);  // handles utf8
        }
    }
}

void ttCStr::MakeUpper()
{
    if (m_psz && *m_psz)
    {
        char* psz = m_psz;
        while (*psz)
        {
            *psz = (char) toupper(*psz);
            psz = (char*) ttNextChar(psz);  // handles utf8
        }
    }
}

bool ttCStr::CopyWide(const wchar_t* pwsz)
{
    if (m_psz)
    {
        ttFree(m_psz);
        m_psz = nullptr;
    }

    assert(pwsz);

    if (!pwsz || !*pwsz)
    {
        m_psz = ttStrDup("");
        return false;
    }

#if defined(_WIN32) && !defined(wxGUI)
    size_t cb = wcslen(pwsz);
    assert(cb <= tt::MAX_STRING_LEN);

    int cbNew = WideCharToMultiByte(CP_UTF8, 0, pwsz, (int) cb, nullptr, 0, NULL, NULL);
    if (cbNew)
    {
        m_psz = (char*) ttMalloc(cbNew + sizeof(char));
        cb = WideCharToMultiByte(CP_UTF8, 0, pwsz, (int) cb, m_psz, cbNew, NULL, NULL);
        if (cb == 0)
            ttFree(m_psz);
        else
            m_psz[cb] = 0;
    }
    if (cbNew == 0 || cb == 0)
    {
        m_psz = ttStrDup("");
        return false;
    }
#else   // not defined(_WIN32)
    wxString str(pwsz);
    m_psz = ttStrDup(str.utf8_str());
#endif  // defined(_WIN32)

    return true;
}

void ttCStr::ReSize(size_t cbNew)
{
    assert(cbNew <= tt::MAX_STRING_LEN);
    if (cbNew > tt::MAX_STRING_LEN)
        cbNew = tt::MAX_STRING_LEN;

#if defined(_WIN32)
    size_t curSize = m_psz ? ttSize(m_psz) : 0;
    if (cbNew != curSize)
        m_psz = m_psz ? (char*) ttReAlloc(m_psz, cbNew) : (char*) ttMalloc(cbNew);
#else
    // until ttHeap gets ported, we won't know if malloc_size() for Mac or malloc_usable_size() for Unix is
    // available
    m_psz = m_psz ? (char*) ttReAlloc(m_psz, cbNew) : (char*) ttMalloc(cbNew);
#endif
}

bool ttCStr::ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive)
{
    assert(pszOldText);
    assert(*pszOldText);

    if (!pszOldText || !*pszOldText || !m_psz || !*m_psz)
        return false;
    if (!pszNewText)
        pszNewText = "";

    char* pszPos = bCaseSensitive ? ttStrStr(m_psz, pszOldText) : ttStrStrI(m_psz, pszOldText);
    if (!pszPos)
        return false;

    size_t cbOld = ttStrLen(pszOldText);
    size_t cbNew = ttStrLen(pszNewText);

    if (cbNew == 0)  // delete the old text since new text is empty
    {
        char* pszEnd = m_psz + ttStrByteLen(m_psz);
        ptrdiff_t cb = pszEnd - pszPos;
        memmove(pszPos, pszPos + cbOld, cb);
        m_psz = (char*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
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

        ttCStr cszTrail(pszPos);
        *pszPos = 0;
        m_psz = (char*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
        *this += pszNewText;
        *this += (char*) cszTrail;
    }
    else  // new text is shorter
    {
        cbOld -= cbNew;
        while (cbNew--)
            *pszPos++ = *pszNewText++;

        char* pszEnd = m_psz + ttStrByteLen(m_psz);
        ptrdiff_t cb = pszEnd - pszPos;
        memmove(pszPos, pszPos + cbOld, cb);
        m_psz = (char*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
    }
    return true;
}

void ttCStr::operator=(const char* psz)
{
    if (m_psz &&
        m_psz == psz)  // This can happen when getting a point to ttCStr and then assigning it to the same ttCStr
        return;

    ttStrDup(psz ? psz : "", &m_psz);
}

void ttCStr::operator+=(const char* psz)
{
    // Don't append our own string to ourself
    assert(m_psz != psz);
    if (m_psz && m_psz == psz)
        return;
    if (!m_psz)
        m_psz = ttStrDup(psz && *psz ? psz : "");
    else if (!psz || !*psz)
        return;  // nothing to add
    else
    {
        size_t cbNew = ttStrByteLen(psz);
        size_t cbOld = ttStrByteLen(m_psz);
        assert(cbNew + cbOld <= tt::MAX_STRING_LEN);
        if (cbNew + cbOld > tt::MAX_STRING_LEN)
            return;  // ignore it if it's too large
        m_psz = (char*) ttReAlloc(m_psz, cbNew + cbOld);
        ttStrCat(m_psz, psz);
    }
}

void ttCStr::operator+=(char ch)
{
    char szTmp[2];
    szTmp[0] = ch;
    szTmp[1] = 0;
    if (!m_psz)
        m_psz = ttStrDup(szTmp);
    else
    {
        m_psz = (char*) ttReAlloc(m_psz, ttStrByteLen(m_psz) + sizeof(char));  // include room for ch
        ttStrCat(m_psz, DEST_SIZE, szTmp);
    }
}

void ttCStr::operator+=(ptrdiff_t val)
{
    char szNumBuf[_MAX_U64TOSTR_BASE10_COUNT];
    ttItoa(val, szNumBuf, sizeof(szNumBuf));
    *this += szNumBuf;
}

char ttCStr::operator[](int pos)
{
    if (!m_psz || pos > (int) ttStrLen(m_psz))
        return 0;
    else
        return m_psz[pos];
}

char ttCStr::operator[](size_t pos)
{
    if (!m_psz || pos > ttStrLen(m_psz))
        return 0;
    else
        return m_psz[pos];
}

char* cdecl ttCStr::printfAppend(const char* pszFormat, ...)
{
    ttCStr csz;
    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(&csz.m_psz, pszFormat, argList);
    va_end(argList);

    *this += csz;

    return m_psz;
}

char* cdecl ttCStr::printf(const char* pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(&m_psz, pszFormat, argList);
    va_end(argList);
    return m_psz;
}

char* cdecl ttCStr::printf(size_t idFmtString, ...)
{
    ttCStr cszTmp;
    cszTmp.GetResString(idFmtString);

    va_list argList;
    va_start(argList, idFmtString);
    ttVPrintf(&m_psz, cszTmp, argList);
    va_end(argList);
    return m_psz;
}

void cdecl ttCStr::WarningMsgBox(const char* pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    ttVPrintf(&m_psz, pszFormat, argList);
    va_end(argList);
#if defined(_WIN32) && !defined(wxGUI)
    ttMsgBox(m_psz, MB_OK | MB_ICONWARNING);
#else
    wxMessageBox(m_psz, wxMessageBoxCaptionStr, wxOK | wxICON_WARNING);
#endif
}

int ttCStr::StrCat(const char* psz)
{
    assert(psz);

    if (psz == nullptr)
        return EINVAL;

    if (!m_psz)
        m_psz = ttStrDup(psz);
    else
    {
        size_t cbNew = ttStrByteLen(psz);
        size_t cbOld = ttStrByteLen(m_psz);
        assert(cbNew + cbOld <= tt::MAX_STRING_LEN);
        if (cbNew + cbOld > tt::MAX_STRING_LEN)
            return EOVERFLOW;  // ignore it if it's too large
        m_psz = (char*) ttReAlloc(m_psz, cbNew + cbOld);
        ttStrCat(m_psz, cbNew + cbOld, psz);
    }
    return 0;
}

int ttCStr::StrCopy(const char* psz)
{
    assert(psz);

    if (psz == nullptr)
        return EINVAL;

    if (!m_psz)
        m_psz = ttStrDup(psz);
    else
    {
        size_t cbNew = ttStrByteLen(psz);
        size_t cbOld = ttSize(m_psz);
        assert(cbNew + cbOld <= tt::MAX_STRING_LEN);
        if (cbNew + cbOld > tt::MAX_STRING_LEN)
            return EOVERFLOW;  // ignore it if it's too large
        ttStrDup(psz, &m_psz);
    }
    return 0;
}

char* ttCStr::Itoa(int32_t val)
{
    char szNum[32];
    ttItoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

char* ttCStr::Itoa(int64_t val)
{
    char szNum[32];
    ttItoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

char* ttCStr::Utoa(uint32_t val)
{
    char szNum[32];
    ttUtoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

char* ttCStr::Utoa(uint64_t val)
{
    char szNum[32];
    ttUtoa(val, szNum, sizeof(szNum));
    return ttStrDup(szNum, &m_psz);
}

char* ttCStr::Hextoa(size_t val, bool bUpperCase)
{
    char szNum[32];
    ttHextoa(val, szNum, bUpperCase);
    return ttStrDup(szNum, &m_psz);
}

char* ttCStr::GetString(const char* pszString, char chBegin, char chEnd)
{
    assert(pszString);

    Delete();  // current string, if any, should be deleted no matter what

    if (!pszString || !*pszString)
        return nullptr;

    size_t cb = ttStrByteLen(pszString);
    assert(cb <= tt::MAX_STRING_LEN);

    if (cb == 0 || cb > tt::MAX_STRING_LEN)
        return nullptr;
    else
    {
        m_psz = (char*) ttMalloc(cb);  // this won't return if it fails, so you will never get a nullptr on return
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
        const char* pszStart = pszString;
        while (*pszString != chEnd && *pszString)
            pszString = ttNextChar(pszString);
        strncpy_s(m_psz, DEST_SIZE, pszStart, pszString - pszStart);
        m_psz[pszString - pszStart] = 0;  // make certain it is null terminated
    }
    else
    {  // if the string didn't start with chBegin, so just copy the string
        ttStrCpy(m_psz, ttSize(m_psz), pszString);
        pszString += cb;
    }

    // If there is a significant size difference, then ReAllocate the memory

    if (cb > 32)  // don't bother ReAllocating if total allocation is 32 bytes or less
        m_psz = (char*) ttReAlloc(m_psz, ttStrByteLen(m_psz));
    return m_psz;
}

char* ttCStr::GetQuotedString(const char* pszQuote)
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
        case '"':  // CH_QUOTE
            return GetString(pszQuote, CH_QUOTE, CH_QUOTE);

        case '\'':  // CH_SQUOTE
            return GetString(pszQuote, CH_SQUOTE, CH_SQUOTE);

        case '`':  // CH_START_QUOTE
            return GetString(pszQuote, CH_START_QUOTE, CH_END_QUOTE);

        case '<':
            return GetString(pszQuote, '<', '>');

        case '[':  // CH_LEFT_BRACKET
            return GetString(pszQuote, '[', ']');

        case '(':  // CH_OPEN_PAREN
            return GetString(pszQuote, '[', ']');
    }
}

bool ttCStr::GetEnv(const char* pszName)
{
    assert(pszName);

    size_t cbEnv = 0;
    // an environment variable larger then 8k is almost certainly bogus and a security risk to use
    if (getenv_s(&cbEnv, nullptr, 0, pszName) == 0 && cbEnv > 0 && cbEnv < (8 * 1024))
    {
        ReSize(cbEnv + 1);
        if (getenv_s(&cbEnv, m_psz, cbEnv, pszName) == 0)
        {
            TrimRight();
            return true;
        }
    }

    return false;
}

///////////////////////////// Windows-only code /////////////////////////////////

#if defined(_WIN32)

char* ttCStr::GetListBoxText(HWND hwnd, size_t sel)
{
    if (m_psz)
        ttFree(m_psz);
    if (sel == (size_t) LB_ERR)
        m_psz = ttStrDup("");
    else
    {
        size_t cb = ::SendMessageA(hwnd, LB_GETTEXTLEN, sel, 0);
        assert(cb != (size_t) LB_ERR);
        if (cb != (size_t) LB_ERR)
        {
            m_psz = (char*) ttMalloc(cb + 1);
            ::SendMessageA(hwnd, LB_GETTEXT, sel, (LPARAM) m_psz);
        }
        else
        {
            m_psz = ttStrDup("");
        }
    }
    return m_psz;
}

char* ttCStr::GetComboLBText(HWND hwnd, size_t sel)
{
    if (m_psz)
        ttFree(m_psz);
    if (sel == (size_t) LB_ERR)
        m_psz = ttStrDup("");
    else
    {
        size_t cb = ::SendMessageA(hwnd, CB_GETLBTEXTLEN, sel, 0);
        assert(cb != (size_t) CB_ERR);
        if (cb != (size_t) CB_ERR)
        {
            m_psz = (char*) ttMalloc(cb + 1);
            ::SendMessageA(hwnd, CB_GETLBTEXT, sel, (LPARAM) m_psz);
        }
        else
        {
            m_psz = ttStrDup("");
        }
    }
    return m_psz;
}

/*
    tt::hinstResources is typically set by InitCaller() and determines where to load resources from. If you need to
    load the resources from a DLL, then first call:

        tt::hinstResources = LoadLibrary("dll name");
*/

char* ttCStr::GetResString(size_t idString)
{
    char szStringBuf[1024];

    if (tt::hinstResources == nullptr)
        tt::hinstResources = GetModuleHandle(NULL);

    if (LoadStringA(tt::hinstResources, (UINT) idString, szStringBuf, (int) sizeof(szStringBuf)) == 0)
    {
        ttCStr strMsg;
        strMsg.printf("Invalid string id: %zu", idString);
        ttFAIL_MSG(strMsg);
        if (m_psz)
            ttFree(m_psz);
        m_psz = ttStrDup("");
    }
    else
    {
        ttStrDup(szStringBuf, &m_psz);
    }
    return m_psz;
}

bool ttCStr::GetWndText(HWND hwnd)
{
    if (m_psz)
    {
        ttFree(m_psz);
        m_psz = nullptr;
    }

    assert(hwnd && IsWindow(hwnd));
    if (!hwnd || !IsWindow(hwnd))
    {
        m_psz = ttStrDup("");
        return false;
    }

    int cb = GetWindowTextLengthA(hwnd);
    assert(cb <= (int) tt::MAX_STRING_LEN);

    if (cb == 0 || cb > (int) tt::MAX_STRING_LEN)
    {
        m_psz = ttStrDup("");
        return false;
    }

    char* psz = (char*) ttMalloc(cb + sizeof(char));
    cb = ::GetWindowTextA(hwnd, psz, cb + sizeof(char));
    if (cb == 0)
    {
        m_psz = ttStrDup("");
        ttFree(psz);
        return false;
    }
    else
        m_psz = psz;
    return true;
}

#endif  // defined(_WIN32)
