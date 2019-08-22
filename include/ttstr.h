/////////////////////////////////////////////////////////////////////////////
// Name:      ttstr.h
// Purpose:   SBCS string class. See ttwstr.h for wide-string equivalent
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This could arguably be named CYetAnotherStringClass. This one provides the basic functionality you find in most
// string classes, but has the addition of some filename handling methods, the ability to printf() directly to the
// string, and some UI retrieving functions.

// The string is stored in a zero-terminated char buffer.

// The printf()/vprintf() methods are similar to the CRT versions only these don't support precision, width, or padding
// These methods support c, C, d, u, x, X, s, S

// The following non-standard options are supported:
//
//      %kd - formats an integer with commas. I.e., 54321 would be formatted as 54,321
//      %kq - outputs quotation marks around the string
//      %ks - adds a 's' to the current buffer if the integer is zero or greater then 1, e.g., printf("item%ks", cItems);
//      %kS - adds a 's' to the current buffer if the __int64 is zero or greater then 1
//      %kls - adds a 's' to the current buffer if the last numeric argument is zero or greater then 1 (printf("%d item%kls", cItems);
//      %kt - formats a size_t value with commas
//      %ku - formats an unsigned integer with commas

//      %kI64d -- handles int64_t, adding commas if needed
//      %kI64u -- handles uint64_t, adding commas if needed

// The following are only valid when compiling for _WIN32

//      %ke - formats a system message assuming the argument is an error number
//      %kr - argument is a resource identifier to a string

#pragma once

#ifndef __TTLIB_CSTR_H__
#define __TTLIB_CSTR_H__

#include "ttheap.h"   // ttCHeap
#include "ttdebug.h"  // ttASSERT macros

class ttCStr
{
public:
    ttCStr(void) { m_psz = nullptr; }
    ttCStr(size_t cb) { m_psz = (char*) ttMalloc(cb); }
    ttCStr(const char* psz) { m_psz = psz ? ttStrDup(psz) : nullptr; }
    ttCStr(ttCStr& csz) { m_psz = csz.GetPtr() ? ttStrDup(csz) : nullptr; }
    ttCStr(const wchar_t* pwsz)
    {
        m_psz = nullptr;
        if (pwsz)
            CopyWide(pwsz);
    }
#if defined(_WX_DEFS_H_)
    ttCStr(const wxString& str)
    {
        m_psz = ttStrDup(str.utf8_str());
    }
#endif
#if defined(_WIN32)
    ttCStr(HWND hwnd)
    {
        m_psz = nullptr;
        GetWndText(hwnd);
    }
#endif  // defined(_WIN32)

    ~ttCStr()
    {
        Delete();
    }

    // Method naming conventions are lower camel case when matching tt:: namespace functions

    char* FindExt(const char* pszExt) { return (char*) ttFindExt(m_psz, pszExt); }  // find a specific filename extension
    char* FindExt() const;                                                          // find any extension
    char* FindStr(const char* psz) { return ttStrStr(m_psz, psz); }
    char* FindStrI(const char* psz) { return ttStrStrI(m_psz, psz); }
    char* FindChar(char ch) { return ttStrChr(m_psz, ch); }
    char* FindLastChar(char ch) { return ttStrChrR(m_psz, ch); }

    size_t StrByteLen() { return m_psz ? ttStrByteLen(m_psz) : 0; }  // length of string in bytes including 0 terminator
    int    StrCat(const char* psz);
    int    StrCopy(const char* psz);
    size_t StrLen() { return m_psz ? ttStrLen(m_psz) : 0; }  // number of characters (use strByteLen() for buffer size calculations)

    bool IsSameStr(const char* psz) { return ttIsSameStr(m_psz, psz); }
    bool IsSameStrI(const char* psz) { return ttIsSameStrI(m_psz, psz); }
    bool IsSameSubStr(const char* psz) { return ttIsSameSubStr(m_psz, psz); }
    bool IsSameSubStrI(const char* psz) { return ttIsSameSubStrI(m_psz, psz); }

    char* FindNonSpace() { return (char*) ttFindNonSpace(m_psz); }
    char* FindSpace() { return (char*) ttFindSpace(m_psz); }

    ptrdiff_t Atoi() { return ttAtoi(m_psz); }

    char* Itoa(int32_t val);
    char* Itoa(int64_t val);
    char* Utoa(uint32_t val);
    char* Utoa(uint64_t val);
    char* Hextoa(size_t val, bool bUpperCase = false);

    void TrimRight() { ttTrimRight(m_psz); }

