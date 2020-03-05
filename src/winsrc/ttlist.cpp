/////////////////////////////////////////////////////////////////////////////
// Name:      ttCList, ttCDblList, ttCStrIntList
// Purpose:   String arrays based off a private heap
// Author:    Ralph Walden
// Copyright: Copyright (c) 2005-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Under Windows, all the classes here use their own private heap. The advantage of this approach is that
// none of the individual allocations need to be FreeAllocd -- the entire heap is destroyed when the class
// destructor is called. By default, all the classes use a non-serialized private heap -- which means
// multiple threads need to use a crit section if accessing a shared instance of these classes.

// In a non-Windows build the classes will be serialized if the underlying ttMalloc() routines are serialed,
// and the underlying CTTHeap destructor will FreeAlloc all memory allocated by this class (but because it will be
// calling FreeAlloc() for every item allocated, it will be a lot slower then the _WIN32 version).

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include "ttlibwin.h"    // Master header file for ttLibwin.lib
#include "ttlist.h"         // ttCList, ttCDblList, ttCStrIntList
#include "ttcritsection.h"  // ttCCritSection, ttCCritLock

ttCList::ttCList(bool bSerialize)
    : ttCHeap(bSerialize)
{
    m_cAllocated = 0;
    m_cItems = 0;
    m_enum = 0;
    m_aptrs = nullptr;
    m_bSerialize = bSerialize;  // save in case Delete() is called (which recreates the heap)
    m_flags = 0;
}

ttCList::ttCList(HANDLE hHeap)
    : ttCHeap(hHeap)
{
    m_cAllocated = 0;
    m_cItems = 0;
    m_enum = 0;
    m_aptrs = nullptr;
    m_bSerialize = false;
    m_flags = 0;
}

bool ttCList::Enum(const char** ppszResult)
{
    ttASSERT_MSG(ppszResult, "NULL pointer!");
    if (!ppszResult)
        return false;

    if (m_cItems == 0 || m_enum == m_cItems)
        return false;
    *ppszResult = m_aptrs[m_enum++];
    return true;
}

bool ttCList::Enum()
{
    if (m_cItems == 0 || m_enum == m_cItems)
        return false;
    ++m_enum;
    return true;
}

char* ttCList::EnumValue()
{
    auto enumPos = m_enum - 1;
    if (m_cItems == 0 || enumPos == m_cItems)
        return nullptr;
    return m_aptrs[enumPos];
}

void ttCList::SetFlags(size_t flags)
{
    m_flags = flags;
    if (m_flags & FLG_ADD_DUPLICATES)
        m_HashPair.Delete();  // Can't use a hash list if duplicates are allowed
}

size_t ttCList::Add(const char* pszKey)
{
    ttASSERT_MSG(pszKey, "NULL pointer!");
    if (!pszKey)
        return (size_t) -1;

    if (isNoDuplicates())
    {
        ttCStr cszKey;
        char* pszNormalized = NormalizeString(pszKey, cszKey);  // return will point to either pszKey or cszKey

        size_t hash = ttHashFromSz(pszNormalized);
        size_t pos = m_HashPair.GetVal(hash);
        if (m_HashPair.InRange(pos))
            return pos;
        else
            m_HashPair.Add(hash, m_cItems);
    }

    if (m_cItems + 1 >= m_cAllocated)  // the +1 is paranoia -- it shouldn't really be necessary
    {
        m_cAllocated += 32;  // add room for 32 strings at a time
        m_aptrs = (char**) (m_aptrs ? ttReAlloc(m_aptrs, m_cAllocated * sizeof(char*)) :
                                      ttMalloc(m_cAllocated * sizeof(char*)));
    }
    m_aptrs[m_cItems] = ttStrDup(pszKey);
    return m_cItems++;
}

