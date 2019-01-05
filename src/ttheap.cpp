/////////////////////////////////////////////////////////////////////////////
// Name:		CTTHeap
// Purpose:		Class for utilizing Windows heap manager
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"		// precompiled header

#include "../include/asserts.h"
#include "../include/ttheap.h"		// CTTHeap

using namespace tt;

CTTHeap	tt::MainHeap;

/*
	CTTHeap can be constructed in one of three ways:
		1) no parameter -> uses the process heap, memory is NOT freed in destructor
		2) bool -> creates a sub-heap, the parameter indicates if the sub-heap should be thread-safe
		3) heap handle -> uses another sub-heap, sub-heap is not freed in destructor

		#3 is typically used when you want a master CTTHeap class that will share it's sub-heap with all child CTTHeap classes, and
		free the entire sub-heap at once.

			CTTHeap master(true);
			CTTHeap child(master);	// this invokes master::HANDLE() and from then on use master's sub-heap

*/

CTTHeap::CTTHeap()
{
	m_hHeap = GetProcessHeap();
	m_bCreated = false;	// prevent deleting heap in destructor
}

CTTHeap::CTTHeap(bool bSerialize)
{
	m_hHeap = HeapCreate(bSerialize ? 0 : HEAP_NO_SERIALIZE, 4096, 0);
	ASSERT_MSG(m_hHeap, "Unable to create heap");
	if (m_hHeap == nullptr) {	// if we can't create a new heap, switch to the process heap
		m_bCreated = false;
		m_hHeap = GetProcessHeap();
	}

	m_bCreated = true;
}

CTTHeap::CTTHeap(HANDLE hHeap)
{
	ASSERT(hHeap);
	m_hHeap = hHeap;
	m_bCreated = false;	// prevent deleting heap in destructor
}

bool CTTHeap::CreateHeap(bool bSerialize)
{
#ifdef _WINDOWS_
	if (m_bCreated && m_hHeap && m_hHeap != GetProcessHeap)
		HeapDestroy(m_hHeap);
	m_hHeap = HeapCreate(bSerialize ? 0 : HEAP_NO_SERIALIZE, 4096, 0);
	ASSERT_MSG(m_hHeap, "Unable to create heap");
	if (m_hHeap == nullptr) {	// if we can't create a new heap, switch to the process heap
		m_bCreated = false;
		m_hHeap = GetProcessHeap();
	}
	else
		m_bCreated = true;
	return m_bCreated;
#else
	return false;	// TODO: [randalphwa - 08-29-2018] Replace this with something that will work on POSIX
#endif
};

void CTTHeap::Attach(HANDLE hHeap)
{
	ASSERT(hHeap);
	ASSERT(!m_bCreated || m_hHeap == GetProcessHeap());
	if (!hHeap || m_bCreated || m_hHeap != GetProcessHeap())
		return;		// destroying an existing sub-heap to attach to a new one would destroy any previous allocations

	m_bCreated = false;		// this keeps us from destroying the attatched heap during our destructor
	m_hHeap = hHeap;
}

CTTHeap::~CTTHeap()
{
	if (m_bCreated && m_hHeap && m_hHeap != GetProcessHeap())
		HeapDestroy(m_hHeap);
}

void* CTTHeap::ttMalloc(size_t cb)
{
	void* pv = HeapAlloc(m_hHeap, 0, cb);
	ASSERT(pv);
	if (!pv)
		OOM();
#ifdef _DEBUG
	memset(pv, 0xCD, cb);
#endif
	return pv;
}

void* CTTHeap::ttCalloc(size_t cb)
{
	void* pv = HeapAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, cb);
	ASSERT(pv);
	if (!pv)
		OOM();
	return pv;
}

void* CTTHeap::ttRealloc(void* pv, size_t cb)
{
	if (!pv)
		return ttMalloc(cb);
	pv = HeapReAlloc(m_hHeap, 0, pv, cb);
	if (!pv)
		OOM();
	return pv;
}

void* CTTHeap::ttRecalloc(void* pv, size_t cb)
{
	if (!pv)
		return ttCalloc(cb);
	pv = HeapReAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, pv, cb);
	if (!pv)
		OOM();
	return pv;
}

void CTTHeap::ttFree(void* pv)
{
	if (pv)
		HeapFree(m_hHeap, 0, pv);
}

size_t CTTHeap::ttSize(const void* pv)
{
	ASSERT(pv);
	if (pv)
		return HeapSize(m_hHeap, 0, pv);
	else
		return 0;
}

char* CTTHeap::ttStrdup(const char* psz)
{
	ASSERT_MSG(psz, "null pointer!");

	if (!psz || !*psz)
		psz = "";

	size_t cb = kstrbyte(psz);
	char* pszDst = (char*) ttMalloc(cb);
	memcpy(pszDst, psz, cb);
	return pszDst;
}

wchar_t* CTTHeap::ttStrdup(const wchar_t* pwsz)
{
	ASSERT_MSG(pwsz, "null pointer!");

	if (!pwsz || !*pwsz)
		pwsz = L"";

	size_t cb = kstrbyte(pwsz);
	wchar_t* pwszDst = (wchar_t*) ttMalloc(cb);
	memcpy(pwszDst, pwsz, cb);
	return pwszDst;
}