    bool IsEmpty() const { return (!m_psz || !*m_psz) ? true : false; }
    bool IsNonEmpty() const { return (m_psz && *m_psz) ? true : false; }
    bool IsNull() const { return (m_psz == nullptr); }

    bool CopyWide(const wchar_t* pwsz);  // convert UNICODE to UTF8 and store it

    bool ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive = false);

    // Filename handling methods

    char* AppendFileName(const char* pszFile);   // returns pointer to full string
    char* ReplaceFilename(const char* pszFile);  // returns pointer to full string

    void ChangeExtension(const char* pszExtension);
    void RemoveExtension();

    void  AddTrailingSlash();  // adds a trailing forward slash if string doesn't already end with '/' or '\'
    char* FindLastSlash();     // Handles any mix of '\' and '/' in the filename

    void  FullPathName();
    char* GetCWD();  // Caution: this will replace any current string

    // UI retrieving methods

#if defined(_WIN32)
    char* GetResString(size_t idString);
    bool  GetWndText(HWND hwnd);

    // The following will always return a pointer, but if an error occurred, it will point to an empty string
    char* GetListBoxText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, LB_GETCURSEL, 0, 0)); }
    char* GetListBoxText(HWND hwnd, size_t sel);

    char* GetComboLBText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, CB_GETCURSEL, 0, 0)); }
    char* GetComboLBText(HWND hwnd, size_t sel);

    char* cdecl printf(size_t idFmtString, ...);  // retrieves the format string from the specified resource
#endif                                            // defined(_WIN32)

    void MakeLower();
    void MakeUpper();

    bool GetEnv(const char* pszName);  // makes a copy of the specified environment variable

    // if the first non whitespace character in pszString == chBegin, get everthing between chBegin and chEnd,
    // otherwise get everything after the whitespace

    char* GetString(const char* pszString, char chBegin, char chEnd);

    char* GetAngleString(const char* pszString) { return GetString(pszString, '<', '>'); }
    char* GetBracketsString(const char* pszString) { return GetString(pszString, '[', ']'); }
    char* GetParenthString(const char* pszString) { return GetString(pszString, '(', ')'); }

    char* GetQuotedString(const char* pszQuote);  // Handles single and double quote strings

    char* cdecl printf(const char* pszFormat, ...);        // Deletes any current string before printing
    char* cdecl printfAppend(const char* pszFormat, ...);  // Appends to the end of any current string
    void cdecl WarningMsgBox(const char* pszFormat, ...);  // displays the formatted string in a warning message box

    void   ReSize(size_t cb);
    size_t SizeBuffer() { return ttSize(m_psz); }  // returns 0 if m_psz is null
    void   Delete()
    {
        if (m_psz)
        {
            ttFree(m_psz);
            m_psz = nullptr;
        }
    }

    char*  GetPtr() { return m_psz; }    // for when casting to char* is problematic
    char** GetPPtr() { return &m_psz; }  // use with extreme caution!

    operator char*() const { return (char*) m_psz; }
    operator void*() const { return (void*) m_psz; }

    void operator=(const char* psz);
    void operator=(const wchar_t* pwsz) { CopyWide(pwsz); };
    void operator=(ttCStr& csz) { *this = (char*) csz; }

    void operator+=(const char* psz);
    void operator+=(char ch);
    void operator+=(ptrdiff_t val);

    char operator[](int pos);
    char operator[](size_t pos);

    bool operator==(const char* psz) { return (IsEmpty() || !psz) ? false : ttIsSameStr(m_psz, psz); }
    bool operator==(char* psz) { return (IsEmpty() || !psz) ? false : ttIsSameStr(m_psz, psz); }
    bool operator!=(const char* psz) { return (IsEmpty() || !psz) ? true : !ttIsSameStr(m_psz, psz); }
    bool operator!=(char* psz) { return (IsEmpty() || !psz) ? true : !ttIsSameStr(m_psz, psz); }

    // Use extreme caution about calling the Transfer functions!

    void TransferTo(char** ppsz)
    {
        if (ppsz)
        {
            *ppsz = m_psz;
            m_psz = nullptr;
        }
    }  // Caller will be responsible for FreeAllocing memory
    void TransferFrom(char** ppsz)
    {
        if (ppsz)
        {
            Delete();
            m_psz = *ppsz;
            *ppsz = nullptr;
        }
    }
    void TransferFrom(ttCStr cszTo)
    {
        Delete();
        cszTo.TransferTo(&m_psz);
    }

private:
    // Class members

    char* m_psz;
};

#endif  // __TTLIB_CSTR_H__
