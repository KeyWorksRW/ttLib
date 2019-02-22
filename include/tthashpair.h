/////////////////////////////////////////////////////////////////////////////
// Name:		ttHashPair
// Purpose:		Class utilizing an array of HASH numbers and an associated value
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This class stores an array of hash numbers and an associated value. The array
// is kept sorted based on the hash number in order to increase lookup speed.

#pragma once

#ifndef __TTLIB_HASHPAIR_H__
#define __TTLIB_HASHPAIR_H__

#include "../include/ttdebug.h"	// ASSERTs

class ttHashPair
{
public:
	ttHashPair(size_t EstimatedMembers = 0);
	~ttHashPair();

	typedef struct {
		size_t hash;
		size_t val;
	} HASH_PAIR;

	// Class functions

	void	Add(size_t hash, size_t val = 0);
	void	Add(const char* psz, size_t val = 0) { Add(tt::HashFromSz(psz), val); }
	bool	Find(size_t hash) const;
	bool	Find(const char* psz) const { return Find(tt::HashFromSz(psz)); }
	size_t	GetVal(size_t hash) const;		// returns -1 if not found
	size_t	GetVal(const char* psz) const { return GetVal(tt::HashFromSz(psz)); }
	void	Remove(size_t hash);
	void	Remove(const char* psz) { Remove(tt::HashFromSz(psz)); }
	void	SetVal(size_t hash, size_t val);
	void	SetVal(const char* psz, size_t val) { SetVal(tt::HashFromSz(psz), val); }

	void	Delete();	// remove all hash/val pairs

	// The URL variants are case-insensitive, and forward/back slashes are considered the same.
	// E.g., foo\bar and Foo/bar will generate the same hash number

	bool	FindURL(const char* pszURL) const { return Find(tt::HashFromURL(pszURL)); }
	void	AddURL(const char* pszURL, size_t val = 0) { Add(tt::HashFromURL(pszURL), val); }
	size_t	GetUrlVal(const char* pszURL) const { return GetVal(tt::HashFromURL(pszURL)); }
	void	RemoveURL(const char* pszURL) { Remove(tt::HashFromURL(pszURL)); }

#ifdef _DEBUG
	void Verify() {	// make certain the hash numbers are in numerical order
		for (size_t pos = 1; pos < m_cMembers; pos++)
			ttASSERT(m_pahash[pos].hash > m_pahash[pos - 1].hash);
	}
#endif

	size_t GetCount() const { return m_cMembers; }
	HASH_PAIR* GetArray() { return m_pahash; }	// Use with caution!

protected:

	HASH_PAIR* FindInsertionPoint(size_t hash) const;
	HASH_PAIR* GetHashPair(size_t hash) const;

	// Class members

	size_t m_cMembers;
	size_t m_cMalloced;
	HASH_PAIR* m_pahash;
};

#endif // __TTLIB_HASHPAIR_H__
