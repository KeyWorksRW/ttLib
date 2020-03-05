/////////////////////////////////////////////////////////////////////////////
// Name:      ttCHashPair
// Purpose:   Class utilizing an array of HASH numbers and an associated value
// Author:    Ralph Walden
// Copyright: Copyright (c) 2004-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include "tthashpair.h"  // ttCHashPair
#include "ttheap.h"      // memory allocation routines

#define GROWTH_MALLOC 16

ttCHashPair::ttCHashPair(size_t EstimatedMembers)
{
    if (!EstimatedMembers)
    {
        m_cAllocated = 0;
        m_aData = nullptr;
    }
    else
    {
        m_aData = (HASH_PAIR*) ttMalloc(EstimatedMembers * sizeof(HASH_PAIR));
        m_cAllocated = EstimatedMembers;
    }
    m_cItems = 0;
}

ttCHashPair::~ttCHashPair()
{
    Delete();
}

void ttCHashPair::Delete()
{
    if (m_aData)
    {
        ttFree(m_aData);
        m_aData = nullptr;
        m_cItems = m_cAllocated = 0;
    }
}

void ttCHashPair::Add(size_t hash, size_t val)
{
    // Note that we are adding the full HASH_PAIR structure, not a pointer to the structure

    if (m_cItems + 1 > m_cAllocated)
    {
        m_cAllocated += GROWTH_MALLOC;
        m_aData = (HASH_PAIR*) ttReAlloc(m_aData, m_cAllocated * sizeof(HASH_PAIR));
    }

    if (!m_cItems)
    {
        m_aData[0].hash = hash;
        m_aData[0].val = val;
        ++m_cItems;
        return;
    }

    HASH_PAIR* pInsert = FindInsertionPoint(hash);
    if (pInsert->hash == hash)  // If hash is already added, update the value
    {
        pInsert->val = val;
        return;
    }

    // The array of pairs is larger then the actual number of members in use, so if the insertion point is at the
    // end of the array, then all we have to do is set the hash/val pair.

    size_t cbMove = (m_aData + m_cItems) - pInsert;
    if (cbMove)
        memmove(pInsert + 1, pInsert, cbMove * sizeof(HASH_PAIR));
    pInsert->hash = hash;
    pInsert->val = val;
    ++m_cItems;
}

void ttCHashPair::SetVal(size_t hash, size_t val)
{
    HASH_PAIR* pPair = GetHashPair(hash);
    if (pPair)
        pPair->val = val;
}

void ttCHashPair::Remove(size_t hashDel)
{
    for (size_t pos = 0; pos < m_cItems; pos++)
    {
        if (m_aData[pos].hash == hashDel)
        {
            if (pos == m_cItems - 1)
            {
                m_cItems--;
                return;
            }

            memmove(m_aData + pos, m_aData + pos + 1, ((m_cItems - 1) - pos) * sizeof(HASH_PAIR));
            m_cItems--;
            return;
        }
    }
}

size_t ttCHashPair::GetVal(size_t hash) const
{
    if (!m_aData)
        return (size_t) -1;

    HASH_PAIR* pLow = m_aData;
    HASH_PAIR* pHigh = m_aData + (m_cItems - 1);

    size_t num = m_cItems;
    while (pLow->hash <= pHigh->hash)
    {
        size_t half = num / 2;
        if (half)
        {
            HASH_PAIR* pMid = pLow + (num & 1 ? half : (half - 1));
            if (pMid->hash == hash)
                return pMid->val;
            else if (pMid->hash > hash)
            {
                pHigh = pMid - 1;
                num = num & 1 ? half : half - 1;
            }
            else
            {
                pLow = pMid + 1;
                num = half;
            }
        }
        else if (num)
            return (pLow->hash == hash ? pLow->val : (size_t) -1);
        else
            return (size_t) -1;
    }
    return (size_t) -1;
}

ttCHashPair::HASH_PAIR* ttCHashPair::FindInsertionPoint(size_t hash) const
{
    if (m_aData[0].hash > hash)
        return &m_aData[0];  // insert at beginning

    HASH_PAIR* pHigh = m_aData + (m_cItems - 1);
    if (pHigh->hash < hash)
        return pHigh + 1;  // insert at end

    HASH_PAIR* pLow = m_aData;
    HASH_PAIR* pMid;

    size_t num = m_cItems;
    while (pLow <= pHigh)
    {
        size_t half = num / 2;
        if (half)
        {
            pMid = pLow + (num & 1 ? half : (half - 1));
            if (pMid->hash == hash)
                return pMid;  // already added
            if (pMid->hash > hash)
            {
                pHigh = pMid - 1;
                if (pHigh->hash < hash)
                    return pMid;  // insert at end
                num = num & 1 ? half : half - 1;
            }
            else
            {
                pLow = pMid + 1;
                num = half;
            }
        }
        else
            return pLow;
    }
    return pLow;
}

ttCHashPair::HASH_PAIR* ttCHashPair::GetHashPair(size_t hash) const
{
    if (!m_aData)
        return nullptr;

    HASH_PAIR* pLow = m_aData;
    HASH_PAIR* pHigh = m_aData + (m_cItems - 1);

    size_t num = m_cItems;
    while (pLow->hash <= pHigh->hash)
    {
        size_t half = num / 2;
        if (half)
        {
            HASH_PAIR* pMid = pLow + (num & 1 ? half : (half - 1));
            if (pMid->hash == hash)
                return pMid;
            else if (pMid->hash > hash)
            {
                pHigh = pMid - 1;
                num = num & 1 ? half : half - 1;
            }
            else
            {
                pLow = pMid + 1;
                num = half;
            }
        }
        else if (num && pLow->hash == hash)
            return pLow;
        else
            return nullptr;
    }
    return nullptr;
}
