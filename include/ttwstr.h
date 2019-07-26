/////////////////////////////////////////////////////////////////////////////
// Name:      ttwstr.h
// Purpose:   Wide-character string class. See ttstr.h for SBCS version
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This could arguably be named CYetAnotherStringClass. This one provides the basic functionality you find in most
// string classes, but has the addition of some filename handling methods, the ability to printf() directly to the
// string, and some UI retrieving functions.

// The string is stored in a zero-terminated wchar_t buffer.

// The printf()/vprintf() methods are similar to the CRT versions only these don't support precision, width, or padding
// These methods support c, C, d, u, x, X, s, S

// The following non-standard options are supported:
//
//      %kd - formats an integer with commas. I.e., 54321 would be formatted as 54,321
//      %kq - outputs quotation marks around the string
//      %ks - adds a 's' to the current buffer if the integer is zero or greater then 1, e.g., printf("item%ks", cItems);
//      %kS - adds a 's' to the current buffer if the __int64 is zero or greater then 1
//      %kt - formats a size_t value with commas
//      %ku - formats an unsigned integer with commas

//      %kI64d -- handles int64_t, adding commas if needed
//      %kI64u -- handles uint64_t, adding commas if needed

// The following are only valid when compiling for _WIN32

//      %ke - formats a system message assuming the argument is an error number
//      %kr - argument is a resource identifier to a string

#pragma once

#ifndef __TTLIB_CWSTR_H__
#define __TTLIB_CWSTR_H__

#include "ttheap.h"     // ttCHeap
#include "ttdebug.h"    // ttASSERT macros
#include "ttstr.h"  // ttCStr

#if defined(_WX_DEFS_H_)
    #include <wx/string.h>
#endif

class ttCWStr
{
public:
    ttCWStr(void)   { m_psz = nullptr; }
    ttCWStr(size_t cb) { m_psz = (wchar_t*) ttMalloc(cb); } // Caution! cb is bytes, not wide chars
    ttCWStr(const wchar_t* pwsz) { m_psz = pwsz ? ttStrDup(pwsz) : nullptr; }
    ttCWStr(ttCWStr& cwsz) { m_psz = cwsz.GetPtr() ? ttStrDup(cwsz.GetPtr()) : nullptr; }
    ttCWStr(const char* psz) { m_psz = nullptr; if (psz) CopyNarrow(psz); }
#if defined(_WX_DEFS_H_)
    ttCWStr(const wxString str) { m_psz = ttStrDup(str.wc_str()); }
 #endif
#if defined(_WIN32)
    ttCWStr(HWND hwnd) { m_psz = nullptr; GetWndText(hwnd); }
#endif

    ~ttCWStr() { if (m_psz)  ttFree(m_psz); }

    // Method naming conventions are lower camel case when matching tt:: namespace functions

    wchar_t*   FindExt(const wchar_t* pszExt) { return (wchar_t*) ttFindExt(m_psz, pszExt); } // find a specific filename extension
    wchar_t*   FindExt() const;                                                               // find any extension
    wchar_t*   FindStr(const wchar_t* psz) { return ttStrStr(m_psz, psz); }
    wchar_t*   FindStrI(const wchar_t* psz) { return ttStrStrI(m_psz, psz); }
    wchar_t*   FindChar(wchar_t ch) { return ttStrChr(m_psz, ch); }
    wchar_t*   FindLastChar(wchar_t ch) { return ttStrChrR(m_psz, ch); }

    size_t  StrByteLen() { return m_psz ? ttStrByteLen(m_psz) : 0; }    // length of string in bytes including 0 terminator
    int     StrCat(const wchar_t* psz);
    int     StrCopy(const wchar_t* psz);
    size_t  StrLen() { return m_psz ? ttStrLen(m_psz) : 0; }        // number of characters (use strByteLen() for buffer size calculations)

    bool    IsSameStr(const wchar_t* psz) { return ttIsSameStr(m_psz, psz); }
    bool    IsSameStrI(const wchar_t* psz) { return ttIsSameStrI(m_psz, psz); }
    bool    IsSameSubStr(const wchar_t* psz) { return ttIsSameSubStr(m_psz, psz); }
    bool    IsSameSubStrI(const wchar_t* psz) { return ttIsSameSubStrI(m_psz, psz); }

    wchar_t*   FindNonSpace() { return (wchar_t*) ttFindNonSpace(m_psz); }
    wchar_t*   FindSpace()    { return (wchar_t*) ttFindSpace(m_psz); }

    ptrdiff_t Atoi() { return ttAtoi(m_psz); }

