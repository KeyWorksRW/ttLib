/////////////////////////////////////////////////////////////////////////////
// Name:		ttmem.h
// Purpose:		Header-only memory classes
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_TTMEM_H__
#define __TTLIB_TTMEM_H__

#include "ttdebug.h"	// Various debugging functionality
#include "ttheap.h" 	// ttHeap

/*
  ttMem and ttTMem allow you to allocate memory that will automatically be freed when the class gets destroyed.

  if (some condition) {
	ttMem szBuf(256);
	strcpy(szBuf, "text");
	strcat(szBuf, "more text");
	cout << (char*) szBuf;
  } // szBuf is freed because it went out of scope
*/

class ttMem	// Header-only class
{
public:
	ttMem(void) { m_pb = nullptr; }
	ttMem(size_t size) { m_pb = (uint8_t*) tt::malloc(size); }
	~ttMem(void) {
		if (m_pb)
			tt::free(m_pb);
		m_pb = nullptr;
	}
	void resize(size_t cb) {
		if (!m_pb)
			m_pb = (uint8_t*) tt::malloc(cb);
		else {
			m_pb = (uint8_t*) tt::realloc(m_pb, cb);
		}
	}
	size_t size() { return tt::size(m_pb); }

	operator void*() { return (void*) m_pb; };
	operator const char*() { return (const char*) m_pb; };
	operator char*()  { return (char*) m_pb; };
	operator uint8_t*() { return m_pb; };

	uint8_t* m_pb;
};

template <typename T> class ttTMem	// Header-only class
{
public:
	ttTMem() { m_p = NULL; }
	ttTMem(size_t size) { m_p = (T) tt::malloc(size); }
	~ttTMem() {
		if (m_p)
			tt::free(m_p);
	}

	void resize(size_t cb) {
		if (!m_p)
			m_p = (T) tt::malloc(cb);
		else
			m_p = (T) tt::realloc(m_p, cb);
	}
	size_t size() { return tt::size(m_p); }

	operator T()	{ ttASSERT(m_p); return m_p; };
	T operator->() { ttASSERT(m_p); return m_p; };

	T m_p;
};

#endif	// __TTLIB_TTMEM_H__
