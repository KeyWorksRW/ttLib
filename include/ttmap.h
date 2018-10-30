/////////////////////////////////////////////////////////////////////////////
// Name:		CTTMap
// Purpose:		Class for storing key/value pairs
// Author:		Ralph Walden
// Copyright:   Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTMAP_H__
#define __TTLIB_TTMAP_H__

// As of 2018, ATL::CSimpleMap still uses int instead of size_t/ptrdiff_t resulting in a minor performance
// penalty when running on a 64 bit platform. In addition, if you do use size_t or ptrdiff_t, you have to
// cast it to (int) every time you use a CSimpleMap function.

// On Windows, this class uses a sub-heap for malloc() and strdup(). When the class is deleted, the entire
// sub-heap is deleted, rather then walking through and deleting each individual allocation.

// This class contains all the methods of CSimpleMap with the exception of Set() and Remove() -- i.e., you
// cannot remove or change a key/value pair once added to CTTMap.

// Support is built in for finding keys or values of type char* key or val. Other key or value types are
// searched for using a == comparison.

// Because the sub-heap is created non-serialized (because allocations are faster that way), this class is
// not thread-safe -- i.e., you can't have two threads accessing it at the same time unless they put their
// own critical section around any access to the class.

template <class TKey, class TVal>
class CTTMap
{
public:
	typedef struct {
		TKey key;
		TVal val;
	} MAP_PAIR;

	CTTMap() {
		m_cAllocated = 0;
		m_cItems = 0;
		m_aMapPairs = nullptr;
		m_pHeap = new CTTHeap(true);
	}
	~CTTMap() {
		delete m_pHeap;
	}

	void Add(const TKey key, const TVal val) {
		if (m_cItems >= m_cAllocated) {
			m_cAllocated += 32;	// add room for 32 items at a time
			m_aMapPairs = (MAP_PAIR*) m_pHeap->realloc(m_aMapPairs, m_cAllocated * sizeof(MAP_PAIR));
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
	// will be freed automatically when the heap is destroyed in CTTMap's destructor

	void  malloc(size_t cb) { return m_pHeap->malloc(cb); }
	char* strdup(const char* psz) { return m_pHeap->strdup(psz); }

	// Class members

protected:
	CTTHeap*  m_pHeap;
	MAP_PAIR* m_aMapPairs;
	ptrdiff_t m_cAllocated;
	ptrdiff_t m_cItems;
};

#endif	// __TTLIB_TTMAP_H__