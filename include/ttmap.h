/////////////////////////////////////////////////////////////////////////////
// Name:		ttMap
// Purpose:		Class for storing key/value pairs
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTMAP_H__
#define __TTLIB_TTMAP_H__

#include "ttheap.h" // ttHeap

// As of 2018, ATL::CSimpleMap still uses int instead of size_t/ptrdiff_t resulting in a minor performance penalty when
// running on a 64 bit platform. In addition, if you do use size_t or ptrdiff_t, you have to cast it to (int) every time
// you use a CSimpleMap function.

// On Windows, this class uses a sub-heap for malloc() and strdup(). When the class is deleted, the entire sub-heap is
// deleted, rather then walking through and deleting each individual allocation.

// This class contains all the methods of CSimpleMap with the exception of Set() and Remove() -- i.e., you cannot remove
// or change a key/value pair once added to ttMap.

template <class TKey, class TVal>
class ttMap
{
public:
	typedef struct {
		TKey key;
		TVal val;
	} MAP_PAIR;

	ttMap() {
		m_cAllocated = 0;
		m_cItems = 0;
		m_aMapPairs = nullptr;
		m_pHeap = new ttHeap(true);
	}
	~ttMap() {
		delete m_pHeap;
	}

	void Add(const TKey key, const TVal val) {
		if (m_cItems >= m_cAllocated) {
			m_cAllocated += 32;	// add room for 32 items at a time
			m_aMapPairs = (MAP_PAIR*) m_pHeap->ttRealloc(m_aMapPairs, m_cAllocated * sizeof(MAP_PAIR));
		}
		m_aMapPairs[m_cItems].key = key;
		m_aMapPairs[m_cItems++].val = val;
	}
	const TKey GetKeyAt(ptrdiff_t pos) const {
		if (pos < 0 || pos >= m_cItems)
			return (TKey) nullptr;
		return m_aMapPairs[pos].key;
	}
	const TVal GetValueAt(ptrdiff_t pos) const {
		if (pos < 0 || pos >= m_cItems)
			return (TVal) nullptr;
		return m_aMapPairs[pos].val;
	}
	ptrdiff_t FindKey(const TKey key) const {
		for (ptrdiff_t pos = 0; pos < m_cItems; pos++)	{
			if (m_aMapPairs[pos].key == key)	{
				return pos;
			}
		}
		return -1;
	}

	ptrdiff_t FindVal(const TVal val) const {
		for (size_t pos = 0; pos < m_cItems; pos++)	{
			if (m_aMapPairs[pos].val == val)	{
				return pos;
			}
		}
		return -1;
	}
	TVal Lookup(const TKey key) const {
		for (ptrdiff_t pos = 0; pos < m_cItems; pos++)	{
			if (m_aMapPairs[pos].key == key)	{
				return m_aMapPairs[pos].val;
			}
		}
		return (TVal) NULL;
	}

	size_t GetCount() const { return m_cItems; }
	size_t GetSize()  const { return m_cItems; }	// for compatibility with CSimpleMap

	// The following functions can be used to allocate memory that won't have to be specifically freed -- it
	// will be freed automatically when the heap is destroyed in ttMap's destructor

	void  ttMalloc(size_t cb) { return m_pHeap->ttMalloc(cb); }
	char* ttStrdup(const char* psz) { return m_pHeap->ttStrdup(psz); }

	// Class members

protected:
	ttHeap*  m_pHeap;
	MAP_PAIR* m_aMapPairs;
	ptrdiff_t m_cAllocated;
	ptrdiff_t m_cItems;
};

#endif	// __TTLIB_TTMAP_H__
