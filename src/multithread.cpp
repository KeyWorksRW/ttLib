/////////////////////////////////////////////////////////////////////////////
// Name:		ttMultiThrd
// Purpose:		Class for utilizing Windows heap manager
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2010-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../include/multithread.h"	// ttMultiThrd

#ifndef _WX_WX_H_
	DWORD __stdcall _MultiThread(void* pv);
#endif

ttMultiThrd::ttMultiThrd()
{
	m_cThreads = 0;
	m_bEndThreads = m_bCanceled = false;
	m_ahsemDone = nullptr;
	m_aThrdInfo = nullptr;
#ifdef _WX_WX_H_
	m_aThrds = nullptr;
	m_psemAvailThrd = new wxSemaphore(0, 1);
#endif
}

ttMultiThrd::~ttMultiThrd()
{
#ifdef _WX_WX_H_
	if (m_aThrds) {
		m_bCanceled = true;
		wxSemaphore semTerminate;
		for (size_t iThread = 0; iThread < m_cThreads; iThread++) {
			// whenever the thread is activated, it checks for m_psemTerminate != null -- and if so, the thread signals it and exits
			m_aThrds[iThread]->m_psemTerminate = &semTerminate;
			m_aThrds[iThread]->m_semStart.Post();
			semTerminate.Wait();
		}
		kfree(m_aThrds);		// note that we do not call delete on the individual threads as per wxThread docs
	}
	delete m_psemAvailThrd;
#else	// not _WX_WX_H_
	if (m_aThrdInfo) {
		m_bEndThreads = true;
		for (size_t iThread = 0; iThread < m_cThreads; iThread++) {
			ReleaseSemaphore(m_aThrdInfo[iThread].hsemStart, 1, NULL);		// thread can't exit until you let it start
			WaitForSingleObject(m_aThrdInfo[iThread].hThread, INFINITE);	// wait for thread to terminate
			CloseHandle(m_aThrdInfo[iThread].hsemStart);
			CloseHandle(m_aThrdInfo[iThread].hsemDone);
			CloseHandle(m_aThrdInfo[iThread].hThread);
		}
	}
	if (m_ahsemDone)
		tt::free(m_ahsemDone);
#endif	// _WX_WX_H_
}

void ttMultiThrd::InitializeThreads(size_t cThreads)	// 0 means create as many threads as there are CPUs
{
#ifdef	_WX_WX_H_
	int cpus = wxThread::GetCPUCount();
	if (cpus == -1)
		cpus = 1;
#else	// not _WX_WX_H_
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpus = (int) si.dwNumberOfProcessors;
#endif	// _WX_WX_H_

	ttASSERT(cThreads <= (size_t) cpus);
	ttASSERT_MSG(!m_cThreads, "You cannot call InitializeThreads more then once!");
	if (m_cThreads)
		return;	// already initialized

	if (cThreads == 0)
		cThreads = (size_t) cpus;

	m_cThreads = (cThreads == 0) ? (size_t) cpus : cThreads;

#ifdef	_WX_WX_H_
	m_aThrds = (CMultiChildThread**) tt::malloc(sizeof(CMultiChildThread**) * m_cThreads);
#else	// not _WX_WX_H_
	m_aThrdInfo = (MULTI_THRD_INFO*) tt::malloc(sizeof(MULTI_THRD_INFO) * m_cThreads);
	m_ahsemDone = (HANDLE*) tt::malloc(sizeof(HANDLE) * m_cThreads);
#endif	// _WX_WX_H_

	for (size_t iThread = 0; iThread < cThreads; iThread++) {
#ifdef	_WX_WX_H_
		m_aThrds[iThread] = new CMultiChildThread;
		m_aThrds[iThread]->m_fDone = true;
		m_aThrds[iThread]->m_psemAvailThrd = m_psemAvailThrd;
		m_aThrds[iThread]->m_psemTerminate = nullptr;
		m_aThrds[iThread]->m_pMultiThrd = this;
#else	// not _WX_WX_H_
		m_aThrdInfo[iThread].hsemStart = CreateSemaphore(NULL, 0, 1, NULL);
		m_aThrdInfo[iThread].hsemDone  = CreateSemaphore(NULL, 1, 1, NULL);
		DWORD thrdID;
		// REVIEW: [ralphw - 05-20-2018] Should switch to _beginthreadex() for portability
		m_aThrdInfo[iThread].hThread  = (HANDLE) CreateThread(NULL, 0, _MultiThread, (LPVOID) this, 0, &thrdID);
		ttASSERT(m_aThrdInfo[iThread].hThread);
		if (!m_aThrdInfo[iThread].hThread) {
			// TODO: [ralphw - 03-02-2010] this would be really bad...
		}
		m_aThrdInfo[iThread].bDone = true;
		m_ahsemDone[iThread] = m_aThrdInfo[iThread].hsemDone;
		m_threadMap.Add(thrdID, &m_aThrdInfo[iThread]);
#endif	// _WX_WX_H_
	}
}