    wchar_t*  Itoa(int32_t val);
    wchar_t*  Itoa(int64_t val);
    wchar_t*  Utoa(uint32_t val);
    wchar_t*  Utoa(uint64_t val);
    wchar_t*  Hextoa(size_t val, bool bUpperCase = false);

    void      TrimRight() { ttTrimRight(m_psz); }

    bool      IsEmpty() const { return (!m_psz || !*m_psz)  ? true : false; }
    bool      IsNonEmpty() const { return (m_psz && *m_psz) ? true : false; }
    bool      IsNull() const { return (m_psz == nullptr); }

    bool      CopyNarrow(const char* psz);   // convert UTF8 to UNICODE and store it

    bool      ReplaceStr(const wchar_t* pszOldText, const wchar_t* pszNewText, bool bCaseSensitive = false);

    // Filename handling methods

    void     AppendFileName(const wchar_t* pszFile);
    void     AddTrailingSlash(); // adds a trailing forward slash if string doesn't already end with '/' or '\'
    void     ChangeExtension(const wchar_t* pszExtension);
    wchar_t* GetCWD();   // Caution: this will replace any current string
    void     RemoveExtension();

    wchar_t* FindLastSlash();     // Handles any mix of '\' and '/' in the filename

    void FullPathName();

    // UI retrieving methods

#if defined(_WIN32)
    const wchar_t* GetResString(size_t idString);
    bool  GetWndText(HWND hwnd);

    // The following will always return a pointer, but if an error occurred, it will point to an empty string
    wchar_t* GetListBoxText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, LB_GETCURSEL, 0, 0)); }
    wchar_t* GetListBoxText(HWND hwnd, size_t sel);

    wchar_t* GetComboLBText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, CB_GETCURSEL, 0, 0)); }
    wchar_t* GetComboLBText(HWND hwnd, size_t sel);
#endif    // defined(_WIN32)

    void    MakeLower();
    void    MakeUpper();

    // if the first non whitespace character in pszString == chBegin, get everthing between chBegin and chEnd, otherwise get everything after the whitespace

    wchar_t* GetString(const wchar_t* pszString, wchar_t chBegin, wchar_t chEnd);

    wchar_t* GetAngleString(const wchar_t* pszString) { return GetString(pszString,    L'<', L'>'); }
    wchar_t* GetBracketsString(const wchar_t* pszString) { return GetString(pszString, L'[', L']'); }
    wchar_t* GetParenthString(const wchar_t* pszString) { return GetString(pszString,  L'(', L')'); }

    wchar_t* GetQuotedString(const wchar_t* pszQuote);    // returns pointer to first character after closing quote (or nullptr if not a quoted string)

    void     cdecl printf(const wchar_t* pszFormat, ...);
    wchar_t* cdecl printfAppend(const wchar_t* pszFormat, ...);

    void     ReSize(size_t cbTotalSize);   // increase buffer size if needed
    size_t   SizeBuffer() { return ttSize(m_psz); }  // returns 0 if m_psz is null
    void     Delete() { if (m_psz) { ttFree(m_psz); m_psz = nullptr; } }

    wchar_t*  GetPtr()  { return m_psz; } // for when casting to char* is problematic
    wchar_t** GetPPtr() { return &m_psz; }    // use with extreme caution!

    operator wchar_t*() const { return (wchar_t*) m_psz; }
    operator void*() const { return (void*) m_psz; }

    void operator=(const char* psz);
    void operator=(const wchar_t* pwsz);
    void operator = (ttCWStr& csz) { *this = (wchar_t*) csz; }

    void operator+=(const wchar_t* psz);
    void operator+=(wchar_t ch);
    void operator+=(ptrdiff_t val);

    wchar_t operator [] (int pos);
    wchar_t operator [] (size_t pos);

    bool operator == (const wchar_t* psz)  { return (IsEmpty() || !psz) ? false : ttIsSameStr(m_psz, psz); }
    bool operator == (wchar_t* psz)        { return (IsEmpty() || !psz) ? false : ttIsSameStr(m_psz, psz); }
    bool operator != (const wchar_t* psz)  { return (IsEmpty() || !psz) ? true  : !ttIsSameStr(m_psz, psz); }
    bool operator != (wchar_t* psz)        { return (IsEmpty() || !psz) ? true  : !ttIsSameStr(m_psz, psz); }

protected:
    // Class functions

    const wchar_t* ProcessKFmt(const wchar_t* pszEnd, va_list* pargList);

    // Class members

    wchar_t*     m_psz;     // using this name instead of m_pwsz to make it easier to copy similar code form ttstr.cpp
};

#endif // __TTLIB_CWSTR_H__
