/////////////////////////////////////////////////////////////////////////////
// Name:		CTTArray
// Purpose:		Simple templated array
// Author:		Ralph Walden (randalphwa)
// Copyright:   Copyright (c) 2010-2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTARRAY_H__
#define __TTLIB_TTARRAY_H__

// A simple array of any type

template <typename T> class CTTArray
{
public:
	CTTArray() {
		m_cAllocated = m_cItems = 0;
		m_aData = NULL;
	}
	~CTTArray() {
		if (m_aData)
			tt::free(m_aData);
	}

	void Add(const T t) {
		if (m_cItems >= m_cAllocated) {
			m_cAllocated += 32;
			m_aData = (T*) (m_aData ? tt::realloc(m_aData, m_cAllocated * sizeof(T)) : tt::malloc(m_cAllocated * sizeof(T)));
		}
		m_aData[m_cItems++] = t;
	}

	bool Find(const T t, size_t* ppos = NULL) const {
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

	size_t GetCount() const { return m_cItems; }

	void operator+=(T t) { Add(t); }
	T operator[](size_t pos) const {
		ASSERT(pos < m_cItems);
		if (pos >= m_cItems)
			return NULL;
		return m_aData[pos];
	}

protected:
	T*      m_aData;
	size_t  m_cAllocated;
	size_t  m_cItems;
};

#endif	// __TTLIB_TTARRAY_H__
