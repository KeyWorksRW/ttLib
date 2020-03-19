/////////////////////////////////////////////////////////////////////////////
// Name:      ttdirdlg.cpp
// Purpose:   Class for displaying a dialog to select a directory
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

//  Derived from:  http://code.msdn.microsoft.com/CppShellCommonFileDialog-17b20409

#include "pch.h"

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <shlobj.h>

#include "ttTR.h"
#include "ttlibspace.h"
#include "ttdebug.h"   // ttASSERT macros

#include "ttdirdlg.h"  // DirDlg

using namespace ttlib;

#pragma comment(lib, "ole32.lib")

DirDlg::DirDlg()
{
    m_Title = _tt("Select a Folder");
}

bool DirDlg::GetFolderName(HWND hwndParent)
{
    clear();
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

    std::wstring title16;
    ttlib::utf8to16(m_Title, title16);

    if (!m_StartingDir.empty())
    {
        pfd->ClearClientData();
        IShellItem* psiFolder;

        std::wstring dir16;
        ttlib::utf8to16(m_StartingDir, dir16);

        hr = SHCreateItemFromParsingName(dir16.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
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
                    assignUTF16(pwszPath);
                    CoTaskMemFree(pwszPath);
                }
                psiResult->Release();
            }
        }
    }
    pfd->Release();
    return (SUCCEEDED(hr) ? true : false);
}
