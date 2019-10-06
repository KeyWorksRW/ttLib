/////////////////////////////////////////////////////////////////////////////
// Name:      ttCritSection, ttCCritLock
// Purpose:   Class for creating, locking, and unlocking a critical section
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Header-only class

#pragma once

class ttCCritSection
{
public:
#if defined(_WIN32)

    ttCCritSection()
    {
        memset(&m_cs, 0, sizeof(CRITICAL_SECTION));
        InitializeCriticalSection(&m_cs);
    };
    ~ttCCritSection() { DeleteCriticalSection(&m_cs); };

    void Lock() { EnterCriticalSection(&m_cs); }
    void Unlock() { LeaveCriticalSection(&m_cs); }

    // Class functions

private:
    // Class members

    CRITICAL_SECTION m_cs;

#else   // not defined(_WIN32)
    wxCriticalSection m_cs;

    void Lock() { m_cs.Enter(); }
    void Unlock() { m_cs.Leave(); }
#endif  // defined(_WIN32)
};      // end ttCCritSection

/*
    Designed to keep a Critical Section locked until the destructor is called

    Example:
        ttCCritSection csFoo;

        if (somecondition) {
            ttCCritLock cl(&csFoo); // locks csFoo
            ...
        }

        // exiting the scope automatically unlocks csFoo

*/

class ttCCritLock
{
public:
    ttCCritLock(ttCCritSection* pcs)
    {
        if (!pcs)
        {
            m_pcs = nullptr;
            return;
        }
        m_pcs = pcs;
        pcs->Lock();
    }
    ~ttCCritLock()
    {
        if (m_pcs)
            m_pcs->Unlock();
    }
    void Unlock()
    {
        if (!m_pcs)
            return;
        m_pcs->Unlock();
        m_pcs = nullptr;
    }

    ttCCritSection* m_pcs;
};
