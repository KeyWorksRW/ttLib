/////////////////////////////////////////////////////////////////////////////
// Purpose:   Additional functions for wxWidgets on Windows
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see LICENSE
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <shellapi.h>

#include "ttstr.h"  // ttString -- Enhanced version of wxString

HINSTANCE ttlib::ShellRun_wx(const wxString& filename, const wxString& args, const wxString& dir, INT nShow, HWND hwndParent)
{
    return ShellExecuteW(hwndParent, NULL, filename.c_str(), args.c_str(), dir.c_str(), nShow);
}

wxString ttlib::LoadStringEx_wx(WORD id)
{
    wxString str;
    ttlib::LoadStringEx(str, id);
    return str;
}

bool ttlib::LoadStringEx(wxString& Result, WORD id)
{
    // Setup result in case resource can't be loaded, resource length is invliad, or string can't be found

#if defined(_DEBUG)
    {
        Result = "String resource not found: ";
        Result << id;
    }
#else
    Result.clear();  // Clear the string in case we return false;
#endif

    // Strings are in blocks of 16 items per block, so find the block containing the string we want

    UINT blockNumber = (id >> 4) + 1;
    auto hres = FindResourceEx(lang_info.hinstResource, RT_STRING, MAKEINTRESOURCE(blockNumber),
                               MAKELANGID(lang_info.PrimaryLanguage, lang_info.SubLanguage));
    if (!hres)
        return false;

    auto sizeRes = SizeofResource(lang_info.hinstResource, hres);
    auto memory = LoadResource(lang_info.hinstResource, hres);

    if (!sizeRes || !memory)
        return false;

    auto begin = static_cast<wchar_t*>(LockResource(memory));
    if (!begin)
        return false;
    auto end = begin + sizeRes;

    // Strings in the block are Pascal-style -- first a WORD specifying the length, followed by the actual string.

    WORD idx = 0;
    WORD indexNumber = id % 16;
    while (begin < end)
    {
        auto length = static_cast<WORD>(begin[0]);
        if (idx == indexNumber)
        {
            // Note that the string can be zero-length, but we still return true since we did find the string.

            Result.assign(begin + 1, 0, length);
            return true;
        }
        else
        {
            // Step over the lead string width and the string itself
            begin += (length + 1);
            ++idx;
        }
    }
    return false;
}
