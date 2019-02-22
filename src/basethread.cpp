/////////////////////////////////////////////////////////////////////////////
// Name:		basethread.cpp
// Purpose:		Base class for a single thread
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2004-2018 KeyWorks Software (Ralph Walden)
// Licence:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <process.h>

#include "../include/ttbasethread.h"	// ttBaseThread
#include "../include/ttdebug.h" 		// ttASSERT macros

ttBaseThread::ttBaseThread ()
{
	m_hthrdWorker = NULL;
	m_bCancelThread = false;
	m_bOleInitialized = false;
};

ttBaseThread::~ttBaseThread ()
{
	StopThread();
};

void ttBaseThread::StartThread()
{
	ttASSERT(m_hthrdWorker == NULL);
	if (m_hthrdWorker)
		return;		// thread has already been started

	m_hthrdWorker = (HANDLE) _beginthreadex(NULL, 0, ttpriv::_BaseThread, this, 0, NULL);
}

void ttBaseThread::StopThread()
{
	if (m_hthrdWorker) {
		SetCancelThreadPending();
		WaitForSingleObject(m_hthrdWorker, INFINITE);
		CloseHandle(m_hthrdWorker);
		m_hthrdWorker = NULL;
	}
}

void ttBaseThread::WaitForThreadToComplete()
{
	if (m_hthrdWorker) {
		WaitForSingleObject(m_hthrdWorker, INFINITE);
		CloseHandle(m_hthrdWorker);
		m_hthrdWorker = NULL;
	}
}

void ttBaseThread::InitializeThreadForOle()
{
	HRESULT hr = ::CoInitialize(NULL);
	if (SUCCEEDED(hr)) {
		m_bOleInitialized = true;
	}
#ifdef _DEBUG
	else {
		ttASSERT_HRESULT(hr, "CoInitialize Failed");
	}
#endif
}

unsigned __stdcall ttpriv::_BaseThread(void* pv)
{
	ttBaseThread* pThis = (ttBaseThread*) pv;
	pThis->doThreadWork();

	if (pThis->m_bOleInitialized)
		::CoUninitialize();

	return 0;
}
