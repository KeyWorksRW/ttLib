/////////////////////////////////////////////////////////////////////////////
// Name:      ttCList, ttCDblList
// Purpose:   String arrays based off a private heap
// Author:    Ralph Walden
// Copyright: Copyright (c) 2005-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// All the classes here use their own private heap. The advantage of this approach is that none of the individual
// allocations need to be FreeAllocd -- the entire heap is destroyed when the class destructor is called. By default,
// all the classes use a non-serialized private heap -- which means multiple threads need to use a crit section if
// accessing a shared class.

#pragma once

#if defined(_WIN32)
    #include <wtypes.h>
#endif  // _WIN32

#include "ttheap.h"      // ttCHeap
#include "tthashpair.h"  // ttCHashPair
#include "ttstr.h"       // ttCStr

// String arrays based off a private heap
class ttCList : public ttCHeap
{
public:
    ttCList(bool bSerialize = true);  // true makes the class thread safe
    ttCList(HANDLE hHeap);

    enum
    {
        FLG_ADD_DUPLICATES = 1 << 0,  // add the string even if it has already been added
        FLG_IGNORE_CASE = 1 << 1,
        FLG_URL_STRINGS = 1 << 2,  // string case is ignored, '\' and '/' are considered the same
    };

    // SetFlags() will not change any previously added strings
    // Use any combination of FLG_ADD_DUPLICATES | FLG_IGNORE_CASE | FLG_URL_STRINGS
    void SetFlags(size_t flags);
    void AllowDuplicates() { SetFlags(FLG_ADD_DUPLICATES); }

    size_t Add(const char* psz);
    bool   Find(const char* psz) const { return InRange(GetPos(psz)); }

    // Returns -1 if not found
    size_t GetPos(const char* psz) const;
    void   InsertAt(size_t pos, const char* psz);
    void   Remove(const char* psz);
    void   Remove(size_t pos);
    void   Replace(size_t pos, const char* psz);
    void   Swap(size_t posA, size_t posB);

    size_t GetCount() const { return m_cItems; }
    bool   IsEmpty() const { return m_cItems == 0; }
    bool   InRange(size_t pos) const { return (pos < m_cItems && m_cItems > 0); }
    // Deletes all strings
    void Delete();

    // zero-based index, will return nullptr if pos >= GetCount()
    char* Get(size_t pos) const;

    // CSimpleArray/CAtlArray equivalents

    size_t GetSize() const { return GetCount(); }
    void   RemoveAll() { Delete(); }
    void   RemoveAt(size_t pos) { Remove(pos); }
    void   SetAt(size_t pos, const char* psz) { Replace(pos, psz); }

    const char* GetAt(size_t pos) const { return Get(pos); }

    // End CSimpleArray/CAtlArray equivalents

    /*
        Example usage:

            ttCList lst;
            // ... add a bunch of strings
            lst.BeginEnum();
            while(lst.Enum())
                cout << lst.EnumValue();
     */

    void BeginEnum() { m_enum = 0; }
    bool Enum();
    // returns pointer to last enumerated string, or nullptr if no string found
    char* EnumValue();
    // use this if you want to receive a pointer to the string
    bool Enum(const char** ppszResult);

    void Sort();  // sort strings into alphabetical order

    // Use the following with caution! All strings MUST have at least iColumn characters!
    //
    // Sort strings using the offset (equivalent to strcmp(str1 + iColumn, str2 + iColumn)
    void Sort(size_t iColumn);

    void  operator+=(const char* psz) { Add(psz); }
    char* operator[](size_t pos) const { return Get(pos); }

    // use with EXTREME caution, provided primarily for testing
    char** GetArray() { return m_aptrs; }

protected:
    inline void swap(ptrdiff_t pos1, ptrdiff_t pos2)
    {
        char* pszTmp = m_aptrs[pos1];
        m_aptrs[pos1] = m_aptrs[pos2];
        m_aptrs[pos2] = pszTmp;
    }
    void qsorti(ptrdiff_t low, ptrdiff_t high);
    void qsortCol(ptrdiff_t low, ptrdiff_t high);

    char* NormalizeString(const char* pszFileName, ttCStr& cszKey) const;

    bool isNoDuplicates() const { return (m_flags & FLG_ADD_DUPLICATES) ? false : true; }

    // Class members

    size_t m_cItems;
    size_t m_enum;

    size_t m_cAllocated;
    char** m_aptrs;

    ttCStr m_cszKey;
    size_t m_flags;
    size_t m_SortColumn;

    ttCHashPair m_HashPair;
    bool        m_bSerialize;
};

