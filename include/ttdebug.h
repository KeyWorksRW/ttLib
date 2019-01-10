/////////////////////////////////////////////////////////////////////////////
// Name:		ttdebug.h
// Purpose:		Various debugging functionality
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 2000-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Under a _DEBUG build, these ASSERTS will display a message box giving you the
// option to ignore the assert, break into a debugger, or exit the program.

#pragma once

#ifndef __TTLIB_ASSERTS_H__
#define __TTLIB_ASSERTS_H__

namespace tt {
	bool AssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);
	void doReportLastError(const char* pszFile, const char* pszFunc, int line);
	void _cdecl CATCH_HANDLER(const char* pszFormat, ...);
	__declspec(noreturn) void OOM(void);
}

#ifdef _DEBUG
	#define ttASSERT(exp) (void)((!!(exp)) || tt::AssertionMsg(#exp, __FILE__, __func__, __LINE__))
	#define ttASSERT_MSG(exp, pszMsg) { if (!(exp)) tt::AssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
	#define ttFAIL(pszMsg) tt::AssertionMsg(pszMsg, __FILE__, __func__, __LINE__)

#ifdef _WINDOWS_
	#define ttASSERT_HRESULT(hr, pszMsg) { if (FAILED(hr)) tt::AssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
	#define ttReportLastError() { tt::doReportLastError(__FILE__, __func__, __LINE__); }
#endif

#else	// not _DEBUG

	#define ttASSERT(exp)
	#define ttASSERT_MSG(exp, pszMsg)
	#define ttFAIL(pszMsg)

#ifdef _WINDOWS_
	#define ttASSERT_HRESULT(hr, pszMsg)
	#define ttReportLastError()
#endif

	#define tt::CATCH_HANDLER __noop
	#define tt::AssertionMsg  __noop

#endif	// _DEBUG

#endif	// __TTLIB_ASSERTS_H__
