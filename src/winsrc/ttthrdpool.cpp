/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::ThrdPool
// Purpose:   Class for utilizing Windows heap manager
// Author:    Ralph Walden
// Copyright: Copyright (c) 2010-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <cassert>
#include <exception>

#include "ttdebug.h"
#include "ttthrdpool.h"

using namespace ttlib;

ThrdPool::ThrdPool(ThreadFunction function, int maxthreads)
{
    m_function = function;

    SYSTEM_INFO si;
    GetSystemInfo(&si);

    // Always leave one CPU available. It will remain available to the caller for handling UI, I/O, etc.
    m_cThreads = si.dwNumberOfProcessors - 1;

    // Allow the caller to override the number of threads, but only if it is within a usable range.
    if (maxthreads > 0 && maxthreads < static_cast<int>(m_cThreads))
        m_cThreads = maxthreads;

    m_threads.resize(m_cThreads);
    m_ahsemDone = static_cast<HANDLE*>(std::malloc(sizeof(HANDLE) * m_cThreads));
    if (!m_ahsemDone)
        throw std::bad_alloc();

    DWORD thrd;
    for (thrd = 0; thrd < m_cThreads; ++thrd)
    {
        auto& ThreadInfo = m_threads[thrd];
        ThreadInfo.hsemStart = CreateSemaphore(NULL, 0, 1, NULL);
        ThreadInfo.hsemDone = CreateSemaphore(NULL, 1, 1, NULL);
        m_ahsemDone[thrd] = ThreadInfo.hsemDone;

        ThreadInfo.hThread = CreateThread(NULL, 0, PoolThread, static_cast<LPVOID>(this), 0, &ThreadInfo.thrdID);
        assert(ThreadInfo.hThread);
        if (!ThreadInfo.hThread)
        {
            --m_cThreads;
            continue;
        }
    }
};

ThrdPool::~ThrdPool()
{
    m_isEndAllThreads = true;

    for (auto& ThrdInfo: m_threads)
    {
        ReleaseSemaphore(ThrdInfo.hsemStart, 1, NULL);    // thread can't exit until you let it start
        WaitForSingleObject(ThrdInfo.hThread, INFINITE);  // wait for thread to terminate
        CloseHandle(ThrdInfo.hsemStart);
        CloseHandle(ThrdInfo.hsemDone);
        CloseHandle(ThrdInfo.hThread);
    }

    if (m_ahsemDone)
        std::free(m_ahsemDone);
}

void ThrdPool::StartThread(void* pData)
{
    DWORD pos = WaitForMultipleObjects(m_cThreads, m_ahsemDone, FALSE, INFINITE) - WAIT_OBJECT_0;
    assert(pos >= 0 && pos < m_cThreads);
    m_threads[pos].pData = pData;
    ReleaseSemaphore(m_threads[pos].hsemStart, 1, NULL);
}

void ThrdPool::WaitForThreadsToComplete()
{
    // Wait until all threads are done.
    WaitForMultipleObjects(m_cThreads, m_ahsemDone, TRUE, INFINITE);

    // All Done semaphores are now non-signalled, so we need to signal them all so that we can wait on them again
    for (size_t thrd = 0; thrd < m_cThreads; thrd++)
        ReleaseSemaphore(m_ahsemDone[thrd], 1, NULL);
}

DWORD WINAPI ttlib::PoolThread(void* pv)
{
    auto ThrdPool = reinterpret_cast<ttlib::ThrdPool*>(pv);
    DWORD thrdID = GetCurrentThreadId();
    for (auto& thrdInfo: ThrdPool->m_threads)
    {
        if (thrdInfo.thrdID == thrdID)
        {
            for (;;)
            {
                if (WaitForSingleObject(thrdInfo.hsemStart, INFINITE) != WAIT_OBJECT_0)
                    break;
                if (ThrdPool->m_isEndAllThreads)
                    return 0;

                try
                {
                    ThrdPool->m_function(thrdInfo.pData);
                }
                catch (...)
                {
                    // This catch is a last-ditch effort to keep the program from crashing. The caller should
                    // have handled exceptions in m_function().
                    TT_ASSERT(false);
                }

                ReleaseSemaphore(thrdInfo.hsemDone, 1, NULL);
            }
            break;
        }
    }
    return 0;
}
