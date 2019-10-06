/////////////////////////////////////////////////////////////////////////////
// Name:      ttCHeapArray
// Purpose:   Simple templated array
// Author:    Ralph Walden
// Copyright: Copyright (c) 2010-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

/*

    Similar to ttCArray, only this one uses a sub-heap so that you can allocate memory for strings and other data, and
    have all of it freed when the sub-heap is released.

*/

#pragma once

#include "ttheap.h"   // ttCHeap
#include "ttdebug.h"  // ttASSERT macros

// A simple header-only array of any type

template<typename T> class ttCHeapArray : public ttCHeap
{
public:
    ttCHeapArray()
        : ttCHeap(true)
    {
        m_cAllocated = m_cItems = 0;
        m_cGrowth = 8;
        m_aData = (T*) nullptr;
    }

    void Add(const T t)
    {
        if (m_cItems >= m_cAllocated)
        {
            m_cAllocated += m_cGrowth;  // allocate room for m_cGrowth items at a time
            m_aData =
                (T*) (m_aData ? ttRealloc(m_aData, m_cAllocated * sizeof(T)) : ttMalloc(m_cAllocated * sizeof(T)));
        }
        m_aData[m_cItems++] = t;
    }

    size_t Add()  // use this to add an emptry member which you can fill in using the returned array index
    {
        if (m_cItems >= m_cAllocated)
        {
            m_cAllocated += m_cGrowth;  // allocate room for m_cGrowth items at a time
            m_aData =
                (T*) (m_aData ? ttReAlloc(m_aData, m_cAllocated * sizeof(T)) : ttMalloc(m_cAllocated * sizeof(T)));
        }
        return m_cItems++;
    }

    bool Find(const T t, size_t* ppos = nullptr) const
    {
        for (size_t pos = 0; pos < m_cItems; pos++)
        {
            if (m_aData[pos] == t)
            {
                if (ppos)
                {
                    *ppos = pos;
                }
                return true;
            }
        }
        return false;
    }

    size_t Find(const T t) const  // returns -1 if not found
    {
        for (size_t pos = 0; pos < m_cItems; pos++)
        {
            if (m_aData[pos] == t)
            {
                return pos;
            }
        }
        return (size_t) -1;
    }

    size_t GetCount() const { return m_cItems; }
    bool   InRange(size_t pos) const { return (pos < m_cItems && m_cItems > 0); }

    void Reset()  // caller's responsibility to delete any allocated members first!
    {
        if (m_aData)
        {
            ttFree(m_aData);
            m_aData = (T*) nullptr;
        }
        m_cAllocated = m_cItems = 0;
    }

    void SetGrowth(size_t growth) { m_cGrowth = growth; }  // Number of items to reserve when more memory is needed

    void operator+=(T t) { Add(t); }
    T&   operator[](size_t pos) const
    {
        ttASSERT(InRange(pos));
        if (!InRange(pos))
            throw;
        return m_aData[pos];
    }

protected:
    size_t m_cItems;
    size_t m_cAllocated;
    size_t m_cGrowth;  // number of items to allocate in advance
    T*     m_aData;
};
