/////////////////////////////////////////////////////////////////////////////
// Name:		ttCArray
// Purpose:		Simple templated array
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2010-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTARRAY_H__
#define __TTLIB_TTARRAY_H__

#include "ttheap.h" 	// ttCHeap
#include "ttdebug.h"	// ttASSERT macros

// A simple header-only array of any type

template <typename T> class ttCArray
{
public:
	ttCArray() {
		m_cAllocated = m_cItems = 0;
		m_aData = NULL;
	}
	~ttCArray() {
		if (m_aData)
			ttFree(m_aData);
	}

	void Add(const T t) {
		if (m_cItems >= m_cAllocated) {
			m_cAllocated += 8;	// allocate room for 8 items at a time
			m_aData = (T*) (m_aData ? ttReAlloc(m_aData, m_cAllocated * sizeof(T)) : ttMalloc(m_cAllocated * sizeof(T)));
		}
		m_aData[m_cItems++] = t;
	}

	size_t Add() {	// use this to add an emptry member which you can fill in using the returned array index
		if (m_cItems >= m_cAllocated) {
			m_cAllocated += 8;	// allocate room for 8 items at a time
			m_aData = (T*) (m_aData ? ttReAlloc(m_aData, m_cAllocated * sizeof(T)) : ttMalloc(m_cAllocated * sizeof(T)));
		}
		return m_cItems++;
	}

	bool Find(const T t, size_t* ppos = nullptr) const {
		for (size_t pos = 0; pos < m_cItems; pos++) {
			if (m_aData[pos] == t) {
				if (ppos) {
					*ppos = pos;
				}
				return true;
			}
		}
		return false;
	}

	size_t Find(const T t) const {	// returns -1 if not found
		for (size_t pos = 0; pos < m_cItems; pos++) {
			if (m_aData[pos] == t) {
				return pos;
			}
		}
		return (size_t) -1;
	}

	size_t GetCount() const { return m_cItems; }
	bool   InRange(size_t pos) const { return (pos < m_cItems && m_cItems > 0); }

	void Reset() {	// caller's responsibility to delete any allocated members first!
		if (m_aData) {
			ttFree(m_aData);
			m_aData = nullptr;
		}
		m_cAllocated = m_cItems = 0;
	}

	void operator+=(T t) { Add(t); }
	T& operator[](size_t pos) const {
		ttASSERT(InRange(pos));
		if (!InRange(pos))
			throw;
		return m_aData[pos];
	}

protected:
	size_t  m_cItems;
	size_t  m_cAllocated;
	T*      m_aData;
};

#endif	// __TTLIB_TTARRAY_H__
