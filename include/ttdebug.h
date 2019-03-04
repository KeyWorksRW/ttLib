/////////////////////////////////////////////////////////////////////////////
// Name:		ttdebug.h
// Purpose:		ttASSERT macros
// Author:		Ralph Walden
// Copyright:	Copyright (c) 2000-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// Under a _DEBUG build, these ASSERTS will display a message box giving you the
// option to ignore the assert, break into a debugger, or exit the program.

#pragma once

#ifndef __TTLIB_ASSERTS_H__
#define __TTLIB_ASSERTS_H__

// Note that all functions listed here are available in release builds. Only the macros are removed in release builds

namespace tt {
	bool AssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line);
	bool AssertionMsg(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction, int line);
	void doReportLastError(const char* pszFile, const char* pszFunc, int line);
	void SetAsserts(bool bDisable);	// enables disables all assertion messages

	__declspec(noreturn) void OOM(void);

	void cdecl ttTrace(const char* pszFormat, ...);	// formats a string and displays it in a ttTrace window (if ttTrace is running)

	extern const WPARAM WMP_TRACE_GENERAL; // WM_USER + 0x1f3;	// general message to send to ttTrace
	extern const WPARAM WMP_TRACE_MSG;     // WM_USER + 0x1f5;	// trace message to send to ttTrace
	extern const WPARAM WMP_CLEAR_TRACE;   // WM_USER + 0x1f9;	// clears the ttTrace window

	extern const char* txtTraceClass;		// class name of window to send trace messages to
	extern const char* txtTraceShareName;	// name of shared memory to write to

#ifdef _WINDOWS_
	DWORD CheckItemID(HWND hwnd, int id, const char* pszID, const char* pszFile, const char* pszFunc, int line);
#endif
}

#ifdef _DEBUG
	#define ttASSERT(exp) (void)((!!(exp)) || tt::AssertionMsg(#exp, __FILE__, __func__, __LINE__))
	#define ttASSERT_MSG(exp, pszMsg) { if (!(exp)) tt::AssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
	#define ttFAIL(pszMsg) tt::AssertionMsg(pszMsg, __FILE__, __func__, __LINE__)

	// checks for both ptr == NULL and *ptr == NULL
	#define ttASSERT_NONEMPTY(ptr) { if (!ptr || !*ptr) tt::AssertionMsg(ptr, __FILE__, __func__, __LINE__); }	// AssertionMsg figures out if it's nullptr or just empty

	#define ttDISABLE_ASSERTS tt::SetAsserts(true)
	#define ttENABLE_ASSERTS  tt::SetAsserts(false)

#ifdef _WINDOWS_
	#define ttASSERT_HRESULT(hr, pszMsg) { if (FAILED(hr)) tt::AssertionMsg(pszMsg, __FILE__, __func__, __LINE__); }
	#define ttReportLastError() { tt::doReportLastError(__FILE__, __func__, __LINE__); }
#endif

#else	// not _DEBUG

	#define ttASSERT(exp)
	#define ttASSERT_MSG(exp, pszMsg)
	#define ttFAIL(pszMsg)
	#define ttASSERT_NONEMPTY(ptr)

	#define ttDISABLE_ASSERTS
	#define ttENABLE_ASSERTS

#ifdef _WINDOWS_
	#define ttASSERT_HRESULT(hr, pszMsg)
	#define ttReportLastError()
#endif

#endif	// _DEBUG

#endif	// __TTLIB_ASSERTS_H__
