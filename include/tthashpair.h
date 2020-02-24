/////////////////////////////////////////////////////////////////////////////
// Name:      ttCHashPair
// Purpose:   Class utilizing an array of HASH numbers and an associated value
// Author:    Ralph Walden
// Copyright: Copyright (c) 2004-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This class stores an array of hash numbers and an associated value. The array
// is kept sorted based on the hash number in order to increase lookup speed.

#pragma once

#include "ttdebug.h"  // ttASSERT macros
#ifndef _TTLIBWIN_H_GUARD_
    #include "ttlibwin.h"
#endif

// Class utilizing an array of HASH numbers and an associated value
class ttCHashPair
{
public:
    ttCHashPair(size_t EstimatedMembers = 0);
    ~ttCHashPair();

    typedef struct
    {
        size_t hash;
        size_t val;
    } HASH_PAIR;

    // Class functions

    void Add(size_t hash, size_t val = 0);
    void Add(const char* psz, size_t val = 0) { Add(ttHashFromSz(psz), val); }
    bool Find(size_t hash) const { return InRange(GetVal(hash)); }
    bool Find(const char* psz) const { return InRange(GetVal(ttHashFromSz(psz))); }
    size_t GetVal(size_t hash) const;  // returns -1 if not found
    size_t GetVal(const char* psz) const { return GetVal(ttHashFromSz(psz)); }
    void Remove(size_t hash);
    void Remove(const char* psz) { Remove(ttHashFromSz(psz)); }
    void SetVal(size_t hash, size_t val);
    void SetVal(const char* psz, size_t val) { SetVal(ttHashFromSz(psz), val); }

    void Delete();  // remove all hash/val pairs

    bool InRange(size_t pos) const { return (pos < m_cItems && m_cItems > 0); }

    // The URL variants are case-insensitive, and forward/back slashes are considered the same.
    // E.g., foo\bar and Foo/bar will generate the same hash number

    bool FindURL(const char* pszURL) const { return Find(ttHashFromURL(pszURL)); }
    void AddURL(const char* pszURL, size_t val = 0) { Add(ttHashFromURL(pszURL), val); }
    size_t GetUrlVal(const char* pszURL) const { return GetVal(ttHashFromURL(pszURL)); }
    void RemoveURL(const char* pszURL) { Remove(ttHashFromURL(pszURL)); }

#if !defined(NDEBUG)  // Starts debug section.
    // Verifies that the hash numbers are in numerical order
    void Verify()
    {
        for (size_t pos = 1; pos < m_cItems; pos++)
            ttASSERT(m_aData[pos].hash > m_aData[pos - 1].hash);
    }
#endif

    size_t GetCount() const { return m_cItems; }
    HASH_PAIR* GetArray() { return m_aData; }  // Use with caution!

protected:
    HASH_PAIR* FindInsertionPoint(size_t hash) const;
    HASH_PAIR* GetHashPair(size_t hash) const;

private:
    // Class members

    size_t m_cItems;
    size_t m_cAllocated;
    HASH_PAIR* m_aData;
};
