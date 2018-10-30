/////////////////////////////////////////////////////////////////////////////
// Name:		CTTHeap
// Purpose:		Class for utilizing Windows heap manager
// Author:		Ralph Walden
// Copyright:   Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
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
	need be freed before the destructor as the entire sub-heap is destroyed at once. A class that wants to utilize
	this functionality should inherit from CTTHeap, and provide a serialization flag in it's constructor:

		classs MyClass :  public CTTHeap
		{
			MyClass() : CTTHeap(true) { } // true to make MyClass thread-safe, otherwise use false

	Now all of the kmalloc/krealloc/krecalloc routines below will be allocated on the sub-heap, and do
	not need to be specifically freed in the destructor.

	Attatching CTTHeap to another heap takes advantage of the sub-heap above by elimintating the need to
	individually free every memory allocation in the destructor.
*/

class CTTHeap
{
public:
	CTTHeap();
	CTTHeap(bool bSerialize);	// Creates a sub-heap. Use true for thread safe, false for speed (but not thread safe)
	~CTTHeap();

	// Class functions

	bool CreateHeap(bool bSerialize = true);	// This will destroy any non-process heap that was previously created
	void Attach(HANDLE hHeap);					// attatch this heap to another heap, or a class that derives from CTTHeap

	void* malloc(size_t cb);	// under _DEBUG, will fill with 0xCC
	void* calloc(size_t cb);
	void* realloc(void* pv, size_t cb);
	void* recalloc(void* pv, size_t cb);

	void   free(void* pv);
	size_t size(const void* pv);

	char*	 strdup(const char* psz);
	wchar_t* strdup(const wchar_t* pwsz);

	operator HANDLE() const { return m_hHeap; }
protected:
	// Class members

	HANDLE	m_hHeap;
	bool	m_bCreated;
#ifdef _DEBUG
	bool	m_SerialHeap;
#endif
}; // end CTTHeap

extern CTTHeap _MainHeap;	// this uses the process heap rather then a sub-heap

// Note: the debug version of kmalloc will fill allocated memory with 0xCC.

inline void*	kcalloc(size_t cb) { return _MainHeap.calloc(cb); }
inline void		kfree(void *pv) { _MainHeap.free(pv); }
inline void*	kmalloc(size_t cb) { return _MainHeap.malloc(cb); }
inline void*	krealloc(void* pv, size_t cbNew) { return _MainHeap.realloc(pv, cbNew); }
inline void*	krecalloc(void* pv, size_t cbNew) { return _MainHeap.recalloc(pv, cbNew); }
inline size_t	ksize(const void* pv) { return _MainHeap.size(pv); }
inline char*	kstrdup(const char* psz) { return _MainHeap.strdup(psz); }
inline wchar_t*	kstrdup(const wchar_t* pwsz) { return _MainHeap.strdup(pwsz); }

/////////////////////////////////////////////// non-Windows code ///////////////////////////////////////////////////

#else	// not _WINDOWS_

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

class CTTHeap
{
public:
	CTTHeap();
	CTTHeap(bool bSerialize);	// For compatibility with _WINDOWS_ version -- it's identical to CTTHeap()
	~CTTHeap();

	bool  CreateHeap(bool bSerialize = true);	// bSerialize will be ignored -- it's there for compatibility of _WINDOWS_ version
	void  Attach(void* hHeap) { }	// No equivalent without _WINDOWS_

	void* malloc(size_t cb);
	void* calloc(size_t cb);
	void* malloc(size_t vb);
	void* realloc(void* pv, size_t vb);
	void  free(void* pv);

	char*	 strdup(const char* psz);
	wchar_t* strdup(const wchar_t* pwsz);

	operator void*() const { return nullptr; }	// there is no equivalent without _WINDOWS_
protected:
	void*	m_aPtrs;
	size_t	m_cAllocated;	// memory allocated to hold m_aPtrs
	size_t	m_cItems;		// current number of ptrs in m_aPtrs
};

#endif	// _WINDOWS_
#endif	// __TTLIB_TTHEAP_H__