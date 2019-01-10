/////////////////////////////////////////////////////////////////////////////
// Name:		ttList, ttDblList
// Purpose:		String arrays based off a private heap
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2005-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// All the classes here use their own private heap. The advantage of this approach is that none of the individual allocations need to be freed -- the entire heap is destroyed																																																																								//
// when the class destructor is called. By default, all the classes use a non-serialized private heap -- which means multiple threads need to use a crit section if accessing
// a shared class.

#pragma once

#ifndef __TTLIB_STRLIST_H__
#define __TTLIB_STRLIST_H__

#include "ttheap.h"		// ttHeap
#include "hashpair.h"	// ttHashPair
#include "ttstring.h"		// ttString

class ttList : public ttHeap
{
public:
	ttList(bool bSerialize = true);		// true makes the class thread safe
	ttList(HANDLE hHeap);

	enum {
		FLG_ADD_DUPLICATES	= 1 << 0,	// add the string even if it has already been added
		FLG_IGNORE_CASE		= 1 << 1,
		FLG_URL_STRINGS		= 1 << 2,	// string case is ignored, '\' and '/' are considered the same
	};

	// SetFlags() will not change any previously added strings

	void SetFlags(size_t flags);	// any combination of FLG_PREVENT_DUPLICATES | FLG_IGNORE_CASE | FLG_URL_STRINGS
	void AllowDuplicates() { SetFlags(FLG_ADD_DUPLICATES); }

	size_t	Add(const char* psz);
	bool	Find(const char* psz) const;
	size_t	GetPos(const char* psz) const;	// returns -1 if not found
	void	InsertAt(size_t pos, const char* psz);
	void	Remove(const char* psz);
	void	Remove(size_t pos);
	void	Replace(size_t pos, const char* psz);
	void	Swap(size_t posA, size_t posB);

	size_t	GetCount() const { return m_cItems; }
	bool	IsEmpty() const { return m_cItems == 0; }
	void	Delete();	// deletes all strings

	const char* Get(size_t pos) const;

	// CSimpleArray/CAtlArray equivalents

	size_t	GetSize() const { return GetCount(); }
	void	RemoveAll() { Delete(); }
	void	RemoveAt(size_t pos) { Remove(pos); }
	void	SetAt(size_t pos, const char* psz) { Replace(pos, psz); }

	const char* GetAt(size_t pos) const { return Get(pos); }

	// End CSimpleArray/CAtlArray equivalents

	/*
		Example usage:

			ttList lst;
			// ... add a bunch of strings
			lst.BeginEnum();
			while(lst.Enum())
				cout << lst.EnumValue();
	 */

	void BeginEnum() { m_enum = 0; }
	bool Enum();
	const char* EnumValue();	// returns pointer to last enumerated string, or nullptr if no string found
	bool Enum(const char** ppszResult);	// use this if you want to receive a pointer to the string

	void Sort();				// sort strings into alphabetical order

	// Use the following with caution! All strings MUST have at least iColumn characters!

	void Sort(size_t iColumn);	// sort strings using the offset (equivalent to strcmp(str1 + iColumn, str2 + iColumn)

	void operator+=(const char* psz) { Add(psz); }
	const char* operator[](size_t pos) const;

protected:
	void inline swap(ptrdiff_t pos1, ptrdiff_t pos2)
	{
		const char* pszTmp = m_aptrs[pos1];
		m_aptrs[pos1] = m_aptrs[pos2];
		m_aptrs[pos2] = pszTmp;
	}
	void qsorti(ptrdiff_t low, ptrdiff_t high);
	void qsortCol(ptrdiff_t low, ptrdiff_t high);

	const char* NormalizeString(const char* pszFileName, ttString& cszKey) const;

	bool isNoDuplicates() const { return (m_flags & FLG_ADD_DUPLICATES) ? false : true; }

	// Class members

	size_t m_SortColumn;
	bool   m_bSerialize;

	const char** m_aptrs;
	size_t m_cAllocated;
	size_t m_cItems;
	size_t m_enum;

	ttHashPair m_HashPair;
	ttString m_cszKey;
	size_t m_flags;
};

// Holds a pair of strings, referenced as Key and Val. Note that unlike ttList, the default behaviour is to allow
// duplicate keys

