/////////////////////////////////////////////////////////////////////////////
// Name:      ttwinspace.cpp
// Purpose:   Windows-only ttlib namespace functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 1992-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// All the functions in this module will ONLY work on a Windows OS

#include "pch.h"  // precompiled header

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <string>

#include <shellapi.h>

#include "ttdebug.h"
#include "ttlibspace.h"
#include "utf8unchecked.h"

namespace ttlib
{
    std::wstring MsgBoxTitle;
}

void ttlib::SetMsgBoxTitle(std::string_view utf8Title)
{
    utf8::unchecked::utf8to16(utf8Title.begin(), utf8Title.end(), back_inserter(ttlib::MsgBoxTitle));
}

int ttlib::MsgBox(std::string_view utf8str, UINT uType)
{
    std::wstring str16;
    utf8::unchecked::utf8to16(utf8str.begin(), utf8str.end(), back_inserter(str16));
    return MessageBoxW(GetActiveWindow(), str16.c_str(),
                       (!ttlib::MsgBoxTitle.empty() ? ttlib::MsgBoxTitle.c_str() : L""), uType);
}

int ttlib::MsgBox(std::string_view utf8str, std::string_view utf8Caption, UINT uType)
{
    std::wstring str16;
    utf8::unchecked::utf8to16(utf8str.begin(), utf8str.end(), back_inserter(str16));
    std::wstring caption16;
    utf8::unchecked::utf8to16(utf8Caption.begin(), utf8Caption.end(), back_inserter(caption16));

    return MessageBoxW(GetActiveWindow(), str16.c_str(), caption16.c_str(), uType);
}
bool ttlib::GetWndText(HWND hwnd, std::string& str)
{
    int cb = GetWindowTextLengthW(hwnd);
    if (cb > 0)
    {
        wchar_t* buffer = static_cast<wchar_t*>(std::malloc((cb + 1) * sizeof(wchar_t)));
        cb = GetWindowTextW(hwnd, buffer, cb);
        std::wstring_view str16(buffer, cb);
        utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(str));
        std::free(static_cast<void*>(buffer));
        return true;
    }
    else
    {
        str.assign(ttlib::emptystring);
        return false;
    }
}

std::string ttlib::GetWndText(HWND hwnd)
{
    std::string str;
    ttlib::GetWndText(hwnd, str);
    return str;
}

std::string ttlib::GetListboxText(HWND hwnd, WPARAM index)
{
    std::string str;
    ttlib::GetListboxText(hwnd, index, str);
    return str;
}

bool ttlib::GetListboxText(HWND hwnd, WPARAM index, std::string& str)
{
    auto cb = SendMessageW(hwnd, LB_GETTEXTLEN, index, 0);
    if (cb != LB_ERR)
    {
        wchar_t* buffer = static_cast<wchar_t*>(std::malloc((cb + 1) * sizeof(wchar_t)));
        cb = SendMessageW(hwnd, LB_GETTEXT, index, (WPARAM) buffer);
        if (cb != LB_ERR)
        {
            std::wstring_view str16(buffer, cb);
            utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(str));
        }
        else
        {
            str.assign(ttlib::emptystring);
        }

        std::free(static_cast<void*>(buffer));
    }
    else
    {
        str.assign(ttlib::emptystring);
    }
    return (cb != LB_ERR);
}

std::string ttlib::GetComboLBText(HWND hwnd, WPARAM index)
{
    std::string str;
    ttlib::GetComboLBText(hwnd, index, str);
    return str;
}

bool ttlib::GetComboLBText(HWND hwnd, WPARAM index, std::string& str)
{
    auto cb = SendMessageW(hwnd, CB_GETLBTEXTLEN, index, 0);
    if (cb != CB_ERR)
    {
        wchar_t* buffer = static_cast<wchar_t*>(std::malloc((cb + 1) * sizeof(wchar_t)));
        cb = SendMessageW(hwnd, CB_GETLBTEXT, index, (WPARAM) buffer);
        if (cb != CB_ERR)
        {
            std::wstring_view str16(buffer, cb);
            utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(str));
        }
        else
        {
            str.assign(ttlib::emptystring);
        }

        std::free(static_cast<void*>(buffer));
    }
    else
    {
        str.assign(ttlib::emptystring);
    }
    return (cb != CB_ERR);
}

void ttlib::SetWndText(HWND hwnd, std::string_view utf8str)
{
    std::wstring str16;
    utf8::unchecked::utf8to16(utf8str.begin(), utf8str.end(), back_inserter(str16));
    SetWindowTextW(hwnd, str16.c_str());
}

