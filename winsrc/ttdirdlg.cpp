/////////////////////////////////////////////////////////////////////////////
// Name:      ttdirdlg.cpp
// Purpose:   Class for displaying a dialog to select a directory
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

//  Derived from:  http://code.msdn.microsoft.com/CppShellCommonFileDialog-17b20409

#include "pch.h"

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#if !defined(TTALL_LIB)
    #if defined(NDEBUG)
        #pragma comment(lib, "ttLibwin.lib")
    #else
        #pragma comment(lib, "ttLibwinD.lib")
    #endif
#endif

#include "../include/ttdirdlg.h"  // ttCDirDlg
#include "../include/ttdebug.h"   // ttASSERT macros

#pragma comment(lib, "ole32.lib")

ttCDirDlg::ttCDirDlg()
{
    m_cwszTitle = L"Select a Folder";
}

bool ttCDirDlg::GetFolderName(HWND hwndParent)
{
    IFileOpenDialog* pfd;
    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (FAILED(hr))
    {
        ttASSERT_HRESULT(hr, "Could not create IFileOpenDialog interface");
        return false;
    }

    DWORD dwOptions;
    hr = pfd->GetOptions(&dwOptions);
    if (SUCCEEDED(hr))
        hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
    pfd->SetTitle(m_cwszTitle);

    if (m_cwszStartingDir.IsNonEmpty())
    {
        pfd->ClearClientData();
        IShellItem* psiFolder;
        hr = SHCreateItemFromParsingName(m_cwszStartingDir, NULL, IID_PPV_ARGS(&psiFolder));
        if (SUCCEEDED(hr))
            pfd->SetDefaultFolder(psiFolder);
    }
    if (SUCCEEDED(hr))
    {
        hr = pfd->Show(hwndParent);
        if (SUCCEEDED(hr))  // Get the selection from the user.
        {
            IShellItem* psiResult = NULL;
            hr = pfd->GetResult(&psiResult);
            if (SUCCEEDED(hr))
            {
                PWSTR pwszPath = NULL;
                hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pwszPath);
                if (SUCCEEDED(hr))
                {
                    m_cszDirName = pwszPath;
                    CoTaskMemFree(pwszPath);
                }
                psiResult->Release();
            }
        }
    }
    pfd->Release();
    return (SUCCEEDED(hr) ? true : false);
}
