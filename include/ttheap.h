/////////////////////////////////////////////////////////////////////////////
// Name:		CTTHeap
// Purpose:		Class for utilizing Windows heap manager
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTHEAP_H__
#define __TTLIB_TTHEAP_H__

#ifdef _WINDOWS_

/*
	The Windows heap manager is a bit faster to use then the C runtime. Replacing the standard memory
	allocation functions provides a performance boost and eliminates the need to check for a null pointer on
	return. If there is insufficient memory, the application will be terminated (see OOM()).

	CTTHeap can also be used to create a sub-heap. Any individual memory allocations on the sub-heap do not
	need to be freed before the destructor as the entire sub-heap is destroyed at once. A class that wants to utilize
	this functionality should inherit from CTTHeap, and provide a serialization flag in it's constructor:

		classs MyClass :  public CTTHeap
		{
			MyClass() : CTTHeap(true) { } // true to make MyClass thread-safe

	Now all of the malloc/realloc/recalloc routines below will be allocated on the sub-heap, and do
	not need to be specifically freed in the destructor.

	Constructing CTTHeap using another heap takes advantage of the sub-heap above by elimintating the need to
	individually free every memory allocation in the destructor.
*/

class CTTHeap
{
public:
	CTTHeap();
	CTTHeap(bool bSerialize);	// Creates a sub-heap. Use true for thread safe, false for speed (but not thread safe)
	CTTHeap(HANDLE hHeap);		// Pass in a heap handle or another CTTHeap class (which will call the HANDLE() operator)

	~CTTHeap();

	// Class functions

	void* ttMalloc(size_t cb);	// under _DEBUG, will fill with 0xCD
	void* ttCalloc(size_t cb);
	void* ttRealloc(void* pv, size_t cb);
	void* ttRecalloc(void* pv, size_t cb);

	void  ttFree(void* pv);

	char*	 ttStrdup(const char* psz);
	wchar_t* ttStrdup(const wchar_t* pwsz);

	size_t	ttSize(const void* pv);
	BOOL	ttValidate(const void* pv) { return HeapValidate(m_hHeap, 0, pv); }

	operator HANDLE() const { return m_hHeap; }

	[[deprecated("Create heap with CTTHeap(bSerialize) instead")]]
		bool CreateHeap(bool bSerialize = true);	// This will destroy any non-process heap that was previously created
	[[deprecated("Create heap with CTTHeap(hHeap) instead")]]
		void Attach(HANDLE hHeap);					// attatch this heap to another heap, or a class that derives from CTTHeap

protected:

	[[deprecated("use ttMalloc() instead")]]
		void* malloc(size_t cb) { return ttMalloc(cb); };
	[[deprecated("use ttCalloc() instead")]]
		void* calloc(size_t cb) { return ttCalloc(cb); };
	[[deprecated("use ttRealloc() instead")]]
		void* realloc(void* pv, size_t cb) { return ttRealloc(pv, cb); }
	[[deprecated("use ttRecalloc() instead")]]
		void* recalloc(void* pv, size_t cb) { return ttRecalloc(pv, cb); }

	[[deprecated("use ttFree() instead")]]
		void   free(void* pv) { ttFree(pv); }

	[[deprecated("use ttStrdup() instead")]]
		char*	 strdup(const char* psz) { return ttStrdup(psz); }
	[[deprecated("use ttStrdup() instead")]]
		wchar_t* strdup(const wchar_t* pwsz)  { return ttStrdup(pwsz); }

	[[deprecated("use ttSize() instead")]]
		size_t size(const void* pv) { return ttSize(pv); }

	// Class members

	HANDLE	m_hHeap;
	bool	m_bCreated;
}; // end CTTHeap

namespace tt {
	extern CTTHeap MainHeap;	// this uses the process heap rather then a sub-heap

	inline void*	calloc(size_t cb) { return tt::MainHeap.ttCalloc(cb); }
	inline void		free(void *pv) { tt::MainHeap.ttFree(pv); }
	inline void*	malloc(size_t cb) { return tt::MainHeap.ttMalloc(cb); }
	inline void*	realloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRealloc(pv, cbNew); }
	inline void*	recalloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRecalloc(pv, cbNew); }
	inline char*	strdup(const char* psz) { return tt::MainHeap.ttStrdup(psz); }
	inline wchar_t*	strdup(const wchar_t* pwsz) { return tt::MainHeap.ttStrdup(pwsz); }
	inline size_t	size(const void* pv) { return tt::MainHeap.ttSize(pv); }
	inline BOOL		validate(const void* pv) { return tt::MainHeap.ttValidate(pv); }
}	// end namespace tt

// The following are obsolete, but not yet marked as deprecated (they will be!). Applications should use the namespace
// versions above instead. I.e., instead of kmalloc(), use tt:malloc()

