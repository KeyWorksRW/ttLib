// This header file is used to create a pre-compiled header for use in the entire project

#pragma once

#ifndef _WIN32_WINNT_VISTA
	#define _WIN32_WINNT_NT4    0x0400
	#define _WIN32_WINNT_WINXP  0x0501
	#define _WIN32_WINNT_VISTA  0x0600
	#define _WIN32_WINNT_WIN7   0x0601
	#define _WIN32_WINNT_WIN8   0x0602
	#define _WIN32_WINNT_WIN10  0x0A00
#endif

#define WINVER 		 _WIN32_WINNT_VISTA		// minimum OS required
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#define STRICT
#define WIN32_LEAN_AND_MEAN

#define _WTL_NEW_PAGE_NOTIFY_HANDLERS
#define _ATL_NO_DEBUG_CRT

#define _WINSOCKAPI_	// so atlbase.h won't pull in WinSock2.h

#include <windows.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "gdi32.lib")

#include <ttdebug.h>	// ttASSERT macros
#define ATLASSERT(expr) ttASSERT(expr)

// Turn off ATL warnings

#include <atldef.h>
#include <atlbase.h>
#include <atlapp.h>

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>

#include <stdint.h>

#include <ttlib.h>	// Master header file for ttLib

extern const char* txtVersion;
extern const char* txtCopyRight;

#include "funcs.h"
#include "strtable.h"

typedef struct {
	BOOL fKeepOnTop:1;

	BOOL fEventMessages:1;
	BOOL fPropertyMessages:1;
	BOOL fKeyHelpMessages:1;
	BOOL fGeneralMessages:1;
	BOOL fScriptMessages:1;

	// events

	BOOL fTCard:1;
	BOOL fTCardText:1;
	BOOL fUserLevelChange:1;
	BOOL fInformationTypeChange:1;
	BOOL fStatusTextChange:1;
	BOOL fProgressChange:1;
	BOOL fCommandStateChange:1;
	BOOL fDownloadBegin:1;
	BOOL fDownloadComplete:1;
	BOOL fTitleChange:1;
	BOOL fWebBrowserPropertyChange:1;
	BOOL fBeforeNavigate:1;
	BOOL fNewWindow:1;
	BOOL fNavigateComplete:1;
	BOOL fDocumentComplete:1;

	BOOL fToolbarButton:1;

	// property changes

	BOOL fChmFile:1;
	BOOL fInitialMapId:1;
	BOOL fUserLevel:1;
	BOOL fDefaultTopic:1;
	BOOL fConceptualInformation:1;
	BOOL fTimedInformation:1;
	BOOL fCustomInformation:1;
	BOOL fVerticalScrollBar:1;
	BOOL fHorizontalScrollBar:1;
	BOOL fWindowStyle:1;
	BOOL fWindowStyleEx:1;

	BOOL fHomeBtn:1;
	BOOL fBackBtn:1;
	BOOL fForwardBtn:1;
	BOOL fStopBtn:1;
	BOOL fRefreshBtn:1;
	BOOL fPrintBtn:1;
	BOOL fCloseBtn:1;
	BOOL fOptionsBtn:1;
	BOOL fBrowseButtons:1;
	BOOL fCustomID:1;
	BOOL fCustomLabel:1;
	BOOL fCustomImage:1;
	BOOL fCustomHandler:1;
	BOOL fBackgroundColor:1;	// popup background color

	// script messages

	BOOL fControlPanel:1;
	BOOL fJumpChm:1;
	BOOL fDisplayDocument:1;
	BOOL fLaunchTriPane:1;
	BOOL fApplyInfoTypes:1;
	BOOL fSetUserLevel:1;
	BOOL fSetConceptualInfoType:1;
	BOOL fSetCustomInfoType:1;
	BOOL fToggleConceptualInformation:1;
	BOOL fToggleCustomInformation:1;
	BOOL fInitializeUserLevelInput:1;
	BOOL fRelatedTopicsMenu:1;
	BOOL fShortCut:1;
	BOOL fSendMessage:1;

	DWORD reserved1;	// reserved for future expansion
	DWORD reserved2;
	DWORD reserved3;
} PROFILE;

extern PROFILE uprof;
