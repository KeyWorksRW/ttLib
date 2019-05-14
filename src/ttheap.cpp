/////////////////////////////////////////////////////////////////////////////
// Name:		ttCHeap
// Purpose:		Class for utilizing Windows heap manager
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"		// precompiled header

#include "../include/ttdebug.h"	// ASSERTs
#include "../include/ttheap.h"	// ttCHeap

ttCHeap tt::MainHeap;

/*
	ttCHeap can be constructed in one of three ways:
		1) no parameter -> uses the process heap, memory is NOT FreeAllocd in destructor
		2) bool -> creates a sub-heap, the parameter indicates if the sub-heap should be thread-safe or not
		3) heap handle -> uses another sub-heap, sub-heap is not FreeAllocd in destructor

		#3 is typically used when you want a master ttCHeap class that will share it's sub-heap with all child ttCHeap classes, and
		FreeAlloc the entire sub-heap at once.

			ttCHeap master(true);
			ttCHeap child(master);	// this invokes master::HANDLE() and from then on use master's sub-heap
*/

ttCHeap::ttCHeap()
{
	m_hHeap = GetProcessHeap();
	m_bCreated = false;	// prevent deleting heap in destructor
}

ttCHeap::ttCHeap(bool bSerialize)
{
	m_hHeap = HeapCreate(bSerialize ? 0 : HEAP_NO_SERIALIZE, 4096, 0);
	ttASSERT_MSG(m_hHeap, "Unable to create heap");
	if (m_hHeap == nullptr) {	// if we can't create a new heap, switch to the process heap
		m_bCreated = false;
		m_hHeap = GetProcessHeap();
	}

	m_bCreated = true;
}

ttCHeap::ttCHeap(HANDLE hHeap)
{
	ttASSERT(hHeap);
	m_hHeap = hHeap;
	m_bCreated = false;	// prevent deleting heap in destructor
}

ttCHeap::~ttCHeap()
{
	if (m_bCreated && m_hHeap && m_hHeap != GetProcessHeap())
		HeapDestroy(m_hHeap);
}

void* ttCHeap::ttMalloc(size_t cb)
{
	void* pv = HeapAlloc(m_hHeap, 0, cb);
	ttASSERT(pv);
	if (!pv)
		ttOOM();
#ifdef _DEBUG
	memset(pv, 0xCD, cb);
#endif
	return pv;
}

void* ttCHeap::ttCalloc(size_t cb)
{
	void* pv = HeapAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, cb);
	if (!pv)
		ttOOM();
	return pv;
}

void* ttCHeap::ttReAlloc(void* pv, size_t cb)
{
	if (!pv)
		return ttMalloc(cb);
	pv = HeapReAlloc(m_hHeap, 0, pv, cb);
	if (!pv)
		ttOOM();
	return pv;
}

void* ttCHeap::ttReCalloc(void* pv, size_t cb)
{
	if (!pv)
		return ttCalloc(cb);
	pv = HeapReAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, pv, cb);
	if (!pv)
		ttOOM();
	return pv;
}

char* ttCHeap::ttStrDup(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");

	if (!psz || !*psz)
		psz = "";

	size_t cb = ttStrByteLen(psz);
	char* pszDst = (char*) ttMalloc(cb);
	memcpy(pszDst, psz, cb);
	return pszDst;
}

wchar_t* ttCHeap::ttStrDup(const wchar_t* pwsz)
{
	ttASSERT_MSG(pwsz, "NULL pointer!");

	if (!pwsz || !*pwsz)
		pwsz = L"";

	size_t cb = ttStrByteLen(pwsz);
	wchar_t* pwszDst = (wchar_t*) ttMalloc(cb);
	memcpy(pwszDst, pwsz, cb);
	return pwszDst;
}

char* ttCHeap::ttStrDup(const char* pszSrc, char** pszDst)
{
	ttASSERT_MSG(pszSrc, "NULL pointer!");

	if (!pszSrc || !*pszSrc)
		pszSrc = "";
	size_t cb = ttStrByteLen(pszSrc);
	*pszDst = *pszDst ? (char*) HeapReAlloc(m_hHeap, 0, *pszDst, cb) : (char*) HeapAlloc(m_hHeap, 0, cb);
	memcpy(*pszDst, pszSrc, cb);
	return *pszDst;
}

#ifndef _INC_STDLIB
	__declspec(noreturn) void __cdecl exit(int _Code);
#endif

// In _DEBUG builds, ttOOM() will provide an option to invoke a debugger before exiting. In non-debug builds it will simply exit.

__declspec(noreturn) void ttOOM(void)
{
#ifdef _DEBUG

#ifdef _WINDOWS_
	int answer = MessageBoxA(GetActiveWindow(), "Out of Memory!!!", "Do you want to call DebugBreak()?", MB_YESNO | MB_ICONERROR);

	if (answer == IDYES)
		DebugBreak();
	// Don't use GetCurrentWindowHandle() since that might only return an active window
	if (tt::hwndMsgBoxParent && IsWindow(tt::hwndMsgBoxParent))
		SendMessage(tt::hwndMsgBoxParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down
#endif	// __WINDOWS_ and _WX_WX_H_

#endif	// _DEBUG

	exit(-1);
}

void*		ttCalloc(size_t cb) { return tt::MainHeap.ttCalloc(cb); }
void*		ttCalloc(size_t num, size_t cb) { return tt::MainHeap.ttCalloc(num * cb); }	// for compatability with C++ standard library
void		ttFree(void* pv) { tt::MainHeap.ttFree(pv); }
void*		ttMalloc(size_t cb) { return tt::MainHeap.ttMalloc(cb); }
void*		ttReAlloc(void* pv, size_t cbNew) { return tt::MainHeap.ttReAlloc(pv, cbNew); }
void*		ttReCalloc(void* pv, size_t cbNew) { return tt::MainHeap.ttReCalloc(pv, cbNew); }
char*		ttStrDup(const char* psz) { return tt::MainHeap.ttStrDup(psz); }
wchar_t*	ttStrDup(const wchar_t* pwsz) { return tt::MainHeap.ttStrDup(pwsz); }
char*		ttStrDup(const char* psz, char** ppszDst) { return tt::MainHeap.ttStrDup(psz, ppszDst); }
size_t		ttSize(const void* pv) { return tt::MainHeap.ttSize(pv); }
bool		ttValidate(const void* pv) { return tt::MainHeap.ttValidate(pv); }
