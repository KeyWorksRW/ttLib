/////////////////////////////////////////////////////////////////////////////
// Name:		ttstr.h
// Purpose:		SBCS string class. See ttwstr.h for wide-string equivalent
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

class ttCStr
{
public:
	ttCStr(void)	{ m_psz = nullptr; }
	ttCStr(size_t cb) { m_psz = (char*) tt::Malloc(cb); }
	ttCStr(const char* psz) { m_psz = tt::StrDup(psz); }
	ttCStr(const wchar_t* psz) { CopyWide(psz); }
	ttCStr(ttCStr& csz) { m_psz = tt::StrDup(csz); }
#ifdef _WINDOWS_
	ttCStr(HWND hwnd) { m_psz = nullptr; getWindowText(hwnd); }
#endif // _WINDOWS_

	~ttCStr() { if (m_psz) tt::FreeAlloc(m_psz); }

	// Method naming conventions are lower camel case when matching tt:: namespace functions

	char*	findExt(const char* pszExt) { return (char*) tt::findExt(m_psz, pszExt); }	// find filename extension
	char*	findStr(const char* psz) { return tt::findStr(m_psz, psz); }
	char*	findStri(const char* psz) { return tt::findStri(m_psz, psz); }
	char*	findChar(char ch) { return tt::findChar(m_psz, ch); }
	char*	findLastChar(char ch) { return tt::findLastChar(m_psz, ch); }

	size_t	strByteLen() { return m_psz ? tt::strByteLen(m_psz) : 0; }	// length of string in bytes including 0 terminator
	int		strCat(const char* psz);
	int		strCopy(const char* psz);
	size_t	strLen() { return m_psz ? tt::strLen(m_psz) : 0; }		// number of characters (use strByteLen() for buffer size calculations)

	bool	isSameStr(const char* psz) { return tt::isSameStr(m_psz, psz); }
	bool	isSameStri(const char* psz) { return tt::isSameStri(m_psz, psz); }
	bool	isSameSubStr(const char* psz) { return tt::isSameSubStr(m_psz, psz); }
	bool	isSameSubStri(const char* psz) { return tt::isSameSubStri(m_psz, psz); }

	char*	findExt() { return (char*) tt::findNonSpace(m_psz); }
	char*	findSpace() { return (char*) tt::findSpace(m_psz); }

	ptrdiff_t Atoi() { return tt::Atoi(m_psz); }

	char*	Itoa(int32_t val);
	char*	Itoa(int64_t val);
	char*	Utoa(uint32_t val);
	char*	Utoa(uint64_t val);

	void	trimRight() { tt::trimRight(m_psz); }

	bool	isEmpty() const { return (!m_psz || !*m_psz)  ? true : false; }
	bool	isNonEmpty() const { return (m_psz && *m_psz) ? true : false; }
	bool	isnull() const { return (m_psz == nullptr); }

	char* cdecl printf(size_t idFmtString, ...);	// retrieves the format string from the specified resource

	bool CopyWide(const wchar_t* pwsz);	// convert UNICODE to UTF8 and store it

	// Filename handling methods

	void	AppendFileName(const char* pszFile);
	void	AddTrailingSlash();	// adds a trailing forward slash if string doesn't already end with '/' or '\'
	void	ChangeExtension(const char* pszExtension);
	char*	getCWD();			// Caution: this will replace any current string
	void	RemoveExtension();
	bool	ReplaceStr(const char* pszOldText, const char* pszNewText, bool bCaseSensitive = false);

	char* findLastSlash();	// Handles any mix of '\' and '/' in the filename
	char* findExt() const;	// will return nullptr if no extension

#ifdef _WINDOWS_
	void getFullPathName();
#endif

	// UI retrieving methods

#ifdef _WINDOWS_
	char* getResString(size_t idString);
	bool  getWindowText(HWND hwnd);

	// The following will always return a pointer, but if an error occurred, it will point to an empty string
	char* getListBoxText(HWND hwnd) { return getListBoxText(hwnd, ::SendMessage(hwnd, LB_GETCURSEL, 0, 0)); }
	char* getListBoxText(HWND hwnd, size_t sel);
#endif	// _WINDOWS_

	void	MakeLower();
	void	MakeUpper();

	// if the first non whitespace character in pszString == chBegin, get everthing between chBegin and chEnd, otherwise get everything after the whitespace

	char*	getString(const char* pszString, char chBegin, char chEnd);

	char*	getAngleString(const char* pszString) { return getString(pszString,    '<', '>'); }
	char*	getBracketsString(const char* pszString) { return getString(pszString, '[', ']'); }
	char*	getParenthString(const char* pszString) { return getString(pszString,  '(', ')'); }

	char*	getQuotedString(const char* pszQuote);	// Handles single and double quote strings

	char* cdecl printf(const char* pszFormat, ...);			// Deletes any current string before printing
	char* cdecl printfAppend(const char* pszFormat, ...);	// Appends to the end of any current string

	void	resize(size_t cb);
	size_t	sizeBuffer() { return tt::SizeAlloc(m_psz); }	// returns 0 if m_psz is null
	void	Delete() { if (m_psz) { tt::FreeAlloc(m_psz); m_psz = nullptr; } }

	char*	getPtr() { return m_psz; }		// for when casting to char* is problematic
	char**	getPPtr() { return &m_psz; }	// use with extreme caution!

	operator char*() const { return (char*) m_psz; }
	operator void*() const { return (void*) m_psz; }

	void operator = (const char* psz);
	void operator = (const wchar_t* pwsz) { CopyWide(pwsz); };
	void operator = (ttCStr& csz) { *this = (char*) csz; }

	void operator += (const char* psz);
	void operator += (char ch);
	void operator += (ptrdiff_t val);

	char operator [] (int pos);
	char operator [] (size_t pos);

	bool operator == (const char* psz)	{ return (isEmpty() || !psz) ? false : tt::isSameStr(m_psz, psz); }
	bool operator == (char* psz)		{ return (isEmpty() || !psz) ? false : tt::isSameStr(m_psz, psz); }
	bool operator != (const char* psz)	{ return (isEmpty() || !psz) ? true  : !tt::isSameStr(m_psz, psz); }
	bool operator != (char* psz)		{ return (isEmpty() || !psz) ? true  : !tt::isSameStr(m_psz, psz); }

	// Use extreme caution about calling the Transfer functions!

	void	TransferTo(char** ppsz) { if (ppsz) { *ppsz = m_psz; m_psz = nullptr; } }		// Caller will be responsible for FreeAllocing memory
	void	TransferFrom(char** ppsz) { if (ppsz) { Delete(); m_psz = *ppsz; *ppsz = nullptr; } }
	void	TransferFrom(ttCStr cszTo) { Delete(); cszTo.TransferTo(&m_psz); }

protected:
	// Class members

	char*	 m_psz;
};

#endif // __TTLIB_CSTR_H__