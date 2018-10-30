/////////////////////////////////////////////////////////////////////////////
// Name:		asserts.cpp
// Purpose:		Various assertion messages
// Author:		Ralph Walden (randalphwa)
// Copyright:	Copyright (c) 1998-2018 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include <stdio.h>

#include "../include/asserts.h"
#include "../include/cstr.h"		// CStr
#include "../include/critsection.h"	// CCritSection

#ifndef _INC_STDLIB
	__declspec(noreturn) void __cdecl exit(int _Code);
#endif

// OOM() is the one function here which also exists in non-DEBUG builds. Everything else is DEBUG-only.
// Under both _DEBUG and _NDEBUG builds, it always exits the program

__declspec(noreturn) void OOM(void)
{
#ifdef _DEBUG

#ifdef _WINDOWS_
	int answer = MessageBoxA(GetActiveWindow(), "Out of Memory!!!", "Do you want to call DebugBreak()?", MB_YESNO | MB_ICONERROR);

	if (answer == IDYES)
		DebugBreak();
	// Don't use GetCurrentWindowHandle() since that might only return an active window
	if (_hwndParent && IsWindow(_hwndParent))
		SendMessage(_hwndParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down
#elif _WX_WX_H_
	wxFAIL_MSG("Out of Memory!!!");
#endif	// __WINDOWS_ and _WX_WX_H_

#endif	// _DEBUG

	exit(-1);
}

#ifdef _DEBUG

#ifdef _WINDOWS_

extern HWND _hwndParent;		// If set, it will receive a WM_CLOSE message if user chooses ABORT before process exits

#endif

static CCritSection crtAssert;

// Displays a message box displaying the ASSERT with an option to ignore, break into a debugger, or exit the program

bool AssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line)
{
	crtAssert.Lock();

	char szBuf[2048];
	sprintf_s(szBuf, "%s\r\n\r\n%s (%s): line %u", pszMsg, pszFile, pszFunction, line);

#ifdef _WINDOWS_

	// wxWidgets does not have an equivalent of MB_ABORTRETRYIGNORE, nor will it work properly in a CONSOLE
	// application, so we just call directly into the Windws API.

	int answer = MessageBoxA(GetActiveWindow(), szBuf, "Retry to call DebugBreak()", MB_ABORTRETRYIGNORE);

	if (answer == IDRETRY) {
		DebugBreak();
		crtAssert.Unlock();
		return false;
	}
	else if (answer == IDIGNORE) {
		crtAssert.Unlock();
		return false;
	}

	if (_hwndParent && IsWindow(_hwndParent))
		SendMessage(_hwndParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down

	crtAssert.Unlock();
	ExitProcess((UINT) -1);

#else	// not _WINDOWS_

#ifdef _WX_WX_H_
	wxFAIL_MSG(pszMsg, pszFile, line, pszFunction);
#else	// not _WX_WX_H_
	asm volatile ("int $3");
#endif // _WX_WX_H_

	crtAssert.Unlock();
	return false;
#endif	// _WINDOWS_

}

// Displays a message box displaying the catch message with an option to ignore, break into a debugger, or exit the program. Example:
//
// try {
//	   ...
// }
// catch (...) {
//	   CATCH_HANDLER("%s (%d) : Exception in %s()", __FILE__, __LINE__, __func__);
// }

void _cdecl CATCH_HANDLER(const char* pszFormat, ...)
{
	ASSERT(pszFormat);
	if (!pszFormat)
		return;

	va_list argList;
	va_start(argList, pszFormat);
	CStr cszMsg;
	cszMsg.vprintf(pszFormat, argList);
	va_end(argList);

#ifdef _WINDOWS_

	int answer = MessageBoxA(GetActiveWindow(), cszMsg, "Retry to call DebugBreak()", MB_ABORTRETRYIGNORE);

	if (answer == IDRETRY) {
		DebugBreak();
		return;
	}
	else if (answer != IDIGNORE) {
		if (_hwndParent && IsWindow(_hwndParent))
			SendMessage(_hwndParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down
		ExitProcess((UINT) -1);
	}

#else	// not _WINDOWS_

#ifdef _WX_WX_H_
	wxFAIL_MSG(cszMsg);
#else	// not _WX_WX_H_
	asm volatile ("int $3");
#endif	// _WX_WX_H_

#endif	// _WINDOWS_
}


#ifdef _WINDOWS_

void doReportLastError(const char* pszFile, const char* pszFunc, int line)
{
	char* pszMsg;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &pszMsg, 0, NULL);

	AssertionMsg(pszMsg, pszFile, pszFunc, line);

	LocalFree((HLOCAL) pszMsg);
}

#endif	// _WINDOWS_

#endif	// _DEBUG
