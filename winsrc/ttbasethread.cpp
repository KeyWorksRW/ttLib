/////////////////////////////////////////////////////////////////////////////
// Name:      basethread.cpp
// Purpose:   Base class for a single thread
// Author:    Ralph Walden
// Copyright: Copyright (c) 2004-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#if !defined(TTALL_LIB)
    #if defined(NDEBUG)
        #pragma comment(lib, "ttLibwin.lib")
    #else
        #pragma comment(lib, "ttLibwinD.lib")
    #endif
#endif

#include <process.h>

#include "../include/ttbasethread.h"  // ttCBaseThread
#include "../include/ttdebug.h"       // ttASSERT macros

ttCBaseThread::ttCBaseThread()
{
    m_hthrdWorker = NULL;
    m_bCancelThread = false;
    // m_bOleInitialized = false;
};

ttCBaseThread::~ttCBaseThread()
{
    StopThread();
};

void ttCBaseThread::StartThread()
{
    ttASSERT(m_hthrdWorker == NULL);
    if (m_hthrdWorker)
        return;  // thread has already been started

    m_hthrdWorker = (HANDLE) _beginthreadex(NULL, 0, ttpriv::_BaseThread, this, 0, NULL);
}

void ttCBaseThread::StopThread()
{
    if (m_hthrdWorker)
    {
        SetCancelThreadPending();
        WaitForSingleObject(m_hthrdWorker, INFINITE);
        CloseHandle(m_hthrdWorker);
        m_hthrdWorker = NULL;
    }
}

void ttCBaseThread::WaitForThreadToComplete()
{
    if (m_hthrdWorker)
    {
        WaitForSingleObject(m_hthrdWorker, INFINITE);
        CloseHandle(m_hthrdWorker);
        m_hthrdWorker = NULL;
    }
}

#if 0
void ttCBaseThread::InitializeThreadForOle()
{
    HRESULT hr = ::CoInitialize(NULL);
    if (SUCCEEDED(hr))
        m_bOleInitialized = true;
    #ifdef _DEBUG
    else
        ttASSERT_HRESULT(hr, "CoInitialize Failed");
    #endif
}
#endif

unsigned __stdcall ttpriv::_BaseThread(void* pv)
{
    ttCBaseThread* pThis = (ttCBaseThread*) pv;
    pThis->DoThreadWork();

    //    if (pThis->m_bOleInitialized)
    //        ::CoUninitialize();

    return 0;
}
