/////////////////////////////////////////////////////////////////////////////
// Name:      ttwinspace.cpp
// Purpose:   Windows-only ttlib namespace functions
// Author:    Ralph Walden
// Copyright: Copyright (c) 1992-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"  // precompiled header

#if !defined(_WIN32)
    #error "This module can only be compiled for Windows"
#endif

#include <string>
#include <string_view>
#include <vector>

#include <shellapi.h>

#include "ttcstr.h"   // cstr -- Classes for handling zero-terminated char strings.
#include "ttdebug.h"  // ttASSERT macros
#include "ttlibspace.h"

namespace ttlib
{
    std::wstring MsgBoxTitle;
}

void ttlib::SetMsgBoxTitle(std::string_view utf8Title)
{
    ttlib::utf8to16(utf8Title, ttlib::MsgBoxTitle);
}

int ttlib::MsgBox(std::string_view utf8str, UINT uType)
{
    std::wstring str16;
    ttlib::utf8to16(utf8str, str16);
    return MessageBoxW(GetActiveWindow(), str16.c_str(), (!ttlib::MsgBoxTitle.empty() ? ttlib::MsgBoxTitle.c_str() : L""), uType);
}

int ttlib::MsgBox(std::string_view utf8str, std::string_view utf8Caption, UINT uType)
{
    std::wstring str16;
    ttlib::utf8to16(utf8str, str16);
    std::wstring caption16;
    ttlib::utf8to16(utf8Caption, caption16);

    return MessageBoxW(GetActiveWindow(), str16.c_str(), caption16.c_str(), uType);
}

int ttlib::MsgBox(WORD idStrResource, UINT uType)
{
    std::wstring str16;
    ttlib::utf8to16(LoadStringEx(idStrResource), str16);

    return MessageBoxW(GetActiveWindow(), str16.c_str(), (!ttlib::MsgBoxTitle.empty() ? ttlib::MsgBoxTitle.c_str() : L""), uType);
}

bool ttlib::GetWndText(HWND hwnd, std::string& str)
{
    auto len = GetWindowTextLengthW(hwnd);
    if (len > 0)
    {
        ++len;  // Add room for trailing zero

        auto str16 = std::make_unique<wchar_t[]>(len).get();
        len = GetWindowTextW(hwnd, str16, len);
        ttlib::utf16to8({ str16, static_cast<size_t>(len) }, str);
        return true;
    }
    else
    {
        str.assign(ttlib::emptystring);
        return false;
    }
}

ttlib::cstr ttlib::GetWndText(HWND hwnd)
{
    ttlib::cstr str;
    ttlib::GetWndText(hwnd, str);
    return str;
}

ttlib::cstr ttlib::GetListboxText(HWND hwnd, WPARAM index)
{
    ttlib::cstr str;
    ttlib::GetListboxText(hwnd, index, str);
    return str;
}

bool ttlib::GetListboxText(HWND hwnd, WPARAM index, std::string& str)
{
    str.clear();

    auto cb = ttlib::SendMsg(hwnd, LB_GETTEXTLEN, index);
    if (!ttlib::isError(cb))
    {
        auto str16 = std::make_unique<wchar_t[]>(cb + 1);
        cb = ttlib::SendMsg(hwnd, LB_GETTEXT, index, str16.get());
        if (!ttlib::isError(cb))
        {
            ttlib::utf16to8({ str16.get(), static_cast<size_t>(cb) }, str);
        }
    }

    return !ttlib::isError(cb);
}

ttlib::cstr ttlib::GetComboLBText(HWND hwnd, WPARAM index)
{
    ttlib::cstr str;
    ttlib::GetComboLBText(hwnd, index, str);
    return str;
}

bool ttlib::GetComboLBText(HWND hwnd, WPARAM index, std::string& str)
{
    str.clear();

    auto cb = ttlib::SendMsg(hwnd, CB_GETLBTEXTLEN, index);
    if (!ttlib::isError(cb))
    {
        auto str16 = std::make_unique<wchar_t[]>(cb + 1);
        cb = SendMessageW(hwnd, CB_GETLBTEXT, index, (WPARAM) str16.get());
        if (!ttlib::isError(cb))
        {
            ttlib::utf16to8({ str16.get(), static_cast<size_t>(cb) }, str);
        }
    }

    return !ttlib::isError(cb);
}

void ttlib::SetWndText(HWND hwnd, std::string_view utf8str)
{
    std::wstring str16;
    ttlib::utf8to16(utf8str, str16);
    SetWindowTextW(hwnd, str16.c_str());
}