size_t ttMultiThrd::GetAvailableThreads()
{
	size_t nAvail = 0;
	for (size_t iThread = 0; iThread < m_cThreads; iThread++) {
#ifdef	_WX_WX_H_
		if (m_aThrds[iThread]->m_fDone) {
#else
		if (m_aThrdInfo[iThread].bDone) {
#endif
			nAvail++;
		}
	}
	return nAvail;
}

void ttMultiThrd::StartThread(void* pvData1, void* pvData2)
{
#ifdef	_WX_WX_H_
	if (!m_aThrds)
		InitializeThreads(0);
	if (GetAvailableThreads() == 0)
		m_psemAvailThrd->Wait();	// wait for at least one thread to become available

	size_t iThread;
	for (iThread = 0; iThread < m_cThreads; iThread++) {
		if (m_aThrds[iThread]->m_fDone)
			break;
	}
	ttASSERT(iThread < m_cThreads);	// theoretically impossible

	m_aThrds[iThread]->m_fDone = false;
	m_aThrds[iThread]->m_pvData1 = pvData1;
	m_aThrds[iThread]->m_pvData2 = pvData2;
	m_aThrds[iThread]->m_semStart.Post();
#else
	ttASSERT_MSG(m_aThrdInfo, "StartThread called before InitializeThreads()");
	if (!m_aThrdInfo)
		InitializeThreads(0);

	DWORD pos = WaitForMultipleObjects((DWORD) m_cThreads, m_ahsemDone, FALSE, INFINITE) - WAIT_OBJECT_0;
	ttASSERT(pos >= 0 && pos < (DWORD) m_cThreads);
	m_aThrdInfo[pos].bDone = false;
	m_aThrdInfo[pos].pvData1 = pvData1;
	m_aThrdInfo[pos].pvData2 = pvData2;
	ReleaseSemaphore(m_aThrdInfo[pos].hsemStart, 1, NULL);
#endif
}

void ttMultiThrd::CancelThreads()
{
	m_bCanceled = true;
	WaitForThreadsToComplete();
	m_bCanceled = false;
}

void ttMultiThrd::WaitForThreadsToComplete()
{
#ifdef	_WX_WX_H_
	for (size_t iThread = 0; iThread < m_cThreads; iThread++) {
		while (!m_aThrds[iThread]->m_fDone)
			m_psemAvailThrd->Wait();	// any thread can complete and signal this, so loop until this one is done
	}
#else	// not _WX_WX_H_
	WaitForMultipleObjects((DWORD) m_cThreads, m_ahsemDone, TRUE, INFINITE);
	// All Done semaphores are now non-signalled, so we need to signal them all so that we can wait on them again
	for (size_t iThread = 0; iThread < m_cThreads; iThread++)
		ReleaseSemaphore(m_ahsemDone[iThread], 1, NULL);
#endif	// _WX_WX_H_
}

size_t tt::GetCPUCount()
{
#ifdef	_WX_WX_H_
	auto cpus = wxThread::GetCPUCount();
	if (cpus == -1)
		cpus = 1;
#else	// not _WX_WX_H_
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	auto cpus = si.dwNumberOfProcessors;
#endif	// _WX_WX_H_
	return (size_t) cpus;
}

#ifdef	_WX_WX_H_
wxThread::ExitCode CMultiChildThread::Entry()
{
	for (;;) {
		m_semStart.Wait();
		m_fDone = false;
		if (m_psemTerminate) {
			m_psemTerminate->Post();	// awknowledge that we are terminating
			Exit();
			return 0;
		}
		try {
			m_pMultiThrd->doThreadWork(m_pvData1, m_pvData2);
		}
		catch (...) {
			wxFAIL_MSG("Exception in doThreadWork()");
		}

		m_fDone = true;
		m_psemAvailThrd->Post();	// used to indicate that at least one thread is available
	}
}
#else	// not _WX_WX_H_

DWORD __stdcall _MultiThread(void* pv)
{
	ttMultiThrd* pThis = (ttMultiThrd*) pv;
	DWORD thrdID = GetCurrentThreadId();
	ptrdiff_t pos = pThis->m_threadMap.FindKey(thrdID);
	ttASSERT(pos >= 0);	// theoretically impossible
	ttMultiThrd::MULTI_THRD_INFO* pThrdInfo = pThis->m_threadMap.GetValueAt(pos);

	for (;;) {
		if (WaitForSingleObject(pThrdInfo->hsemStart, INFINITE) != WAIT_OBJECT_0)
			break;
		if (pThis->m_bEndThreads || pThis->m_bCanceled)
			return 0;

		try {
			pThis->doThreadWork(pThrdInfo->pvData1, pThrdInfo->pvData2);
		}
		catch (...) {
#ifdef _DEBUG
			tt::CATCH_HANDLER("%s (%d) : Exception in doThreadWork()", __FILE__, __LINE__);
#endif
		}

		pThrdInfo->bDone = true;
		ReleaseSemaphore(pThrdInfo->hsemDone, 1, NULL);
	}
	return 0;
}

#endif	// _WX_WX_H_