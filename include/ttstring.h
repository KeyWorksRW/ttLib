/////////////////////////////////////////////////////////////////////////////
// Name:		ttstring.h
// Purpose:		SBCS string class. See ttwstring.h for wide-string equivalent
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This could arguably be named CYetAnotherStringClass. This one provides the basic functionality you find in most
// string classes, but has the addition of some filename handling methods, the ability to printf() directly to the
// string, and some UI retrieving functions.

// The string is stored in a zero-terminated char buffer.

// The printf()/vprintf() methods are similar to the CRT versions only these don't support precision, width, or padding
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

#ifndef __TTLIB_CSTR_H__
#define __TTLIB_CSTR_H__

#include "ttheap.h" // ttHeap
#include "ttstr.h"	// various functions dealing with strings

class ttString
{
public:
	ttString(void)	{ m_psz = nullptr; }
	// ttString(size_t cb) { m_psz = (char*) tt::malloc(cb); }
	ttString(const char* psz) { m_psz = tt::strdup(psz); }
	ttString(const wchar_t* psz) { CopyWide(psz); }
	ttString(ttString& csz) { m_psz = tt::strdup(csz); }
#ifdef _WINDOWS_
	ttString(HWND hwnd) { m_psz = nullptr; GetWindowText(hwnd); }
#endif // _WINDOWS_

	~ttString() { if (m_psz) tt::free(m_psz); }

	bool CopyWide(const wchar_t* pwsz);	// convert UNICODE to UTF8 and store it

	// Filename handling methods

	void	AppendFileName(const char* pszFile);
	void	AddTrailingSlash();	// adds a trailing forward slash if string doesn't already end with '/' or '\'
	void	ChangeExtension(const char* pszExtension);
	void	GetCWD();			// Caution: this will replace any current string
	void	RemoveExtension();
	bool	ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive = false);

	char* FindLastSlash();	// Handles any mix of '\' and '/' in the filename
	char* FindExt() const;	// will return nullptr if no extension

#ifdef _WINDOWS_
	void GetFullPathName();
#endif

	// UI retrieving methods

#ifdef _WINDOWS_
	char* __cdecl printf(size_t idFmtString, ...);	// retrieves the format string from the specified resource
	bool GetWindowText(HWND hwnd);

	// The following will always return a pointer, but if an error occurred, it will point to an empty string
	char* GetListBoxText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, LB_GETCURSEL, 0, 0)); }
	char* GetListBoxText(HWND hwnd, size_t sel);
	char* GetResString(size_t idString);
#endif	// _WINDOWS_

	void	MakeLower();
	void	MakeUpper();

	// When compiled with wxWidgets, IsSame() functions work with UTF8 strings, otherwise they only correctly handle the ANSI portion of UTF8 strings

	bool	samesubstri(const char* psz) { return tt::samesubstri(m_psz, psz); }
	bool	samestri(const char* psz) { return tt::samestri(m_psz, psz); }

	char*	GetQuotedString(const char* pszQuote);	// Handles `', '', "", <> -- copies the string inside and returns a pointer to it

	char* cdecl printf(const char* pszFormat, ...);			// Deletes any current string before printing
	char* cdecl printfAppend(const char* pszFormat, ...);	// Appends to the end of any current string

	bool	IsEmpty() const { return (m_psz ? (*m_psz ? false : true) : true); }
	bool	IsNonEmpty() const { return (!IsEmpty()); }
	bool	IsNull() const { return (m_psz == nullptr); }
	void	Delete() { if (m_psz) { tt::free(m_psz); m_psz = nullptr; } }
	char*	Enlarge(size_t cbTotalSize);	// increase buffer size if needed

	char*	getptr() { return m_psz; }		// for when casting to char* is problematic


	operator char*() const { return (char*) m_psz; }
	operator void*() const { return (void*) m_psz; }

	void operator = (const char* psz);
	void operator = (const wchar_t* pwsz) { CopyWide(pwsz); };
	void operator = (ttString & csz) { *this = (char*) csz; }

	void operator += (const char* psz);
	void operator += (char ch);
	void operator += (ptrdiff_t val);

	char operator [] (int pos);
	char operator [] (size_t pos);

	bool operator == (const char* psz)	{ return (IsEmpty() || !psz) ? false : tt::samestr(m_psz, psz); }
	bool operator == (char* psz)		{ return (IsEmpty() || !psz) ? false : tt::samestr(m_psz, psz); }

protected:
	// Class members

	char*	 m_psz;
};

#endif // __TTLIB_CSTR_H__