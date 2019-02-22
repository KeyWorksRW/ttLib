/////////////////////////////////////////////////////////////////////////////
// Name:		basethread.h
// Purpose:
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

namespace ttpriv {
	unsigned __stdcall _BaseThread(void* pv);
}

class ttBaseThread
{
public:
	ttBaseThread();
	~ttBaseThread();

	// Class functions

	void StopThread();
	void StartThread();
	void InitializeThreadForOle();	// Call this in doThreadWork() if you need Ole support
	void WaitForThreadToComplete();

	virtual void doThreadWork() = NULL;		// Derived class MUST supply this!
	virtual void SetCancelThreadPending() { m_bCancelThread = true; }
	virtual bool isCancelThreadPending() { return m_bCancelThread; }

	bool	m_bCancelThread;

protected:

	friend unsigned __stdcall ttpriv::_BaseThread(void* pv);

	// Class members

	HANDLE  m_hthrdWorker;
	bool	m_bOleInitialized;


}; // end ttBaseThread