class ttDblList : public ttHeap
{
public:
	void IgnoreCase() { m_bIgnoreCase = true; }	// ignore case when searching for keys or values
	void PreventDuplicateKeys();				// key won't be added if it already exists

	ttDblList(bool bSerialize = false);
	~ttDblList();

	void Add(const char* pszKey, const char* pszVal);

	bool		FindKey(const char* pszKey, size_t* ppos = nullptr) const;
	bool		FindVal(const char* pszVal, size_t* ppos = nullptr) const;
	const char* GetKeyAt(size_t pos) const;
	const char* GetValAt(size_t pos) const;
	const char* GetMatchingVal(const char* pszKey) const;

	const char* GetValueAt(size_t pos) const { return GetValAt(pos); }	// to match CSimpleArray::GetValueAt

	void Replace(size_t pos, const char* pszKey, const char* pszVal);
	size_t inline GetCount() const { return m_cItems; }

	void SortKeys();
	void SortVals();

protected:
	void inline swap(ptrdiff_t pos1, ptrdiff_t pos2)
	{
		char* pszKey = m_aptrs[pos1].pszKey;
		char* pszVal = m_aptrs[pos1].pszVal;
		m_aptrs[pos1].pszKey = m_aptrs[pos2].pszKey;
		m_aptrs[pos2].pszKey = pszKey;
		m_aptrs[pos1].pszVal = m_aptrs[pos2].pszVal;
		m_aptrs[pos2].pszVal = pszVal;
	}
	void qsorti(ptrdiff_t low, ptrdiff_t high);

	typedef struct {
		char* pszKey;
		char* pszVal;
	} DBLPTRS;

	// Class members

	DBLPTRS* m_aptrs;
	size_t	 m_cAllocated;
	size_t	 m_cItems;

	bool m_bSortKeys;
	bool m_bSerialize;
	bool m_bIgnoreCase;

	ttHashPair* m_pHashLookup;
};

// Similar to ttDblList only the val is an array of ptrdiff_t instead of a char*

class ttStrIntList : public ttHeap
{
public:
	typedef struct {
		char*	   pszKey;
		ptrdiff_t* pVal;
	} DBLPTRS;

	ttStrIntList(bool bSerialize = false);

	/*
		If the string already exists, but the Val doesn't, Add() will add the Val to the array of Vals
		associated with the string. I.e.,

		Add("my key", 1);
		Add("my key", 1);	// nothing will be added because 1 is already associated with "my key"
		Add("my key", 2);	// Now both 1 and 2 are associated with "my key"
	*/

	void	Add(const char* pszKey, ptrdiff_t newVal);
	bool	Add(size_t posKey, ptrdiff_t newVal);	// Add vals to an existing key
	bool	FindKey(const char* pszKey, size_t* ppos = nullptr) const;
	size_t	GetCount() const { return m_cItems; }	// returns the number of keys actually added (duplicates are not added)

	// To get all the numbers, either use Enum() or call these GetVal routines. Be aware that any pointer to
	// the values is only valid for as long as no additional values are added to the key

	bool GetValCount(const char* pszKey, ptrdiff_t* pVal) const;
	ptrdiff_t GetValCount(size_t posKey) const;

	bool GetVal(const char* pszKey, ptrdiff_t* pVal, size_t posVal = 0) const;
	bool GetVal(size_t posKey, ptrdiff_t* pVal, size_t posVal = 0) const;
	const char* GetKey(size_t posKey) const;

	/*
		You can enumerate through all the numbers assigned to a string using code
		similar to the following:

		ttStrIntList strList;
		// add some string/number pairs
		if (strList.BeginEnum("my key")) {
			ptrdiff_t num;
			while (strList.Enum(&num)) {
				// do something with num
			}
		}
	*/

	bool BeginEnum(const char* pszKey);
	bool Enum(ptrdiff_t* pVal);

protected:
	// Class members

	DBLPTRS* m_aptrs;
	size_t	 m_cAllocated;
	size_t	 m_cItems;

	size_t	  m_posEnumKey;
	ptrdiff_t m_posEnumVal;

	bool m_bSerialize;
};

#endif	// __TTLIB_STRLIST_H__
