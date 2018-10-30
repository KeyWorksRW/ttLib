/////////////////////////////////////////////////////////////////////////////
// Name:		asserts.h
// Purpose:		Assert functionality
// Author:		Ralph Walden (randalphwa)
// Copyright:   Copyright (c) 2000-2018 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Under a _DEBUG build, these ASSERTS will display a message box giving you the
// option to ignore the assert, break into a debugger, or exit the program.

#pragma once

#ifndef __TTLIB_ASSERTS_H__
#define __TTLIB_ASSERTS_H__

#ifdef _DEBUG

	#define ASSERT(exp) (void)((!!(exp)) || AssertionMsg(#exp, __FILE__, __func__, __LINE__))
	#define ASSERT_MSG(exp, pszMsg) { if (!(exp)) AssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
	#define ASSERT_COMMENT(exp, pszComment) { if (!(exp)) AssertionMsg(pszComment, __FILE__, __func__, __LINE__); }
	#define VERIFY(exp) (void)((!!(exp)) || AssertionMsg(#exp, __FILE__, __func__, __LINE__))
	#define FAIL(pszMsg) AssertionMsg(pszMsg, __FILE__, __func__, __LINE__)

#ifdef _WINDOWS_
	#define ASSERT_HRESULT(hr, pszMsg) { if (FAILED(hr)) AssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
	#define ReportLastError() { doReportLastError(__FILE__, __func__, __LINE__); }
	void doReportLastError(const char* pszFile, const char* pszFunc, int line);
#endif

	void _cdecl CATCH_HANDLER(const char* pszFormat, ...);
	bool  AssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);

#else	// not _DEBUG

	#define ASSERT(exp)
	#define ASSERT_MSG(exp, pszMsg)
	#define ASSERT_COMMENT(exp, pszMsg)
	#define VERIFY(exp) ((void)(exp))
	#define FAIL(pszMsg)


#ifdef _WINDOWS_
	#define ASSERT_HRESULT(hr, pszMsg)
	#define ReportLastError()
#endif

	#define CATCH_HANDLER __noop
	#define AssertionMsg  __noop

#endif	// _DEBUG

#endif	// __TTLIB_ASSERTS_H__
