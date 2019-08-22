/////////////////////////////////////////////////////////////////////////////
// Name:      ttCMap
// Purpose:   Header-only class for storing key/value pairs
// Author:    Ralph Walden
// Copyright: Copyright (c) 2018-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTMAP_H__
#define __TTLIB_TTMAP_H__

#include "ttheap.h"  // ttCHeap

// Caution! You MUST use ttCMap::ttMalloc or ttCMap::ttStrDup if you want allocated memory to be freed in the
// destructor.

// On Windows, this class uses a sub-heap for memory allocation. When the class is deleted, the entire sub-heap is
// deleted, rather then walking through and deleting each individual allocation.

template<class TKey, class TVal>
class ttCMap
{
public:
    typedef struct
    {
        TKey key;
        TVal val;
    } MAP_PAIR;

    ttCMap()
    {
        m_cAllocated = 0;
        m_cItems = 0;
        m_aMapPairs = nullptr;
        m_bCallersHeap = false;
        m_pHeap = new ttCHeap(true);
    }
    ttCMap(ttCHeap* pCallerHeap)
    {
        ttASSERT_MSG(pCallerHeap, "NULL pointer!");
        m_cAllocated = 0;
        m_cItems = 0;
        m_aMapPairs = nullptr;
        m_bCallersHeap = true;
        m_pHeap = pCallerHeap;
    }
    ~ttCMap()
    {
        if (!m_bCallersHeap)
            delete m_pHeap;
    }

    // Public functions

    int Add(const TKey key, const TVal val)
    {
        if (m_cItems >= m_cAllocated)
        {
            m_cAllocated += 8;  // number of items to add at a time
            m_aMapPairs = (MAP_PAIR*) m_pHeap->ttReAlloc(m_aMapPairs, m_cAllocated * sizeof(MAP_PAIR));
        }
        m_aMapPairs[m_cItems].key = key;
        m_aMapPairs[m_cItems].val = val;
        return m_cItems++;
    }
    const TKey GetKeyAt(int pos) const
    {
        if (pos < 0 || pos >= m_cItems)
            return (TKey) NULL;
        return m_aMapPairs[pos].key;
    }
    const TVal GetValueAt(int pos) const
    {
        if (pos < 0 || pos >= m_cItems)
            return (TVal) NULL;
        return m_aMapPairs[pos].val;
    }
    int FindKey(const TKey key) const
    {
        for (int pos = 0; pos < m_cItems; pos++)
        {
            if (m_aMapPairs[pos].key == key)
            {
                return pos;
            }
        }
        return -1;
    }

    int FindVal(const TVal val) const
    {  // Caution! You cannot use this if val is a string type
        for (size_t pos = 0; pos < m_cItems; pos++)
        {
            if (m_aMapPairs[pos].val == val)
            {
                return pos;
            }
        }
        return -1;
    }
    TVal Lookup(const TKey key) const
    {
        for (int pos = 0; pos < m_cItems; pos++)
        {
            if (m_aMapPairs[pos].key == key)
            {
                return m_aMapPairs[pos].val;
            }
        }
        return (TVal) NULL;
    }

    int GetCount() const { return m_cItems; }
    int GetSize() const { return m_cItems; }  // for compatibility with CSimpleMap

    // The following functions can be used to allocate memory that won't have to be specifically FreeAllocd -- it will
    // be FreeAllocd automatically when the heap is destroyed in ttCMap's destructor

    void  ttMalloc(size_t cb) { return m_pHeap->ttMalloc(cb); }
    char* ttStrdup(const char* psz) { return m_pHeap->ttStrdup(psz); }

private:
    // Class members

    ttCHeap*  m_pHeap;
    MAP_PAIR* m_aMapPairs;

    int  m_cItems;
    int  m_cAllocated;
    bool m_bCallersHeap;
};

#endif  // __TTLIB_TTMAP_H__