size_t ttCList::GetPos(const char* pszKey) const
{
    ttASSERT_NONEMPTY(pszKey);
    if (!pszKey || !*pszKey)
        return (size_t) -1;

    if (isNoDuplicates())
    {
        ttCStr cszKey;
        char* pszNormalized = NormalizeString(pszKey, cszKey);  // return will point to either pszKey or cszKey
        return m_HashPair.GetVal(pszNormalized);
    }

    if (m_flags & FLG_IGNORE_CASE || m_flags & FLG_URL_STRINGS)
    {
        ttCStr cszKey;
        char* pszNormalized = NormalizeString(pszKey, cszKey);  // return will point to either pszKey or cszKey

        ttCStr cszList;
        for (size_t pos = 0; pos < m_cItems; ++pos)
        {
            char* pszList = NormalizeString(m_aptrs[pos], cszList);
            if (ttIsSameStr(pszList, pszNormalized))
                return pos;
        }
        return (size_t) -1;
    }

    for (size_t pos = 0; pos < m_cItems; ++pos)
    {
        if (ttIsSameStr(m_aptrs[pos], pszKey))
            return pos;
    }
    return (size_t) -1;
}

char* ttCList::Get(size_t pos) const
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return nullptr;
    return m_aptrs[pos];
}

void ttCList::Remove(size_t pos)
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return;

    if (isNoDuplicates())
    {
        // The hash pair stores the position of our string which we need to update
        ttCHashPair::HASH_PAIR* phshPair = m_HashPair.GetArray();
        for (size_t posHsh = 0; posHsh < m_cItems; ++posHsh)
        {
            if (phshPair[posHsh].val > pos)
                --phshPair[posHsh].val;
        }
        ttCStr cszKey;
        char* pszNormalized =
            NormalizeString(m_aptrs[pos], cszKey);  // return will point to either pszKey or cszKey
        m_HashPair.Remove(pszNormalized);
    }

    ttFree((void*) m_aptrs[pos]);
    memmove((void*) (m_aptrs + pos), (void*) (m_aptrs + pos + 1), (m_cItems - (pos + 1)) * sizeof(char*));
    --m_cItems;
}

void ttCList::Remove(const char* psz)
{
    size_t pos = GetPos(psz);
    if (pos != (size_t) -1)
        Remove(pos);
}

void ttCList::Delete()
{
    if (m_cItems == 0)
        return;  // we haven't added anything yet, so nothing to do!
    if (isCreated() && !isMainHeap())
    {
        DeleteAll();
    }
    else
    {  // if we didn't create the heap ourselves, then we need to delete each individual item
        for (size_t pos = 0; pos < m_cItems; ++pos)
        {
            ttFree((void*) m_aptrs[pos]);
        }
        ttFree((void*) m_aptrs);
    }
    m_cAllocated = 0;
    m_cItems = 0;
    m_enum = 0;
    m_aptrs = nullptr;
    m_HashPair.Delete();
}

void ttCList::Replace(size_t pos, const char* pszKey)
{
    ttASSERT_NONEMPTY(pszKey);
    ttASSERT(InRange(pos));
    if (!InRange(pos) || ttIsEmpty(pszKey))
        return;

    if (isNoDuplicates())
        m_HashPair.Remove(m_aptrs[pos]);

    ttFree((void*) m_aptrs[pos]);

    m_aptrs[pos] = ttStrDup(pszKey);

    if (isNoDuplicates())
    {
        ttCStr cszKey;
        char* pszNormalized = NormalizeString(pszKey, cszKey);
        m_HashPair.Add(pszNormalized, pos);
    }
}

void ttCList::Swap(size_t posA, size_t posB)
{
    if (posA > m_cItems || posB > m_cItems || posA == posB)
        return;
    char* pszA = m_aptrs[posA];
    m_aptrs[posA] = m_aptrs[posB];
    m_aptrs[posB] = pszA;

    if (isNoDuplicates())
    {
        // We have to "normalize" the strings (deal with case-insensitive, forward/back slash conversion) before
        // CHashPair can find them

        ttCStr cszA, cszB;
        char* pszNormalizedA = NormalizeString(m_aptrs[posA], cszA);
        char* pszNormalizedB = NormalizeString(m_aptrs[posB], cszB);

        m_HashPair.SetVal(pszNormalizedA, posA);
        m_HashPair.SetVal(pszNormalizedB, posB);
    }
}

