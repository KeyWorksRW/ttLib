/////////////////////////////////////////////////////////////////////////////
// Name:      ttCMultiThrd, CMultiChildThread
// Purpose:   Class for handling multiple thread
// Author:    Ralph Walden
// Copyright: Copyright (c) 2010-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

// This class creates a pool of threads which are created with a call to IniatializeThreads(nThreads) specifying how
// many threads you want. The threads will exist until this class's destructor is called.

// Inherit from this class and supply a doThreadWork(pvData1, pvData2) function.

// To use a thread, call StartThread(pvData1, pvData2) -- this will in turn call doThreadWork(pvData1, pvData2) with
// the same parameters you passed to StartThread.

// Call GetAvailableThreads() to see how many threads are currently available (keeping in mind that this is simply a
// snapshot -- by the time this function returns, other threads may have completed their work).

// Call WaitForThreadsToComplete() to wait for all threads to finish normally, or call CancelThreads() to finish
// threads early (this requires you to check for isCancelled() in your doThreadWork function). CancelThreads cancels
// the current task and the puts the thread back into the pool

// a try/catch section is placed around the call to doThreadWork

#include "ttmap.h"  // ttCMap

size_t ttGetCPUCount();

class ttCMultiThrd
{
public:
    ttCMultiThrd();
    ~ttCMultiThrd();

    virtual void DoThreadWork(
        void* pvData1,
        void* pvData2) = NULL;  // Derived class MUST supply this! While running, call isCancelled() to return

    bool isCancelled() { return m_bCanceled; }  // true if threads are being aborted
    void CancelThreads();

    void InitializeThreads(size_t nThreads = 0);     // 0 means create as many threads as there are CPUs
    void StartThread(void* pvData1, void* pvData2);  // will not return until an available thread is found

    size_t GetAvailableThreads();  // returns currently available threads

    void WaitForThreadsToComplete();  // waits for all threads to finish, then returns

protected:
    size_t m_cThreads;

    bool m_bEndThreads;
    bool m_bCanceled;

    typedef struct
    {
        HANDLE hThread;
        HANDLE hsemStart;
        HANDLE hsemDone;
        void*  pvData1;  // additional data passed to the worker thread
        void*  pvData2;  // additional data passed to the worker thread
        bool   bDone;
    } MULTI_THRD_INFO;

    ttCMap<DWORD /* threadID */, MULTI_THRD_INFO*> m_threadMap;

    friend DWORD __stdcall _ttMultiThread(void* pv);
    MULTI_THRD_INFO* m_aThrdInfo;
    HANDLE*          m_ahsemDone;
};