ttlib::cstr ttlib::LoadTextResource(DWORD idResource, HMODULE hmodResource)
{
    ttlib::cstr str;
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

HINSTANCE ttlib::ShellRun(std::string_view filename, std::string_view args, std::string_view dir, INT nShow, HWND hwndParent)
{
    std::wstring name16;
    ttlib::utf8to16(filename, name16);
    std::wstring args16;
    ttlib::utf8to16(args, args16);
    std::wstring dir16;
    ttlib::utf8to16(dir, dir16);

    return ShellExecuteW(hwndParent, NULL, name16.c_str(), args16.c_str(), dir16.c_str(), nShow);
}

HFONT ttlib::CreateLogFont(std::string_view TypeFace, size_t point, bool Bold, bool Italics)
{
    HDC hdc = CreateCompatibleDC(NULL);
    SetMapMode(hdc, MM_TEXT);

    LOGFONTW lf;
    ZeroMemory(&lf, sizeof(LOGFONTW));

    int ratio = MulDiv(GetDeviceCaps(hdc, LOGPIXELSY), 100, 72);
    lf.lfHeight = MulDiv((int) point, ratio, 100);
    if ((point * ratio) % 100 >= 50)
        lf.lfHeight++;

    lf.lfHeight = -lf.lfHeight;
    lf.lfItalic = Italics ? 1 : 0;
    if (Bold)
        lf.lfWeight = FW_BOLD;

    std::wstring name16;
    ttlib::utf8to16(TypeFace, name16);
    ttASSERT(name16.length() < LF_FACESIZE);
    HFONT hfont = nullptr;
    if (name16.length() < LF_FACESIZE)
    {
        std::wcscpy(lf.lfFaceName, name16.c_str());
        hfont = CreateFontIndirectW(&lf);
    }

    DeleteDC(hdc);

    return hfont;
}

//////////////////////////////// Windows-only ttlib::cmd functions ////////////////////////

#include "ttparser.h"

ttlib::cmd::cmd()
{
    int argc;
    auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    for (auto argpos = 1; argpos < argc; ++argpos)
    {
        auto& arg = m_originalArgs.emplace_back();
        arg.assignUTF16(argv[argpos]);
    }
    LocalFree(argv);
}

//////////////////////////////// Windows-only ttlib::cstr functions ////////////////////////

#include "ttcstr.h"

using namespace ttlib;

cstr& cstr::GetWndText(HWND hwnd)
{
    clear();
    auto len = GetWindowTextLengthW(hwnd);
    if (len > 0)
    {
        ++len;
        auto str16 = std::make_unique<wchar_t[]>(len);
        len = GetWindowTextW(hwnd, str16.get(), len);
        ttlib::utf16to8({ str16.get(), static_cast<size_t>(len) }, *this);
    }
    return *this;
}

cstr& cstr::GetListBoxText(HWND hwndCtrl, size_t sel)
{
    clear();

    if (ttlib::isError(sel))
    {
        sel = ttlib::SendMsg(hwndCtrl, LB_GETCURSEL);
        if (ttlib::isError(sel))
        {
            return *this;
        }
    }

    auto len = ttlib::SendMsg(hwndCtrl, LB_GETTEXTLEN, sel);
    if (!ttlib::isError(len))
    {
        auto str16 = std::make_unique<wchar_t[]>(len + 1);
        len = SendMsg(hwndCtrl, LB_GETTEXT, sel, str16.get());
        if (!ttlib::isError(len))
        {
            ttlib::utf16to8({ str16.get(), static_cast<size_t>(len) }, *this);
        }
    }

    return *this;
}

cstr& cstr::GetComboLBText(HWND hwndCtrl, size_t sel)
{
    clear();

    if (ttlib::isError(sel))
    {
        sel = ttlib::SendMsg(hwndCtrl, CB_GETCURSEL);
        if (ttlib::isError(sel))
        {
            return *this;
        }
    }

    auto len = ttlib::SendMsg(hwndCtrl, CB_GETLBTEXTLEN, sel);
    if (!ttlib::isError(len))
    {
        auto str16 = std::make_unique<wchar_t[]>(len + 1);
        len = ttlib::SendMsg(hwndCtrl, CB_GETLBTEXT, sel, str16.get());
        if (!ttlib::isError(len))
        {
            ttlib::utf16to8({ str16.get(), static_cast<size_t>(len) }, *this);
        }
    }

    return *this;
}