void ttCList::InsertAt(size_t pos, const char* pszKey)
{
    if (pos >= m_cItems)
    {
        Add(pszKey);
        return;
    }

    if (m_cItems + 1 >= m_cAllocated)  // the +1 is paranoia -- it shouldn't really be necessary
    {
        m_cAllocated += 32;  // add room for 32 strings at a time
        m_aptrs = (char**) (m_aptrs ? ttReAlloc(m_aptrs, m_cAllocated * sizeof(char*)) :
                                      ttMalloc(m_cAllocated * sizeof(char*)));
    }
    memmove((void*) (m_aptrs + pos + 1), (void*) (m_aptrs + pos), (m_cItems - pos + 1) * sizeof(char*));

    if (isNoDuplicates())
    {
        ttCHashPair::HASH_PAIR* pHashPair = m_HashPair.GetArray();
        for (size_t posHash = 0; posHash < m_HashPair.GetCount(); ++posHash)
        {
            if (pHashPair[posHash].val >= pos)
                pHashPair[posHash].val++;
        }

        ttCStr cszKey;
        char* pszNormalized = NormalizeString(pszKey, cszKey);
        m_HashPair.Add(pszNormalized, pos);
    }

    m_aptrs[pos] = ttStrDup(pszKey);
    m_cItems++;
}

void ttCList::Sort()
{
    qsorti(0, m_cItems - 1);
    if (isNoDuplicates())
    {
        for (size_t pos = 0; pos < m_cItems; ++pos)
            m_HashPair.Add(m_aptrs[pos], pos);  // It already exists, this will just update the associated position
    }
}

// It is the caller's responsibility to ensure that all strings have at least iColumn characters

void ttCList::Sort(size_t iColumn)
{
    m_SortColumn = iColumn;
    qsortCol(0, m_cItems - 1);
    if (isNoDuplicates())
    {
        for (size_t pos = 0; pos < m_cItems; ++pos)
            m_HashPair.Add(m_aptrs[pos], pos);  // It already exists, this will just update the associated position
    }
}

char* ttCList::NormalizeString(const char* pszString, ttCStr& cszKey) const
{
    if (m_flags & FLG_IGNORE_CASE || m_flags & FLG_URL_STRINGS)
    {
        cszKey = pszString;
        cszKey.MakeLower();  // FLG_URL_STRINGS are always case-insensitive
        if (m_flags & FLG_URL_STRINGS)
            ttBackslashToForwardslash(cszKey);
        return cszKey;
    }
    return (char*) pszString;
}

void ttCList::qsorti(ptrdiff_t low, ptrdiff_t high)
{
    if (low >= high)
        return;

    // Do we need to sort?

    ptrdiff_t pos = high - 1;
    while (pos >= low)
    {
        if (strcmp(m_aptrs[pos], m_aptrs[pos + 1]) > 0)
            break;
        else
            pos--;
    }
    if (pos < low)
        return;  // it's already sorted

    swap(low, (low + high) / 2);

    ptrdiff_t end = low;
    for (pos = low + 1; pos <= high; pos++)
    {
        if (strcmp(m_aptrs[pos], m_aptrs[low]) < 0)
            swap(++end, pos);
    }
    swap(low, end);

    if (low < end - 1)
        qsorti(low, end - 1);
    if (end + 1 < high)
        qsorti(end + 1, high);
}

// This is a bit dangerous as we do NOT check to see if each string actually has at least m_SortColumn characters

void ttCList::qsortCol(ptrdiff_t low, ptrdiff_t high)
{
    if (low >= high)
        return;

    // Do we need to sort?

    ptrdiff_t pos = high - 1;
    while (pos >= low)
    {
        if (strcmp(m_aptrs[pos] + m_SortColumn, m_aptrs[pos + 1] + m_SortColumn) > 0)
            break;
        else
            pos--;
    }
    if (pos < low)
        return;  // it's already sorted

    swap(low, (low + high) / 2);

    ptrdiff_t end = low;
    for (pos = low + 1; pos <= high; pos++)
    {
        if (strcmp(m_aptrs[pos] + m_SortColumn, m_aptrs[low] + m_SortColumn) < 0)
            swap(++end, pos);
    }
    swap(low, end);

    if (low < end - 1)
        qsortCol(low, end - 1);
    if (end + 1 < high)
        qsortCol(end + 1, high);
}

/////////////////////   ttCDblList      //////////////////////////

ttCDblList::ttCDblList(bool bSerialize)
    : ttCHeap(bSerialize)
{
    m_cAllocated = 0;
    m_cItems = 0;
    m_aptrs = nullptr;
    m_bSerialize = bSerialize;
    m_pHashLookup = nullptr;
    m_bIgnoreCase = false;
}