#ifndef TTHEAP_DEPRECATE	// define this to track down the calls that need to be changed
	inline void*	kcalloc(size_t cb) { return tt::MainHeap.ttCalloc(cb); }
	inline void		kfree(void *pv) { tt::MainHeap.ttFree(pv); }
	inline void*	kmalloc(size_t cb) { return tt::MainHeap.ttMalloc(cb); }
	inline void*	krealloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRealloc(pv, cbNew); }
	inline void*	krecalloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRecalloc(pv, cbNew); }
	inline char*	kstrdup(const char* psz) { return tt::MainHeap.ttStrdup(psz); }
	inline wchar_t*	kstrdup(const wchar_t* pwsz) { return tt::MainHeap.ttStrdup(pwsz); }
	inline size_t	ksize(const void* pv) { return tt::MainHeap.ttSize(pv); }
	inline BOOL		kvalidate(const void* pv) { return tt::MainHeap.ttValidate(pv); }
#else	// not TTHEAP_DEPRECATE
	[[deprecated("use tt::func() instead")]]
	inline void*	kcalloc(size_t cb) { return tt::MainHeap.ttCalloc(cb); }
	[[deprecated("use tt::func() instead")]]
	inline void		kfree(void *pv) { tt::MainHeap.ttFree(pv); }
	[[deprecated("use tt::func() instead")]]
	inline void*	kmalloc(size_t cb) { return tt::MainHeap.ttMalloc(cb); }
	[[deprecated("use tt::func() instead")]]
	inline void*	krealloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRealloc(pv, cbNew); }
	[[deprecated("use tt::func() instead")]]
	inline void*	krecalloc(void* pv, size_t cbNew) { return tt::MainHeap.ttRecalloc(pv, cbNew); }
	[[deprecated("use tt::func() instead")]]
	inline char*	kstrdup(const char* psz) { return tt::MainHeap.ttStrdup(psz); }
	[[deprecated("use tt::func() instead")]]
	inline wchar_t*	kstrdup(const wchar_t* pwsz) { return tt::MainHeap.ttStrdup(pwsz); }
	[[deprecated("use tt::func() instead")]]
	inline size_t	ksize(const void* pv) { return tt::MainHeap.ttSize(pv); }
	[[deprecated("use tt::func() instead")]]
	inline BOOL		kvalidate(const void* pv) { return tt::MainHeap.ttValidate(pv); }
#endif	// TTHEAP_DEPRECATE

/////////////////////////////////////////////// non-Windows code ///////////////////////////////////////////////////

#else	// not _WINDOWS_	(See ISSUE #5: need to implement a POSIX version for portability)

// Because CTTHep is inefficient when not built on _WINDOWS_, use normal CRT allocation routines instead of _MainHeap

inline void*	kcalloc(size_t cb) { return calloc(1, cb); }
inline void		kfree(const void *pv) { free(pv); }
inline void*	kmalloc(size_t cb) { return malloc(cb); }
inline void*	krealloc(void* pv, size_t cbNew) { return (void*) (pv ? realloc(pv, cbNew) : malloc(cbNew)); }
inline void*	krecalloc(void* pv, size_t cbNew) { return (void*) (pv ? realloc(pv, cbNew) : calloc(1, cbNew)); }
inline char*	kstrdup(const char* psz) { return strdup(psz); }
inline wchar_t*	kstrdup(const wchar_t* pwsz) { return _MainHeap.strdup(pwsz); }

// There isn't a way to get the size of an allocated buffer using the standard malloc() routines, so ksize() can't be implemented

// __inline size_t		ksize(const void* pv) { return _MainHeap.size(pv); }

// To mimic the _WINDOWS_ functionality that cleans up all memory allocations in the destructor, we'll need to track
// every memory allocation in order to free each one on exit.

// BUGBUG!!! The following is a placeholder -- there is currently no POSIX version (see Issue #5).

class CTTHeap
{
public:
	CTTHeap();
	CTTHeap(bool bSerialize);	// For compatibility with _WINDOWS_ version -- it's identical to CTTHeap()
	CTTHeap(HANDLE hHeap);
	~CTTHeap();

	void* ttMalloc(size_t cb);	// under _DEBUG, will fill with 0xCD
	void* ttCalloc(size_t cb);
	void* ttRealloc(void* pv, size_t cb);
	void* ttRecalloc(void* pv, size_t cb);

	void  ttFree(void* pv);

	char*	 ttStrdup(const char* psz);
	wchar_t* ttStrdup(const wchar_t* pwsz);

	operator HANDLE() const { return NULL; }	// need to return something that points to a sub-heap
protected:
	void*	m_aPtrs;
	size_t	m_cAllocated;	// memory allocated to hold m_aPtrs
	size_t	m_cItems;		// current number of ptrs in m_aPtrs
};

#endif	// _WINDOWS_
#endif	// __TTLIB_TTHEAP_H__
