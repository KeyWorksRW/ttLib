/////////////////////////////////////////////////////////////////////////////
// Name:      ttCMultiThrd
// Purpose:   Class for utilizing Windows heap manager
// Author:    Ralph Walden
// Copyright: Copyright (c) 2010-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/ttdebug.h"        // ttASSERT macros
#include "../include/ttmultithread.h"  // ttCMultiThrd

DWORD __stdcall _ttMultiThread(void* pv);

ttCMultiThrd::ttCMultiThrd()
{
    m_cThreads = 0;
    m_bEndThreads = m_bCanceled = false;
    m_ahsemDone = nullptr;
    m_aThrdInfo = nullptr;
}

ttCMultiThrd::~ttCMultiThrd()
{
    if (m_aThrdInfo)
    {
        m_bEndThreads = true;
        for (size_t iThread = 0; iThread < m_cThreads; iThread++)
        {
            ReleaseSemaphore(m_aThrdInfo[iThread].hsemStart, 1, NULL);    // thread can't exit until you let it start
            WaitForSingleObject(m_aThrdInfo[iThread].hThread, INFINITE);  // wait for thread to terminate
            CloseHandle(m_aThrdInfo[iThread].hsemStart);
            CloseHandle(m_aThrdInfo[iThread].hsemDone);
            CloseHandle(m_aThrdInfo[iThread].hThread);
        }
    }
    if (m_ahsemDone)
        ttFree(m_ahsemDone);
}

void ttCMultiThrd::InitializeThreads(size_t cThreads)  // 0 means create as many threads as there are CPUs
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    int cpus = (int) si.dwNumberOfProcessors;

    ttASSERT(cThreads <= (size_t) cpus);
    ttASSERT_MSG(!m_cThreads, "You cannot call InitializeThreads more then once!");
    if (m_cThreads)
        return;  // already initialized

    if (cThreads == 0)
        cThreads = (size_t) cpus;

    m_cThreads = (cThreads == 0) ? (size_t) cpus : cThreads;

    m_aThrdInfo = (MULTI_THRD_INFO*) ttMalloc(sizeof(MULTI_THRD_INFO) * m_cThreads);
    m_ahsemDone = (HANDLE*) ttMalloc(sizeof(HANDLE) * m_cThreads);

    for (size_t iThread = 0; iThread < cThreads; iThread++)
    {
        m_aThrdInfo[iThread].hsemStart = CreateSemaphore(NULL, 0, 1, NULL);
        m_aThrdInfo[iThread].hsemDone = CreateSemaphore(NULL, 1, 1, NULL);
        DWORD thrdID;
        // REVIEW: [ralphw - 05-20-2018] Should switch to _beginthreadex() for portability
        m_aThrdInfo[iThread].hThread = (HANDLE) CreateThread(NULL, 0, _ttMultiThread, (LPVOID) this, 0, &thrdID);
        ttASSERT(m_aThrdInfo[iThread].hThread);
        if (!m_aThrdInfo[iThread].hThread)
        {
            // TODO: [ralphw - 03-02-2010] this would be really bad...
        }
        m_aThrdInfo[iThread].bDone = true;
        m_ahsemDone[iThread] = m_aThrdInfo[iThread].hsemDone;
        m_threadMap.Add(thrdID, &m_aThrdInfo[iThread]);
    }
}

size_t ttCMultiThrd::GetAvailableThreads()
{
    size_t nAvail = 0;
    for (size_t iThread = 0; iThread < m_cThreads; iThread++)
    {
        if (m_aThrdInfo[iThread].bDone)
            nAvail++;
    }
    return nAvail;
}

void ttCMultiThrd::StartThread(void* pvData1, void* pvData2)
{
    ttASSERT_MSG(m_aThrdInfo, "StartThread called before InitializeThreads()");
    if (!m_aThrdInfo)
        InitializeThreads(0);

    DWORD pos = WaitForMultipleObjects((DWORD) m_cThreads, m_ahsemDone, FALSE, INFINITE) - WAIT_OBJECT_0;
    ttASSERT(pos >= 0 && pos < (DWORD) m_cThreads);
    m_aThrdInfo[pos].bDone = false;
    m_aThrdInfo[pos].pvData1 = pvData1;
    m_aThrdInfo[pos].pvData2 = pvData2;
    ReleaseSemaphore(m_aThrdInfo[pos].hsemStart, 1, NULL);
}

void ttCMultiThrd::CancelThreads()
{
    m_bCanceled = true;
    WaitForThreadsToComplete();
    m_bCanceled = false;
}

void ttCMultiThrd::WaitForThreadsToComplete()
{
    WaitForMultipleObjects((DWORD) m_cThreads, m_ahsemDone, TRUE, INFINITE);
    // All Done semaphores are now non-signalled, so we need to signal them all so that we can wait on them again
    for (size_t iThread = 0; iThread < m_cThreads; iThread++)
        ReleaseSemaphore(m_ahsemDone[iThread], 1, NULL);
}

size_t ttGetCPUCount()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    auto cpus = si.dwNumberOfProcessors;
    return (size_t) cpus;
}

DWORD __stdcall _ttMultiThread(void* pv)
{
    ttCMultiThrd* pThis = (ttCMultiThrd*) pv;
    DWORD         thrdID = GetCurrentThreadId();
    auto          pos = pThis->m_threadMap.FindKey(thrdID);
    ttASSERT(pos >= 0);  // theoretically impossible
    ttCMultiThrd::MULTI_THRD_INFO* pThrdInfo = pThis->m_threadMap.GetValueAt(pos);

    for (;;)
    {
        if (WaitForSingleObject(pThrdInfo->hsemStart, INFINITE) != WAIT_OBJECT_0)
            break;
        if (pThis->m_bEndThreads || pThis->m_bCanceled)
            return 0;

        try
        {
            pThis->DoThreadWork(pThrdInfo->pvData1, pThrdInfo->pvData2);
        }
        catch (...)
        {
            ttFAIL("Exception in doThreadWork()");
        }

        pThrdInfo->bDone = true;
        ReleaseSemaphore(pThrdInfo->hsemDone, 1, NULL);
    }
    return 0;
}