ttCDblList::~ttCDblList()
{
    delete m_pHashLookup;
}

void ttCDblList::Delete()
{
    if (m_cItems == 0)
        return;  // we haven't added anything yet, so nothing to do!

    if (isCreated() && !isMainHeap())
    {
        DeleteAll();
    }
    else
    {  // if we didn't create the heap ourselves, then we need to delete each individual item
        for (size_t pos = 0; pos < m_cItems; ++pos)
        {
            ttFree((void*) m_aptrs[pos].pszKey);
            ttFree((void*) m_aptrs[pos].pszVal);
        }
        ttFree((void*) m_aptrs);
    }
    m_cAllocated = 0;
    m_cItems = 0;
    m_aptrs = nullptr;

    if (m_pHashLookup)
    {
        delete m_pHashLookup;
        m_pHashLookup = nullptr;
    }
}

void ttCDblList::PreventDuplicateKeys()
{
    if (!m_pHashLookup)
        m_pHashLookup = new ttCHashPair;
}

void ttCDblList::Add(const char* pszKey, const char* pszVal)
{
    ttASSERT_NONEMPTY(pszKey);
    ttASSERT_MSG(pszVal,
                 "NULL pointer!");  // it's okay to add an empty val string as long as it isn't a null pointer

    if (!pszKey || !*pszKey || !pszVal)
        return;

    if (m_pHashLookup)
    {
        size_t hash = ttHashFromSz(pszKey);
        if (m_pHashLookup->Find(hash))
            return;
        else
            m_pHashLookup->Add(hash);
    }

    if (m_cItems >= m_cAllocated)
    {
        m_cAllocated += 32;  // add room for 32 strings at a time
        m_aptrs = (DBLPTRS*) (m_aptrs ? ttReAlloc(m_aptrs, m_cAllocated * sizeof(DBLPTRS)) :
                                        ttMalloc(m_cAllocated * sizeof(DBLPTRS)));
    }
    m_aptrs[m_cItems].pszKey = ttStrDup(pszKey);
    m_aptrs[m_cItems++].pszVal = ttStrDup(pszVal);
}

bool ttCDblList::FindKey(const char* pszKey, size_t* ppos) const
{
    ttASSERT_NONEMPTY(pszKey);
    if (!pszKey || !*pszKey)
        return false;
    if (m_bIgnoreCase)
    {
        for (size_t i = 0; i < m_cItems; i++)
        {
            if (ttIsSameStrI(m_aptrs[i].pszKey, pszKey))
            {
                if (ppos)
                    *ppos = i;
                return true;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < m_cItems; i++)
        {
            if (ttIsSameStr(m_aptrs[i].pszKey, pszKey))
            {
                if (ppos)
                    *ppos = i;
                return true;
            }
        }
    }
    return false;
}

bool ttCDblList::FindNextKey(const char* pszKey, size_t* ppos) const
{
    ttASSERT_NONEMPTY(pszKey);
    if (!pszKey || !*pszKey)
        return false;
    if (m_bIgnoreCase)
    {
        for (size_t i = *ppos + 1; i < m_cItems; i++)
        {
            if (ttIsSameStrI(m_aptrs[i].pszKey, pszKey))
            {
                if (ppos)
                    *ppos = i;
                return true;
            }
        }
    }
    else
    {
        for (size_t i = *ppos + 1; i < m_cItems; i++)
        {
            if (ttIsSameStr(m_aptrs[i].pszKey, pszKey))
            {
                if (ppos)
                    *ppos = i;
                return true;
            }
        }
    }
    return false;
}

bool ttCDblList::FindVal(const char* pszVal, size_t* ppos) const
{
    ttASSERT_MSG(pszVal, "NULL pointer!");
    if (!pszVal)
        return false;
    if (m_bIgnoreCase)
    {
        for (size_t i = 0; i < m_cItems; i++)
        {
            if (ttIsSameStrI(m_aptrs[i].pszVal, pszVal))
            {
                if (ppos)
                    *ppos = i;
                return true;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < m_cItems; i++)
        {
            if (ttIsSameStr(m_aptrs[i].pszVal, pszVal))
            {
                if (ppos)
                    *ppos = i;
                return true;
            }
        }
    }
    return false;
}

char* ttCDblList::GetKeyAt(size_t pos) const
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return nullptr;
    return m_aptrs[pos].pszKey;
}

char* ttCDblList::GetValAt(size_t pos) const
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return nullptr;
    return m_aptrs[pos].pszVal;
}

