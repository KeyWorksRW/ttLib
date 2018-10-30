/////////////////////////////////////////////////////////////////////////////
// Name:		CTTHeap
// Purpose:		Class for utilizing Windows heap manager
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// TODO: [ralphw - 07-10-2018] Need to create a POSIX version (for OSX and Unix) -- probably using mmap()

#include "precomp.h"		// precompiled header

#include "../include/asserts.h"
#include "../include/ttheap.h"		// CTTHeap

CTTHeap	_MainHeap;

CTTHeap::CTTHeap()
{
	m_hHeap = GetProcessHeap();
	m_bCreated = false;
#ifdef _DEBUG
	m_SerialHeap = true;	// Process heap is serialized by default
#endif
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
#ifdef _DEBUG
	m_SerialHeap = bSerialize;
#endif
}

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

void* CTTHeap::malloc(size_t cb)
{
	void* pv = HeapAlloc(m_hHeap, 0, cb);
	ASSERT(pv);
	if (!pv)
		OOM();
#ifdef _DEBUG
	memset(pv, 0xCC, cb);
#endif
	return pv;
}

void* CTTHeap::calloc(size_t cb)
{
	void* pv = HeapAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, cb);
	ASSERT(pv);
	if (!pv)
		OOM();
	return pv;
}

void* CTTHeap::realloc(void* pv, size_t cb)
{
	if (!pv)
		return malloc(cb);
	pv = HeapReAlloc(m_hHeap, 0, pv, cb);
	if (!pv)
		OOM();
	return pv;
}

void* CTTHeap::recalloc(void* pv, size_t cb)
{
	if (!pv)
		return calloc(cb);
	pv = HeapReAlloc(m_hHeap, 0 | HEAP_ZERO_MEMORY, pv, cb);
	if (!pv)
		OOM();
	return pv;
}

void CTTHeap::free(void* pv)
{
	if (pv)
		HeapFree(m_hHeap, 0, pv);
}

size_t CTTHeap::size(const void* pv)
{
	ASSERT(pv);
	if (pv)
		return HeapSize(m_hHeap, 0, pv);
	else
		return 0;
}

char* CTTHeap::strdup(const char* psz)
{
	ASSERT_MSG(psz, "null pointer!");

	if (!psz || !*psz)
		psz = "";

	size_t cb = kstrbyte(psz);
	char* pszDst = (char*) malloc(cb);
	memcpy(pszDst, psz, cb);
	return pszDst;
}

wchar_t* CTTHeap::strdup(const wchar_t* pwsz)
{
	ASSERT_MSG(pwsz, "null pointer!");

	if (!pwsz || !*pwsz)
		pwsz = L"";

	size_t cb = kstrbyte(pwsz);
	wchar_t* pwszDst = (wchar_t*) malloc(cb);
	memcpy(pwszDst, pwsz, cb);
	return pwszDst;
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
