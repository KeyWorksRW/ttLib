/////////////////////////////////////////////////////////////////////////////
// Name:		ttdebug.cpp
// Purpose:		Various debugging functionality
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"	// precompiled header

#include <stdio.h>

#include "../include/ttdebug.h" 		// ttASSERT macros
#include "../include/ttstring.h"		// ttCStr
#include "../include/ttstr.h"			// ttStr
#include "../include/ttcritsection.h"	// CCritSection

#ifndef _INC_STDLIB
	__declspec(noreturn) void __cdecl exit(int _Code);
#endif

// OOM() is the one function here which also exists in non-DEBUG builds. Everything else is DEBUG-only.
// Under both _DEBUG and _NDEBUG builds, it always exits the program

__declspec(noreturn) void tt::OOM(void)
{
#ifdef _DEBUG

#ifdef _WINDOWS_
	int answer = MessageBoxA(GetActiveWindow(), "Out of Memory!!!", "Do you want to call DebugBreak()?", MB_YESNO | MB_ICONERROR);

	if (answer == IDYES)
		DebugBreak();
	// Don't use GetCurrentWindowHandle() since that might only return an active window
	if (tt::hwndParent && IsWindow(tt::hwndParent))
		SendMessage(tt::hwndParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down
#elif _WX_WX_H_
	wxFAIL_MSG("Out of Memory!!!");
#endif	// __WINDOWS_ and _WX_WX_H_

#endif	// _DEBUG

	exit(-1);
}

#ifdef _DEBUG

namespace ttdbg {
	ttCCritSection crtAssert;
	bool bNoAssert = false;		// Setting this to true will cause AssertionMsg to return without doing anything
}

using namespace ttdbg;

// Displays a message box displaying the ASSERT with an option to ignore, break into a debugger, or exit the program

bool tt::AssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line)
{
	if (ttdbg::bNoAssert)
		return false;

	crtAssert.Lock();

	// We special case a null or empty pszMsg -- ttASSERT_NONEMPTY(ptr) takes advantage of this

	if (!pszMsg)
		pszMsg = "NULL pointer!";
	else if (!*pszMsg)
		pszMsg = "Empty string!";

	char szBuf[2048];
	sprintf_s(szBuf, sizeof(szBuf), "%s\r\n\r\n%s (%s): line %u", pszMsg, pszFile, pszFunction, line);

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

	if (tt::hwndParent && IsWindow(tt::hwndParent))
		SendMessage(tt::hwndParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down

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

bool tt::AssertionMsg(const wchar_t* pwszMsg, const char* pszFile, const char* pszFunction, int line)
{
	if (ttdbg::bNoAssert)
		return false;

	crtAssert.Lock();
	bool bResult;
	{	// use a brace so that cszMsg gets deleted before we release the critical section
		ttCStr cszMsg;

		// We special case a null or empty pszMsg -- ttASSERT_NONEMPTY(ptr) takes advantage of this

		if (!pwszMsg)
			cszMsg = "NULL pointer!";
		else if (!*pwszMsg)
			cszMsg = "Empty string!";
		else
			cszMsg = pwszMsg;

		bResult = tt::AssertionMsg((char*) cszMsg, pszFile, pszFunction, line);
	}
	crtAssert.Unlock();
	return bResult;
}

// Displays a message box displaying the catch message with an option to ignore, break into a debugger, or exit the program. Example:
//
// try {
//	   ...
// }
// catch (...) {
//	   CATCH_HANDLER("%s (%d) : Exception in %s()", __FILE__, __LINE__, __func__);
// }

void _cdecl tt::CATCH_HANDLER(const char* pszFormat, ...)
{
	ttASSERT(pszFormat);
	if (!pszFormat)
		return;

	ttStr strMsg;
	va_list argList;
	va_start(argList, pszFormat);
	tt::vprintf(&strMsg.m_psz, pszFormat, argList);
	va_end(argList);

#ifdef _WINDOWS_

	int answer = MessageBoxA(GetActiveWindow(), strMsg, "Retry to call DebugBreak()", MB_ABORTRETRYIGNORE);

	if (answer == IDRETRY) {
		DebugBreak();
		return;
	}
	else if (answer != IDIGNORE) {
		if (tt::hwndParent && IsWindow(tt::hwndParent))
			SendMessage(tt::hwndParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down
		ExitProcess((UINT) -1);
	}

#else	// not _WINDOWS_

#ifdef _WX_WX_H_
	wxFAIL_MSG(strMsg);
#else	// not _WX_WX_H_
	asm volatile ("int $3");
#endif	// _WX_WX_H_

#endif	// _WINDOWS_
}

#ifdef _WINDOWS_

void tt::doReportLastError(const char* pszFile, const char* pszFunc, int line)
{
	char* pszMsg;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &pszMsg, 0, NULL);

	AssertionMsg(pszMsg, pszFile, pszFunc, line);

	LocalFree((HLOCAL) pszMsg);
}

DWORD tt::CheckItemID(HWND hwnd, int id, const char* pszID, const char* pszFile, const char* pszFunc, int line)
{
	if (::GetDlgItem(hwnd, id) == NULL) {
		ttCStr cszMsg;
		cszMsg.printf("Invalid dialog control id: %s (%u)", pszID, id);
		tt::AssertionMsg(cszMsg, pszFile, pszFunc, line);
	}
	return id;
}

#endif	// _WINDOWS_

#endif	// _DEBUG