// Holds a pair of strings, referenced as Key and Val. Note that unlike ttCList, the default behaviour is to allow
// duplicate keys
class ttCDblList : public ttCHeap
{
public:
    // ignore case when searching for keys or values
    void IgnoreCase() { m_bIgnoreCase = true; }
    // key won't be added if it already exists
    void PreventDuplicateKeys();

    ttCDblList(bool bSerialize = false);
    ~ttCDblList();

    void Add(const char* pszKey, const char* pszVal);

    bool  FindKey(const char* pszKey, size_t* ppos = nullptr) const;
    bool  FindVal(const char* pszVal, size_t* ppos = nullptr) const;
    char* GetKeyAt(size_t pos) const;
    char* GetValAt(size_t pos) const;
    char* GetMatchingVal(const char* pszKey) const;

    char* GetValueAt(size_t pos) const { return GetValAt(pos); }  // to match CSimpleArray::GetValueAt

    void Replace(size_t pos, const char* pszKey, const char* pszVal);

    size_t GetCount() const { return m_cItems; }
    bool   IsEmpty() const { return m_cItems == 0; }
    bool   InRange(size_t pos) const { return (pos < m_cItems && m_cItems > 0); }
    // Deletes all strings
    void Delete();

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

    typedef struct
    {
        char* pszKey;
        char* pszVal;
    } DBLPTRS;

    // Class members

    size_t   m_cItems;
    size_t   m_cAllocated;
    DBLPTRS* m_aptrs;

    bool m_bSortKeys;
    bool m_bSerialize;
    bool m_bIgnoreCase;

    ttCHashPair* m_pHashLookup;
};

// Similar to ttCDblList only the val is an array of ptrdiff_t instead of a char*
class ttCStrIntList : public ttCHeap
{
public:
    typedef struct
    {
        char*      pszKey;
        ptrdiff_t* pVal;
    } DBLPTRS;

    ttCStrIntList(bool bSerialize = false);

    // ignore case when searching for keys
    void IgnoreCase() { m_bIgnoreCase = true; }

    /*
        If the string already exists, but the Val doesn't, Add() will add the Val to the array of Vals
        associated with the string. I.e.,

        Add("my key", 1);
        Add("my key", 1);   // nothing will be added because 1 is already associated with "my key"
        Add("my key", 2);   // Now both 1 and 2 are associated with "my key"
    */

    void Add(const char* pszKey, ptrdiff_t newVal);
    // Add vals to an existing key
    bool Add(size_t posKey, ptrdiff_t newVal);
    bool FindKey(const char* pszKey, size_t* ppos = nullptr) const;
    // returns the number of keys actually added (duplicates are not added)
    size_t GetCount() const { return m_cItems; }
    bool   InRange(size_t pos) const { return (pos < m_cItems && m_cItems > 0); }

    // Deletes all strings
    void Delete();

    // To get all the numbers, either use Enum() or call these GetVal routines. Be aware that any pointer to
    // the values is only valid for as long as no additional values are added to the key

    bool      GetValCount(const char* pszKey, ptrdiff_t* pVal) const;
    ptrdiff_t GetValCount(size_t posKey) const;

    bool  GetVal(const char* pszKey, ptrdiff_t* pVal, size_t posVal = 0) const;
    bool  GetVal(size_t posKey, ptrdiff_t* pVal, size_t posVal = 0) const;
    char* GetKey(size_t posKey) const;

    /*
        You can enumerate through all the numbers assigned to a string using code
        similar to the following:

        ttCStrIntList strList;
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

    size_t   m_cItems;
    size_t   m_cAllocated;
    DBLPTRS* m_aptrs;

    size_t    m_posEnumKey;
    ptrdiff_t m_posEnumVal;

    bool m_bSerialize;
    bool m_bIgnoreCase;
};

class ttCCritSection;  // forward definition

// This class is primarily designed for use with localized id/string pairs, which is why it doesn't have as much
// functionality as the other string list classes.
class ttCIntStrList
{
public:
    ttCIntStrList(void);  // Until issue #43 (https://github.com/KeyWorksRW/keyBld/issues/43) gets implemented
    ~ttCIntStrList();

    // returns pointer to the duplicated string, not the original
    const char* Add(size_t id, const char* psz);
    const char* Find(size_t id);
    // returns the number of unique id/string pairs added (duplicates ids are not added)
    size_t GetCount() const { return m_cItems; }

    // resets the list to empty state (frees all string memory).
    void Delete();

    bool InRange(size_t pos) const { return (pos < m_cItems && m_cItems > 0); }

private:
    typedef struct
    {
        size_t      id;
        const char* psz;
    } KEYVAL_PAIR;

    // Class members

    size_t          m_cItems;
    size_t          m_cAllocated;
    KEYVAL_PAIR*    m_aData;
    ttCCritSection* m_pcrit;
};
