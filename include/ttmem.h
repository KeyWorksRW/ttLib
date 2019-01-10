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
	ttMem(void) { pb = nullptr; }
	ttMem(size_t size) { pb = (uint8_t*) tt::malloc(size); }
	~ttMem(void) {
		if (pb)
			tt::free(pb);
		pb = nullptr;
	}
	void resize(size_t cb) {
		if (!pb)
			pb = (uint8_t*) tt::malloc(cb);
		else {
			pb = (uint8_t*) tt::realloc(pb, cb);
		}
	}

	operator void*() { return (void*) pb; };
	operator const char*() { return (const char*) pb; };
	operator char*()  { return (char*) pb; };
	operator uint8_t*() { return pb; };

	uint8_t* pb;
};

template <typename T> class ttTMem	// Header-only class
{
public:
	ttTMem() { p = NULL; }
	ttTMem(size_t size) { p = (T) tt::malloc(size); }
	~ttTMem() {
		if (p)
			tt::free(p);
	}

	void resize(size_t cb) {
		if (!p)
			p = (T) tt::malloc(cb);
		else
			p = (T) tt::realloc(p, cb);
	}
	size_t size() { return tt::size(p); }

	operator T()	{ ttASSERT(p); return p; };
	T operator->() { ttASSERT(p); return p; };

	T p;
};

#endif	// __TTLIB_TTMEM_H__