std::string ttlib::LoadTextResource(DWORD idResource, HMODULE hmodResource)
{
    std::string str;
    auto hrsrc = FindResourceA(hmodResource, MAKEINTRESOURCEA(idResource), (char*) RT_RCDATA);
    ttASSERT(hrsrc);
    if (!hrsrc)
        return str;
    HGLOBAL hglb = LoadResource(hmodResource, hrsrc);
    ttASSERT(hglb);
    if (!hglb)
        return str;
    auto pbuf = LockResource(hglb);
    ttASSERT(pbuf);
    if (!pbuf)
        return str;
    str.assign(static_cast<char*>(pbuf), static_cast<size_t>(SizeofResource(hmodResource, hrsrc)));
    return str;
}

HINSTANCE ttlib::ShellRun(std::string_view filename, std::string_view args, std::string_view dir, INT nShow,
                          HWND hwndParent)
{
    std::wstring name16;
    utf8::unchecked::utf8to16(filename.begin(), filename.end(), back_inserter(name16));
    std::wstring args16;
    utf8::unchecked::utf8to16(args.begin(), args.end(), back_inserter(args16));
    std::wstring dir16;
    utf8::unchecked::utf8to16(dir.begin(), dir.end(), back_inserter(dir16));

    return ShellExecuteW(hwndParent, NULL, name16.c_str(), args16.c_str(), dir16.c_str(), nShow);
}

//////////////////////////////// Windows-only ttlib::cstr functions ////////////////////////

#include "ttcstr.h"

using namespace ttlib;

cstr& cstr::GetWndText(HWND hwnd)
{
    int cb = GetWindowTextLengthW(hwnd);
    if (cb > 0)
    {
        wchar_t* buffer = static_cast<wchar_t*>(std::malloc((cb + 1) * sizeof(wchar_t)));
        cb = GetWindowTextW(hwnd, buffer, cb);
        std::wstring_view str16(buffer, cb);
        utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(*this));
        std::free(static_cast<void*>(buffer));
        return *this;
    }
    else
    {
        assign(ttlib::emptystring);
        return *this;
    }
}

cstr& cstr::GetResString(size_t idString, HMODULE hmodResource)
{
    clear();

    auto hrsrc = FindResourceA(hmodResource, MAKEINTRESOURCEA(idString), (char*) RT_STRING);
    ttASSERT(hrsrc);
    if (!hrsrc)
        return *this;
    HGLOBAL hglb = LoadResource(hmodResource, hrsrc);
    ttASSERT(hglb);
    if (!hglb)
        return *this;
    auto pbuf = LockResource(hglb);
    ttASSERT(pbuf);
    if (!pbuf)
        return *this;
    assign(static_cast<char*>(pbuf), static_cast<size_t>(SizeofResource(hmodResource, hrsrc)));
    return *this;
}

cstr& cstr::GetListBoxText(HWND hwndCtrl, size_t sel)
{
    if (sel == tt::npos)
    {
        sel = SendMessageW(hwndCtrl, LB_GETCURSEL, 0, 0);
        if (sel == LB_ERR)
        {
            assign(ttlib::emptystring);
            return *this;
        }
    }

    auto cb = SendMessageW(hwndCtrl, LB_GETTEXTLEN, sel, 0);
    if (cb != LB_ERR)
    {
        wchar_t* buffer = static_cast<wchar_t*>(std::malloc((cb + 1) * sizeof(wchar_t)));
        cb = SendMessageW(hwndCtrl, LB_GETTEXT, sel, (WPARAM) buffer);
        if (cb != LB_ERR)
        {
            std::wstring_view str16(buffer, cb);
            utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(*this));
        }
        else
        {
            assign(ttlib::emptystring);
        }

        std::free(static_cast<void*>(buffer));
    }
    else
    {
        assign(ttlib::emptystring);
    }

    return *this;
}

cstr& cstr::GetComboLBText(HWND hwndCtrl, size_t sel)
{
    if (sel == tt::npos)
    {
        sel = SendMessageW(hwndCtrl, CB_GETCURSEL, 0, 0);
        if (sel == CB_ERR)
        {
            assign(ttlib::emptystring);
            return *this;
        }
    }

    auto cb = SendMessageW(hwndCtrl, CB_GETLBTEXTLEN, sel, 0);
    if (cb != LB_ERR)
    {
        wchar_t* buffer = static_cast<wchar_t*>(std::malloc((cb + 1) * sizeof(wchar_t)));
        cb = SendMessageW(hwndCtrl, CB_GETLBTEXT, sel, (WPARAM) buffer);
        if (cb != LB_ERR)
        {
            std::wstring_view str16(buffer, cb);
            utf8::unchecked::utf16to8(str16.begin(), str16.end(), back_inserter(*this));
        }
        else
        {
            assign(ttlib::emptystring);
        }

        std::free(static_cast<void*>(buffer));
    }
    else
    {
        assign(ttlib::emptystring);
    }

    return *this;
}
