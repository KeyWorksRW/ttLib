/////////////////////////////////////////////////////////////////////////////
// Name:		ttWString.h
// Purpose:		Wide-character string class. See ttString.h for SBCS version
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This could arguably be named CYetAnotherStringClass. This one provides the basic functionality you find in most
// string classes, but has the addition of some filename handling methods, the ability to printf() directly to the
// string, and some UI retrieving functions.

// The string is stored in a zero-terminated wchar_t buffer.

// The printf()/vprintf() thethods are similar to the CRT versions only these don't support precision, width, or padding
// These methods support c, C, d, u, x, X, s, S

// The following non-standard options are supported:
//
//		%kd - formats an integer with commas. I.e., 54321 would be formatted as 54,321
//		%kq - outputs quotation marks around the string
//		%ks - adds a 's' to the current buffer if the integer is zero or greater then 1, e.g., printf("item%ks", cItems);
//		%kS - adds a 's' to the current buffer if the __int64 is zero or greater then 1
//		%kt - formats a size_t value with commas
//		%ku - formats an unsigned integer with commas

//		%kI64d -- handles int64_t, adding commas if needed
//		%kI64u -- handles uint64_t, adding commas if needed

// The following are only valid when compiling for _WINDOWS_

//		%ke - formats a system message assuming the argument is an error number
//		%kr - argument is a resource identifier to a string

#pragma once

#include "ttstring.h"	// ttString

class ttWString
{
public:
	ttWString(void)	{ m_psz = nullptr; }
	ttWString(size_t cb) { m_psz = (wchar_t*) tt::malloc(cb); }
	ttWString(const wchar_t* psz) { m_psz = tt::strdup(psz ? psz : L""); }
	ttWString(const char* psz) { m_psz = nullptr; CopyNarrow(psz); }
#ifdef _WINDOWS_
	ttWString(HWND hwnd) { m_psz = nullptr; GetWindowText(hwnd); }
#endif // _WINDOWS_

	~ttWString() { if (m_psz)  tt::free(m_psz); }

	// Filename handling methods

	void	AppendFileName(const wchar_t* pszFile);
	void	AddTrailingSlash();	// adds a trailing forward slash if string doesn't already end with '/' or '\'
	void	ChangeExtension(const wchar_t* pszExtension);
	void	GetCWD();	// Caution: this will replace any current string
	void	RemoveExtension();

	const wchar_t* FindLastSlash();		// Handles any mix of '\' and '/' in the filename

#ifdef _WINDOWS_
	void GetFullPathName();
#endif

	// UI retrieving methods

#ifdef _WINDOWS_
	bool GetWindowText(HWND hwnd);

	// The following will always return a pointer, but if an error occurred, it will point to an empty string
	const wchar_t* GetListBoxText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, LB_GETCURSEL, 0, 0)); }
	const wchar_t* GetListBoxText(HWND hwnd, size_t sel);
	const wchar_t* GetResString(size_t idString);
#endif	// _WINDOWS_

	void	MakeLower();
	void	MakeUpper();

	// When compiled with wxWidgets, IsSame() functions work with UTF8 strings, otherwise they only correctly handle the ANSI portion of UTF8 strings

	bool	IsSameSubString(const wchar_t* psz);	// returns true if psz is an exact match to the first part of ttWString (case-insensitive)
	bool	IsSameString(const wchar_t* psz);		// returns true if psz is an exact match to ttWString (case-insensitive)

	wchar_t* GetQuotedString(wchar_t* pszQuote);	// returns pointer to first character after closing quote (or nullptr if not a quoted string)

	void __cdecl printf(const wchar_t* pszFormat, ...);
	void		 vprintf(const wchar_t* pszFormat, va_list argList);

	bool	 IsEmpty() const { return (m_psz ? (*m_psz ? false : true) : true); }
	bool	 IsNonEmpty() const { return (!IsEmpty()); }
	bool	 IsNull() const { return (m_psz == nullptr); }
	void	 Delete() { if (m_psz) { tt::free(m_psz); m_psz = nullptr; } }
	wchar_t* Enlarge(size_t cbTotalSize);	// increase buffer size if needed

	wchar_t* getptr() { return m_psz; }	// for when casting to char* is problematic

	operator const wchar_t*() const { return (const wchar_t*) m_psz; }
	operator wchar_t*() const { return (wchar_t*) m_psz; }
	operator void*() const { return (void*) m_psz; }

	void operator=(const char* psz);
	void operator=(const wchar_t* pwsz);

	void operator+=(const wchar_t* psz);
	void operator+=(wchar_t ch);
	void operator+=(ptrdiff_t val);
	void operator+=(ttWString csz);

	wchar_t operator[](int pos);

	bool operator==(const wchar_t* pwsz) { return (IsEmpty() || !pwsz) ? false : tt::samestr(m_psz, pwsz); }
	bool operator==(const ttWString* pwstr) { return (IsEmpty() || !pwstr || pwstr->IsEmpty()) ? false : tt::samestr(m_psz, *pwstr); }

	bool CopyNarrow(const char* psz);	// convert UTF8 to UNICODE and store it

protected:
	// Class functions

	const wchar_t* ProcessKFmt(const wchar_t* pszEnd, va_list* pargList);

	// Class members

	wchar_t*	 m_psz;		// using this name instead of m_pwsz to make it a tad easier to copy similar code form ttstring.cpp
};
