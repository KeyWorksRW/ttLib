/////////////////////////////////////////////////////////////////////////////
// Name:      ttmem.h
// Purpose:   Header-only memory classes
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// ttCMem and ttCTMem allow you to allocate memory that will automatically be freed when the class gets destroyed.

#pragma once

#ifndef __TTLIB_TTMEM_H__
#define __TTLIB_TTMEM_H__

#include "ttdebug.h"  // ttASSERT macros
#include "ttheap.h"   // ttCHeap

// Header-only class for unsigned character type (uint8_t*) (works fine for zero-terminated strings and binary data).
class ttCMem
{
public:
    ttCMem(void) { m_pb = nullptr; }
    ttCMem(size_t size) { m_pb = (uint8_t*) ttMalloc(size); }
    ~ttCMem(void)
    {
        if (m_pb)
            ttFree(m_pb);
    }
    void resize(size_t cb)
    {
        if (!m_pb)
            m_pb = (uint8_t*) ttMalloc(cb);
        else
            m_pb = (uint8_t*) ttReAlloc(m_pb, cb);
    }
    size_t size() { return ttSize(m_pb); }

    operator void*() { return (void*) m_pb; };
    operator const char*() { return (const char*) m_pb; };
    operator char*() { return (char*) m_pb; };
    operator uint8_t*() { return m_pb; };

    uint8_t* m_pb;
};

template<typename T>
class ttCTMem  // Header-only class
{
public:
    ttCTMem() { m_p = NULL; }
    ttCTMem(size_t size) { m_p = (T) ttMalloc(size); }
    ~ttCTMem()
    {
        if (m_p)
            ttFree(m_p);
    }

    void resize(size_t cb)
    {
        if (!m_p)
            m_p = (T) ttMalloc(cb);
        else
            m_p = (T) ttReAlloc(m_p, cb);
    }
    size_t size() { return ttSize(m_p); }

    operator T()
    {
        ttASSERT(m_p);
        return m_p;
    };
    T operator->()
    {
        ttASSERT(m_p);
        return m_p;
    };

    T m_p;
};

#endif  // __TTLIB_TTMEM_H__
