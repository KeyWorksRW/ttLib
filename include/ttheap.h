/////////////////////////////////////////////////////////////////////////////
// Name:      ttCHeap
// Purpose:   Class for utilizing Windows heap manager
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(_WIN32) && !defined(PTEST)

#include <wtypes.h>

/*
    The Windows heap manager is a bit faster to use then the C runtime. Replacing the standard memory allocation
    functions provides a performance boost and eliminates the need to check for a null pointer on return. If there is
    insufficient memory, the application will be terminated (see OOM()).

    ttCHeap can also be used to create a sub-heap. Any individual memory allocations on the sub-heap do not need to be
    freed before the destructor as the entire sub-heap is destroyed at once. A class that wants to utilize this
    functionality should inherit from ttCHeap, and provide a serialization flag in it's constructor:

        classs MyClass :  public ttCHeap
        {
            MyClass() : ttCHeap(true) { } // true to make MyClass thread-safe

    Now all of the ttMalloc/ttReAlloc/ttReCalloc routines below will be allocated on the sub-heap, and do not need to
    be specifically FreeAllocd in the destructor.

    Constructing ttCHeap using another heap takes advantage of the sub-heap above by elimintating the need to
    individually free every memory allocation in the destructor.
*/

class ttCHeap
{
public:
    ttCHeap();

    // Creates a sub-heap. Use true for thread safe, false for speed (but not thread safe)
    ttCHeap(bool bSerialize);

    // Pass in a heap handle or another ttCHeap class (which will call the HANDLE() operator)
    ttCHeap(HANDLE hHeap);

    ~ttCHeap();

    // Public functions

    // We use the "tt" prefix to make certain there is no confusion in a derived class that the memory routines are from
    // this class rather than the standard memory functions.

    // In Debug builds, this will fill the allocated memory with 0xCD.
    void* ttMalloc(size_t cb);
    void* ttCalloc(size_t cb);
    void* ttReAlloc(void* pv, size_t cb);
    void* ttReCalloc(void* pv, size_t cb);

    void ttFree(void* pv)
    {
        if (pv)
            HeapFree(m_hHeap, 0, pv);
    }

    // identical to ttFree
    void ttDelete(void* pv)
    {
        if (pv)
            HeapFree(m_hHeap, 0, pv);
    }

    char*    ttStrDup(const char* psz);
    wchar_t* ttStrDup(const wchar_t* pwsz);

    // allocates/ReAllocates *ppszDst
    char*    ttStrDup(const char* psz, char** ppszDst);
    wchar_t* ttStrDup(const wchar_t* psz, wchar_t** ppszDst);

    size_t ttSize(const void* pv) { return pv ? HeapSize(m_hHeap, 0, pv) : 0; }
    bool   ttValidate(const void* pv) { return HeapValidate(m_hHeap, 0, pv); }

    // Sub-heap has been created (rather then using ProcessHeap or a master ttCHeap class)
    bool isCreated() { return m_bCreated; }
    bool isMainHeap() { return (m_hHeap && m_hHeap == GetProcessHeap()); }

    void DeleteAll();

    operator HANDLE() const { return m_hHeap; }

protected:
    // Class members

    HANDLE m_hHeap;
    bool   m_bCreated;

private:
    bool m_bSerialize;

};  // end ttCHeap

namespace tt
{
    extern ttCHeap MainHeap;  // this uses the process heap rather then a sub-heap
}

#endif  // defined(_WIN32)