char* ttCDblList::GetMatchingVal(const char* pszKey) const
{
    ttASSERT_MSG(pszKey, "NULL pointer!");
    if (!pszKey)
        return nullptr;
    if (m_bIgnoreCase)
    {
        for (size_t pos = 0; pos < m_cItems; ++pos)
        {
            if (ttIsSameStrI(m_aptrs[pos].pszKey, pszKey))
                return m_aptrs[pos].pszVal;
        }
    }
    else
    {
        for (size_t pos = 0; pos < m_cItems; ++pos)
        {
            if (ttIsSameStr(m_aptrs[pos].pszKey, pszKey))
                return m_aptrs[pos].pszVal;
        }
    }
    return nullptr;
}

void ttCDblList::Replace(size_t pos, const char* pszKey, const char* pszVal)
{
    ttASSERT_NONEMPTY(pszKey);
    ttASSERT_MSG(pszVal, "NULL pointer!");  // okay if pszVal is an empty string, just not a null pointer
    ttASSERT(InRange(pos));
    if (!InRange(pos) || !pszKey || !*pszKey || *pszVal)
        return;

    ttFree((void*) m_aptrs[pos].pszKey);
    ttFree((void*) m_aptrs[pos].pszVal);
    m_aptrs[pos].pszKey = ttStrDup(pszKey);
    m_aptrs[pos].pszVal = ttStrDup(pszVal);
}

void ttCDblList::SortKeys()
{
    ttASSERT(m_cItems > 0);
    if (m_cItems < 2)
        return;
    m_bSortKeys = true;
    qsorti(0, m_cItems - 1);
}

void ttCDblList::SortVals()
{
    ttASSERT(m_cItems > 0);
    if (m_cItems < 2)
        return;
    m_bSortKeys = false;
    qsorti(0, m_cItems - 1);
}

void ttCDblList::qsorti(ptrdiff_t low, ptrdiff_t high)
{
    if (low >= high)
        return;

    // First find out if we are already sorted

    ptrdiff_t pos = high - 1;
    while (pos >= low)
    {
        if (m_bSortKeys)
        {
            if (strcmp(m_aptrs[pos].pszKey, m_aptrs[pos + 1].pszKey) > 0)
                break;
            else
                pos--;
        }
        else
        {
            if (strcmp(m_aptrs[pos].pszVal, m_aptrs[pos + 1].pszVal) > 0)
                break;
            else
                pos--;
        }
    }
    if (pos < low)
        return;  // it's already sorted

    swap(low, (low + high) / 2);

    ptrdiff_t end = low;
    for (pos = low + 1; pos <= high; pos++)
    {
        if (m_bSortKeys)
        {
            if (strcmp(m_aptrs[pos].pszKey, m_aptrs[low].pszKey) < 0)
                swap(++end, pos);
        }
        else
        {
            if (strcmp(m_aptrs[pos].pszVal, m_aptrs[low].pszVal) < 0)
                swap(++end, pos);
        }
    }
    swap(low, end);

    if (low < end - 1)
        qsorti(low, end - 1);
    if (end + 1 < high)
        qsorti(end + 1, high);
}

//////////////////////////////// ttCStrIntList  /////////////////////////////////

ttCStrIntList::ttCStrIntList(bool bSerialize)
    : ttCHeap(bSerialize)
{
    m_cAllocated = 0;
    m_cItems = 0;
    m_aptrs = nullptr;
    m_bSerialize = bSerialize;
    m_posEnumKey = (size_t) -1;
    m_bIgnoreCase = false;
}

