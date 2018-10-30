/////////////////////////////////////////////////////////////////////////////
// Name:        CStr.h
// Purpose:     SBCS string class. See cwstr.h for wide-string equivalent
// Author:      Ralph Walden
// Copyright:   Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
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

#include "ttheap.h"	// CTTHeap
#include "kstr.h"

class CStr
{
public:
	CStr(void)	{ m_psz = nullptr; }
	// CStr(size_t cb) { m_psz = (char*) kmalloc(cb); }
	CStr(const char* psz) { m_psz = kstrdup(psz); }
	CStr(const wchar_t* psz) { CopyWide(psz); }
	CStr(CStr& csz) { m_psz = kstrdup(csz); }
#ifdef _WINDOWS_
	CStr(HWND hwnd) { m_psz = nullptr; GetWindowText(hwnd); }
#endif // _WINDOWS_

	~CStr() { if (m_psz)  kfree(m_psz); }

	// Filename handling methods

	void 	AppendFileName(const char* pszFile);
	void 	AddTrailingSlash();	// adds a trailing forward slash if string doesn't already end with '/' or '\'
	void	ChangeExtension(const char* pszExtension);
	void	GetCWD();			// Caution: this will replace any current string
	void 	RemoveExtension();
	bool	ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive = false);

	const char* FindLastSlash();	// Handles any mix of '\' and '/' in the filename
	const char* FindExt() const;	// will return pointer to "" if no extension

#ifdef _WINDOWS_
	void GetFullPathName();
#endif

	// UI retrieving methods

#ifdef _WINDOWS_
	const char* __cdecl printf(size_t idFmtString, ...);	// retrieves the format string from the specified resource
	bool GetWindowText(HWND hwnd);

	// The following will always return a pointer, but if an error occurred, it will point to an empty string
	const char* GetListBoxText(HWND hwnd) { return GetListBoxText(hwnd, ::SendMessage(hwnd, LB_GETCURSEL, 0, 0)); }
	const char* GetListBoxText(HWND hwnd, size_t sel);
	const char* GetResString(size_t idString);
#endif	// _WINDOWS_

	void	MakeLower();
	void	MakeUpper();

	// When compiled with wxWidgets, IsSame() functions work with UTF8 strings, otherwise they only correctly handle the ANSI portion of UTF8 strings

	bool	IsSameSubString(const char* psz);	// returns true if psz is an exact match to the first part of CStr (case-insensitive)
	bool	IsSameString(const char* psz) { return ::IsSameString(m_psz, psz); } // returns true if psz is an exact match to CStr (case-insensitive)

	char*	GetQuotedString(const char* pszQuote);	// Handles `', '', "", <> -- copies the string inside and returns a pointer to it

	const char* __cdecl printf(const char* pszFormat, ...);			// Deletes any current string before printing
	const char* __cdecl printfAppend(const char* pszFormat, ...);	// Appends to the end of any current string

	/*
		Typical use for vprintf:

		void _cdecl MyFunc(const char* pszFormat, ...) {

			va_list args;
			va_start(args, pszFormat);

			CStr csz;
			csz.vprintf(pszFormat, args);
	*/

	void vprintf(const char* pszFormat, va_list argList);	// Appends to the end of any current string before printing

	bool	IsEmpty() const { return (m_psz ? (*m_psz ? false : true) : true); }
	bool	IsNonEmpty() const { return (!IsEmpty()); }
	void	Delete() { if (m_psz) { kfree(m_psz); m_psz = nullptr; } }
	char*	Enlarge(size_t cbTotalSize);	// increase buffer size if needed

	char*	getptr() { return m_psz; }		// for when casting to char* is problematic

	operator const char*() const { return (const char*) m_psz; }
	operator char*() const { return (char*) m_psz; }
	operator void*() const { return (void*) m_psz; }

	void operator=(const char* psz);
	void operator=(const wchar_t* pwsz) { CopyWide(pwsz); };
	void operator=(CStr& csz) { *this = (char*) csz; }

	void operator+=(const char* psz);
	void operator+=(char ch);
	void operator+=(ptrdiff_t val);

	char operator[](int pos);
	char operator[](size_t pos);

	bool operator==(const char* psz) { return (IsEmpty() || !psz) ? false : kstrcmp(m_psz, psz); }

	bool CopyWide(const wchar_t* pwsz);	// convert UNICODE to UTF8 and store it

protected:
	const char* ProcessKFmt(const char* pszEnd, va_list* pargList);

	// Class members

	char*	 m_psz;
};

#endif // __TTLIB_CSTR_H__