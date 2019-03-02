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
#include "../include/ttstr.h"		// ttCStr
#include "../include/ttcritsection.h"	// CCritSection

bool (_cdecl *pttAssertHandlerA)(const char* pszMsg, const char* pszFile, const char* pszFunction, int line) = nullptr;
bool (_cdecl *pttAssertHandlerW)(const wchar_t* pszMsg, const char* pszFile, const char* pszFunction, int line) = nullptr;

namespace ttdbg {
	ttCCritSection crtAssert;
	bool bNoAssert = false;		// Setting this to true will cause AssertionMsg to return without doing anything
	bool bNoRecurse = false;
}

using namespace ttdbg;

// Displays a message box displaying the ASSERT with an option to ignore, break into a debugger, or exit the program

bool tt::AssertionMsg(const char* pszMsg, const char* pszFile, const char* pszFunction, int line)
{
	if (ttdbg::bNoAssert)
		return false;

	// if pttAssertHandlerA calls us, then we need to use a normal assert message

	if (pttAssertHandlerA && !bNoRecurse) {
		bNoRecurse = true;
		bool bResult = pttAssertHandlerA(pszMsg, pszFile, pszFunction, line);
		bNoRecurse = false;
		return bResult;
	}

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

	if (tt::hwndMsgBoxParent && IsWindow(tt::hwndMsgBoxParent))
		SendMessage(tt::hwndMsgBoxParent, WM_CLOSE, 0, 0);	// attempt to give the application window a graceful way to shut down

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

	if (pttAssertHandlerW)
		return pttAssertHandlerW(pwszMsg, pszFile, pszFunction, line);

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

void tt::SetAsserts(bool bDisable)
{
	ttdbg::bNoAssert = bDisable;
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
