/////////////////////////////////////////////////////////////////////////////
// Name:		ttCHeap
// Purpose:		Class for utilizing Windows heap manager
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTHEAP_H__
#define __TTLIB_TTHEAP_H__

// CAUTION! If you include this file and your code specifies "using namespace tt;" then ALL of your standard memory
// functions (malloc, size, free, etc.) will be replaced. If you don't want that to happen, then do NOT specify
// "using namespace tt;" and instead use the "tt::" namespace prefix for all ttLib functions including the ones here.

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

/*
	The Windows heap manager is a bit faster to use then the C runtime. Replacing the standard memory
	allocation functions provides a performance boost and eliminates the need to check for a null pointer on
	return. If there is insufficient memory, the application will be terminated (see OOM()).

	ttCHeap can also be used to create a sub-heap. Any individual memory allocations on the sub-heap do not
	need to be freed before the destructor as the entire sub-heap is destroyed at once. A class that wants to utilize
	this functionality should inherit from ttCHeap, and provide a serialization flag in it's constructor:

		classs MyClass :  public ttCHeap
		{
			MyClass() : ttCHeap(true) { } // true to make MyClass thread-safe

	Now all of the malloc/realloc/recalloc routines below will be allocated on the sub-heap, and do
	not need to be specifically freed in the destructor.

	Constructing ttCHeap using another heap takes advantage of the sub-heap above by elimintating the need to
	individually free every memory allocation in the destructor.
*/

class ttCHeap
{
public:
	ttCHeap();
	ttCHeap(bool bSerialize);	// Creates a sub-heap. Use true for thread safe, false for speed (but not thread safe)
	ttCHeap(HANDLE hHeap);		// Pass in a heap handle or another ttCHeap class (which will call the HANDLE() operator)

	~ttCHeap();

	// Class functions

	// We use the "tt" prefix to make certain there is no confusion in a derived class that the memory routines are from
	// this class rather than the standard memory functions. Outside of the class, using the "tt::" prefix avoids
	// potential confusion.

	void* ttMalloc(size_t cb);	// under _DEBUG, will fill with 0xCD
	void* ttCalloc(size_t cb);
	void* ttRealloc(void* pv, size_t cb);
	void* ttRecalloc(void* pv, size_t cb);

	void  ttFree(void* pv) { if (pv) HeapFree(m_hHeap, 0, pv); }

	char*	 ttStrdup(const char* psz);
	wchar_t* ttStrdup(const wchar_t* pwsz);

	char*	 ttStrdup(const char* psz, char** ppszDst);		// allocates/reallocates *ppszDst

	size_t	ttSize(const void* pv) { return pv ? HeapSize(m_hHeap, 0, pv) : 0; }
	bool	ttValidate(const void* pv) { return HeapValidate(m_hHeap, 0, pv); }

	operator HANDLE() const { return m_hHeap; }

protected:
	// Class members

	HANDLE	m_hHeap;
	bool	m_bCreated;
}; // end ttCHeap

// The tt namespace is used in other ttLib header files as well, so this is not a complete list. It is STRONGLY
// recommended that you reference all functions in ttLib with "tt::". Do NOT declare "using namespace tt" unless you want
// other function calls in your code to be replaced. See https://github.com/KeyWorksRW/ttLib/issues/12 for discussion of
// the issue.

namespace tt {
	extern ttCHeap MainHeap;	// this uses the process heap rather then a sub-heap

	inline void*	calloc(size_t cb) { return tt::MainHeap.ttCalloc(cb); }
	inline void*	calloc(size_t num, size_t cb) { return tt::MainHeap.ttCalloc(num * cb); }	// for compatability with C++ standard library
	inline void		free(void *pv) { tt::MainHeap.ttFree(pv); }
	inline void*	malloc(size_t cb) { return tt::MainHeap.ttMalloc(cb); }
	inline void*	realloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRealloc(pv, cbNew); }
	inline void*	recalloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRecalloc(pv, cbNew); }
	inline char*	strdup(const char* psz) { return tt::MainHeap.ttStrdup(psz); }
	inline wchar_t*	strdup(const wchar_t* pwsz) { return tt::MainHeap.ttStrdup(pwsz); }
	inline char*	strdup(const char* psz, char** ppszDst) { return tt::MainHeap.ttStrdup(psz, ppszDst); }
	inline size_t	size(const void* pv) { return tt::MainHeap.ttSize(pv); }
	inline bool		validate(const void* pv) { return tt::MainHeap.ttValidate(pv); }
}	// end namespace tt

#endif	// __TTLIB_TTHEAP_H__