void ttCStrIntList::Add(const char* pszKey, ptrdiff_t newVal)
{
    ttASSERT_NONEMPTY(pszKey);
    if (!pszKey || !*pszKey)
        return;

    for (size_t pos = 0; pos < m_cItems; ++pos)
    {
        if (m_bIgnoreCase ? ttIsSameStrI(m_aptrs[pos].pszKey, pszKey) : ttIsSameStr(m_aptrs[pos].pszKey, pszKey))
        {
            ptrdiff_t cItems = m_aptrs[pos].pVal[0];
            for (ptrdiff_t valPos = 1; valPos <= cItems; ++valPos)
            {
                if (newVal == m_aptrs[pos].pVal[valPos])
                    return;  // we've already added this value
            }
            // newVal not found, so add it
            ++cItems;
            m_aptrs[pos].pVal = (ptrdiff_t*) ttReAlloc(m_aptrs[pos].pVal, (cItems + 1) * sizeof(ptrdiff_t));
            m_aptrs[pos].pVal[0] = cItems;
            m_aptrs[pos].pVal[cItems] = newVal;
            return;
        }
    }

    // key not found, so add it

    if (m_cItems >= m_cAllocated)
    {
        m_cAllocated += 32;  // add room for 32 strings at a time
        m_aptrs = (DBLPTRS*) (m_aptrs ? ttReAlloc(m_aptrs, m_cAllocated * sizeof(DBLPTRS)) :
                                        ttMalloc(m_cAllocated * sizeof(DBLPTRS)));
    }
    m_aptrs[m_cItems].pszKey = ttStrDup(pszKey);
    m_aptrs[m_cItems].pVal = (ptrdiff_t*) ttMalloc(2 * sizeof(ptrdiff_t));
    m_aptrs[m_cItems].pVal[0] = 1;
    m_aptrs[m_cItems].pVal[1] = newVal;
    ++m_cItems;
}

void ttCStrIntList::Delete()
{
    if (m_cItems == 0)
        return;  // we haven't added anything yet, so nothing to do!

    if (isCreated() && !isMainHeap())
    {
        DeleteAll();
    }
    else
    {  // if we didn't create the heap ourselves, then we need to delete each individual item
        for (size_t pos = 0; pos < m_cItems; ++pos)
        {
            ttFree((void*) m_aptrs[pos].pszKey);
            ttFree((void*) m_aptrs[pos].pVal);
        }
        ttFree((void*) m_aptrs);
    }
    m_cAllocated = 0;
    m_cItems = 0;
    m_aptrs = nullptr;
    m_posEnumKey = (size_t) -1;
}

bool ttCStrIntList::Add(size_t pos, ptrdiff_t newVal)
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return false;

    ptrdiff_t cItems = m_aptrs[pos].pVal[0];
    for (ptrdiff_t valPos = 1; valPos <= cItems; ++valPos)
    {
        if (newVal == m_aptrs[pos].pVal[valPos])
            return true;
    }
    // newVal not found, so add it
    ++cItems;
    m_aptrs[pos].pVal = (ptrdiff_t*) ttReAlloc(m_aptrs[pos].pVal, (cItems + 1) * sizeof(ptrdiff_t));
    m_aptrs[pos].pVal[0] = cItems;
    m_aptrs[pos].pVal[cItems] = newVal;
    return true;
}

bool ttCStrIntList::FindKey(const char* pszKey, size_t* ppos) const
{
    ttASSERT_NONEMPTY(pszKey);
    if (!pszKey || !*pszKey)
        return false;
    for (size_t pos = 0; pos < m_cItems; ++pos)
    {
        if (m_bIgnoreCase ? ttIsSameStrI(m_aptrs[pos].pszKey, pszKey) : ttIsSameStr(m_aptrs[pos].pszKey, pszKey))
        {
            if (ppos)
                *ppos = pos;
            return true;
        }
    }
    return false;
}

bool ttCStrIntList::GetValCount(const char* pszKey, ptrdiff_t* pVal) const
{
    ttASSERT_NONEMPTY(pszKey);
    ttASSERT_MSG(pVal, "NULL pointer!");

    if (!pszKey || !*pszKey || !pVal)
        return false;
    for (size_t pos = 0; pos < m_cItems; ++pos)
    {
        if (m_bIgnoreCase ? ttIsSameStrI(m_aptrs[pos].pszKey, pszKey) : ttIsSameStr(m_aptrs[pos].pszKey, pszKey))
        {
            *pVal = m_aptrs[pos].pVal[0];
            return true;
        }
    }
    return false;
}

