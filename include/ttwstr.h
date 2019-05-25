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

// The printf()/vprintf() thethods are similar to the CRT versions only these don't support precision, width, or padding
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

// The following are only valid when compiling for _WINDOWS_

//      %ke - formats a system message assuming the argument is an error number
//      %kr - argument is a resource identifier to a string

#pragma once

#include "ttstr.h"  // ttCStr

class ttCWStr
{
public:
    ttCWStr(void)   { m_psz = nullptr; }
    ttCWStr(size_t cb) { m_psz = (wchar_t*) ttMalloc(cb); } // Caution! cb is bytes, not wide chars
    ttCWStr(const wchar_t* pwsz) { m_psz = pwsz ? ttStrDup(pwsz) : nullptr; }
    ttCWStr(const char* psz) { m_psz = nullptr; if (psz) CopyNarrow(psz); }
    ttCWStr(ttCWStr& cwsz) { m_psz = cwsz.GetPtr() ? ttStrDup(cwsz.GetPtr()) : nullptr; }
#ifdef _WINDOWS_
    ttCWStr(HWND hwnd) { m_psz = nullptr; GetWindowText(hwnd); }
#endif // _WINDOWS_

    ~ttCWStr() { if (m_psz)  ttFree(m_psz); }

    // Filename handling methods

    void    AppendFileName(const wchar_t* pszFile);
    void    AddTrailingSlash(); // adds a trailing forward slash if string doesn't already end with '/' or '\'
    void    ChangeExtension(const wchar_t* pszExtension);
    void    GetCWD();   // Caution: this will replace any current string
    void    RemoveExtension();

    const wchar_t* FindLastSlash();     // Handles any mix of '\' and '/' in the filename

#ifdef _WINDOWS_
    void GetFullPathName();
#endif

    // UI retrieving methods

#ifdef _WINDOWS_
    const wchar_t* GetResString(size_t idString);
    bool GetWindowText(HWND hwnd);

    // The following will always return a pointer, but if an error occurred, it will point to an empty string
    const wchar_t* GetListBoxText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, LB_GETCURSEL, 0, 0)); }
    const wchar_t* GetListBoxText(HWND hwnd, size_t sel);
#endif  // _WINDOWS_

    void    MakeLower();
    void    MakeUpper();

    // When compiled with wxWidgets, IsSame() functions work with UTF8 strings, otherwise they only correctly handle the ANSI portion of UTF8 strings

    bool    IsSameSubString(const wchar_t* psz);    // returns true if psz is an exact match to the first part of ttCWStr (case-insensitive)
    bool    IsSameString(const wchar_t* psz);       // returns true if psz is an exact match to ttCWStr (case-insensitive)

    wchar_t* GetQuotedString(wchar_t* pszQuote);    // returns pointer to first character after closing quote (or nullptr if not a quoted string)

    void cdecl  printf(const wchar_t* pszFormat, ...);
    void        vprintf(const wchar_t* pszFormat, va_list argList);

    bool     IsEmpty() const { return (m_psz ? (*m_psz ? false : true) : true); }
    bool     IsNonEmpty() const { return (!IsEmpty()); }
    bool     IsNull() const { return (m_psz == nullptr); }
    void     Delete() { if (m_psz) { ttFree(m_psz); m_psz = nullptr; } }
    wchar_t* Enlarge(size_t cbTotalSize);   // increase buffer size if needed

    wchar_t* GetPtr() { return m_psz; } // for when casting to char* is problematic

    operator const wchar_t*() const { return (const wchar_t*) m_psz; }
    operator wchar_t*() const { return (wchar_t*) m_psz; }
    operator void*() const { return (void*) m_psz; }

    void operator=(const char* psz);
    void operator=(const wchar_t* pwsz);

    void operator+=(const wchar_t* psz);
    void operator+=(wchar_t ch);
    void operator+=(ptrdiff_t val);
    void operator+=(ttCWStr csz);

    wchar_t operator[](int pos);

    bool operator==(const wchar_t* pwsz) { return (IsEmpty() || !pwsz) ? false : ttIsSameStr(m_psz, pwsz); }
    bool operator==(const ttCWStr* pwstr) { return (IsEmpty() || !pwstr || pwstr->IsEmpty()) ? false : ttIsSameStr(m_psz, *pwstr); }

    bool CopyNarrow(const char* psz);   // convert UTF8 to UNICODE and store it

protected:
    // Class functions

    const wchar_t* ProcessKFmt(const wchar_t* pszEnd, va_list* pargList);

    // Class members

    wchar_t*     m_psz;     // using this name instead of m_pwsz to make it a tad easier to copy similar code form ttstring.cpp
};
