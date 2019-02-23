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
		1) no parameter -> uses the process heap, memory is NOT freed in destructor
		2) bool -> creates a sub-heap, the parameter indicates if the sub-heap should be thread-safe or not
		3) heap handle -> uses another sub-heap, sub-heap is not freed in destructor

		#3 is typically used when you want a master ttCHeap class that will share it's sub-heap with all child ttCHeap classes, and
		free the entire sub-heap at once.

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
		tt::OOM();
#ifdef _DEBUG
	memset(pv, 0xCD, cb);
#endif
	return pv;
}

void* ttCHeap::ttCalloc(size_t cb)
{
	void* pv = HeapAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, cb);
	if (!pv)
		tt::OOM();
	return pv;
}

void* ttCHeap::ttRealloc(void* pv, size_t cb)
{
	if (!pv)
		return ttMalloc(cb);
	pv = HeapReAlloc(m_hHeap, 0, pv, cb);
	if (!pv)
		tt::OOM();
	return pv;
}

void* ttCHeap::ttRecalloc(void* pv, size_t cb)
{
	if (!pv)
		return ttCalloc(cb);
	pv = HeapReAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, pv, cb);
	if (!pv)
		tt::OOM();
	return pv;
}

char* ttCHeap::ttStrdup(const char* psz)
{
	ttASSERT_MSG(psz, "NULL pointer!");

	if (!psz || !*psz)
		psz = "";

	size_t cb = tt::strbyte(psz);
	char* pszDst = (char*) ttMalloc(cb);
	memcpy(pszDst, psz, cb);
	return pszDst;
}

wchar_t* ttCHeap::ttStrdup(const wchar_t* pwsz)
{
	ttASSERT_MSG(pwsz, "NULL pointer!");

	if (!pwsz || !*pwsz)
		pwsz = L"";

	size_t cb = tt::strbyte(pwsz);
	wchar_t* pwszDst = (wchar_t*) ttMalloc(cb);
	memcpy(pwszDst, pwsz, cb);
	return pwszDst;
}

char* ttCHeap::ttStrdup(const char* pszSrc, char** pszDst)
{
	ttASSERT_MSG(pszSrc, "NULL pointer!");

	if (!pszSrc || !*pszSrc)
		pszSrc = "";
	size_t cb = tt::strbyte(pszSrc);
	*pszDst = *pszDst ? (char*) HeapReAlloc(m_hHeap, 0, *pszDst, cb) : (char*) HeapAlloc(m_hHeap, 0, cb);
	memcpy(*pszDst, pszSrc, cb);
	return *pszDst;
}