ptrdiff_t ttCStrIntList::GetValCount(size_t pos) const
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return 0;  // There isn't a way to indicate an error other then the ASSERT in Debug builds.
    return m_aptrs[pos].pVal[0];
}

bool ttCStrIntList::GetVal(const char* pszKey, ptrdiff_t* pVal, size_t posVal) const
{
    ttASSERT_NONEMPTY(pszKey);
    ttASSERT_MSG(pVal, "NULL pointer!");

    if (!pszKey || !*pszKey || !pVal)
        return false;
    for (size_t posKey = 0; posKey < m_cItems; ++posKey)
    {
        if (m_bIgnoreCase ? ttIsSameStrI(m_aptrs[posKey].pszKey, pszKey) :
                            ttIsSameStr(m_aptrs[posKey].pszKey, pszKey))
        {
            if ((ptrdiff_t) posVal > m_aptrs[posKey].pVal[0])
                return false;
            *pVal = m_aptrs[posKey].pVal[posVal];
            return true;
        }
    }
    return false;
}

bool ttCStrIntList::GetVal(size_t pos, ptrdiff_t* pVal, size_t posVal) const
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return false;

    if ((ptrdiff_t) posVal > m_aptrs[pos].pVal[0])
        return false;
    *pVal = m_aptrs[pos].pVal[posVal];
    return true;
}

bool ttCStrIntList::BeginEnum(const char* pszKey)
{
    if (!FindKey(pszKey, &m_posEnumKey))
    {
        m_posEnumKey = (size_t) -1;  // flag it as invalid
        return false;
    }
    m_posEnumVal = 1;
    return true;
}

bool ttCStrIntList::Enum(ptrdiff_t* pVal)
{
    if (m_posEnumKey >= m_cItems)  // this will also catch -1 as the value (since m_posEnumKey is unsigned)
        return false;
    if (m_posEnumVal > m_aptrs[m_posEnumKey].pVal[0])
        return false;
    *pVal = m_aptrs[m_posEnumKey].pVal[m_posEnumVal++];
    return true;
}

char* ttCStrIntList::GetKey(size_t pos) const
{
    ttASSERT(InRange(pos));
    if (!InRange(pos))
        return nullptr;

    return m_aptrs[pos].pszKey;
}

//////////////////////////////// ttCIntStrList  /////////////////////////////////

// Unlike the above classes, this class does NOT use a sub-heap

ttCIntStrList::ttCIntStrList(void)
{
    m_cAllocated = 0;
    m_cItems = 0;
    m_aData = nullptr;
    m_pcrit = nullptr;
}

ttCIntStrList::~ttCIntStrList()
{
    Delete();
    if (m_aData)
        ttFree((void*) m_aData);
    if (m_pcrit)
        delete m_pcrit;
}

const char* ttCIntStrList::Add(size_t id, const char* psz)
{
    ttASSERT_MSG(psz, "NULL pointer!");
    if (!psz)
        return nullptr;

    if (!m_pcrit)
        m_pcrit = new ttCCritSection;
    ttCCritLock lock(m_pcrit);

    if (m_cItems + 1 > m_cAllocated)
    {
        m_cAllocated += 32;
        m_aData = (KEYVAL_PAIR*) ttReAlloc(m_aData, m_cAllocated * sizeof(KEYVAL_PAIR));
    }

    m_aData[m_cItems].id = id;
    m_aData[m_cItems].psz = ttStrDup(psz);

    return m_aData[m_cItems++].psz;
}

const char* ttCIntStrList::Find(size_t id)
{
    if (!m_pcrit)
        m_pcrit = new ttCCritSection;
    ttCCritLock lock(m_pcrit);

    for (size_t pos = 0; InRange(pos); ++pos)
    {
        if (m_aData[pos].id == id)
            return m_aData[pos].psz;
    }

    return nullptr;
}

void ttCIntStrList::Delete()
{
    if (!m_pcrit)
        m_pcrit = new ttCCritSection;
    ttCCritLock lock(m_pcrit);

    if (m_cItems == 0)
        return;  // we haven't added anything yet, so nothing to do!

    for (size_t pos = 0; InRange(pos); ++pos)
        ttFree((void*) m_aData[pos].psz);
    m_cItems = 0;
}