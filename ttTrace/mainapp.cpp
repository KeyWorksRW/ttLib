/////////////////////////////////////////////////////////////////////////////
// Name:		mainapp.cpp
// Purpose:		Application starting point
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:     Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "aboutdlg.h"
#include "mainfrm.h"
#include "resource.h"
#include "traceview.h"

CAppModule _Module;

// REVIEW: [randalphwa - 3/4/2019] This will share settings with the older KeyView. Is that a good thing?
const char* txtKeyViewRegKey = "Software\\KeyWorks\\KeyView";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /* lpstrCmdLine */, int nCmdShow)
{
    HWND hwnd = FindWindow("KeyViewMsgs", NULL);
    if (hwnd)
    {
        // already running, so activate other instance and terminate this instance
        ::SetForegroundWindow(hwnd);
        return 0;
    }

#ifdef _DEBUG
    HRESULT hRes =
#endif
        ::CoInitialize(NULL);
    // If you are running on NT 4.0 or higher you can use the following call instead to
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ttASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_BAR_CLASSES);  // add flags to support other controls

#ifdef _DEBUG
    hRes =
#endif
        _Module.Init(NULL, hInstance, &LIBID_ATLLib);
    ttASSERT(SUCCEEDED(hRes));

    HMODULE hInstRich = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
    ttASSERT(hInstRich != NULL);

    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CMainFrame wndMain;
    RECT* prc = 0;
    RECT rc = { 0, 0, 0, 0 };

    CRegKey key;
    if (key.Open(HKEY_CURRENT_USER, txtKeyViewRegKey) == ERROR_SUCCESS)
    {
        DWORD dwType = REG_BINARY;
        DWORD dwSize = sizeof(RECT);
        if ((RegQueryValueEx(key, "WindowRect", 0, &dwType, (BYTE*) &rc, &dwSize) == ERROR_SUCCESS) &&
            (REG_BINARY == dwType) && (sizeof(RECT) == dwSize) && !IsRectEmpty(&rc))
            prc = &rc;
    }

    if (wndMain.CreateEx(0, prc) == NULL)
    {
        ttFAIL("Main window creation failed!\n");
        return 0;
    }
    ttInitCaller(txtAppName);

    wndMain.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();

    ::FreeLibrary(hInstRich);

    // [ralphw - 12-10-2003] Appears to be entirely unnecesary
    // _Module.Term();
    ::CoUninitialize();

    return nRet;
}
