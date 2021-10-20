/////////////////////////////////////////////////////////////////////////////
// Name:      ttregistry.cpp
// Purpose:   Class for working with the Windows registry
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../../LICENSE
/////////////////////////////////////////////////////////////////////////////

#include <winreg.h>

#include "ttlibspace.h"
#include "ttregistry.h"

using namespace ttlib;

bool registry::Open(std::string_view name, REGSAM access, HKEY hkey)
{
    if (m_hkey)
        RegCloseKey(m_hkey);

    std::wstring name16;
    utf8to16(name, name16);
    auto result = RegOpenKeyExW(hkey, name16.c_str(), 0, access, &m_hkey);
    if (result == ERROR_SUCCESS)
        return true;
    m_hkey = NULL;

    return false;
}

bool registry::Create(std::string_view name, HKEY hkey)
{
    if (m_hkey)
        RegCloseKey(m_hkey);

    std::wstring name16;
    utf8to16(name, name16);
    return (RegCreateKeyExW(hkey, name16.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hkey, NULL) ==
            ERROR_SUCCESS);
}

std::string registry::ReadString(std::string_view name)
{
    std::string result;
    if (m_hkey == NULL)
        return result;

    std::wstring name16;
    ttlib::utf8to16(name, name16);
    DWORD type;
    DWORD size = 0;
    RegQueryValueExW(m_hkey, name16.c_str(), NULL, &type, NULL, &size);
    if (!size || (type != REG_SZ && type != REG_EXPAND_SZ))
        return result;

    auto str16 = std::make_unique<wchar_t[]>(size + 1);

    RegQueryValueExW(m_hkey, name16.c_str(), NULL, &type, reinterpret_cast<LPBYTE>(str16.get()), &size);
    ttlib::utf16to8(str16.get(), result);
    return result;
}

bool registry::WriteString(std::string_view name, std::string_view value)
{
    if (m_hkey == NULL)
        return false;
    std::wstring name16;
    ttlib::utf8to16(name, name16);
    std::wstring value16;
    ttlib::utf8to16(value, value16);

    return (RegSetValueExW(m_hkey, name16.c_str(), 0, REG_SZ, reinterpret_cast<LPBYTE>(value16.data()),
                           static_cast<DWORD>(value16.size()) + sizeof(wchar_t)) == ERROR_SUCCESS);
}

ttlib::multistr registry::ReadEnumstr(std::string_view name)
{
    ttlib::multistr result(ReadString(name));
    return result;
}

bool registry::WriteEnumstr(std::string_view name, const ttlib::multistr& estr)
{
    std::string str;
    for (auto& iter: estr)
    {
        if (!str.empty())
            str += ";";
        str += iter;
    }
    return WriteString(name, str);
}

size_t registry::ReadValue(std::string_view name)
{
    if (m_hkey == NULL)
        return tt::npos;
    std::wstring name16;
    ttlib::utf8to16(name, name16);
    size_t result;
    DWORD type;
    DWORD size = 0;

    if (RegQueryValueExW(m_hkey, name16.c_str(), NULL, &type, reinterpret_cast<LPBYTE>(&result), &size) != ERROR_SUCCESS)
    {
        return tt::npos;
    }
#if defined(_WIN64)
    if (type != REG_QWORD)
        return tt::npos;
#else
    if (type != REG_DWORD)
        return tt::npos;
#endif
    return size;
}

bool registry::WriteValue(std::string_view name, size_t value)
{
    if (m_hkey == NULL)
        return false;
    std::wstring name16;
    ttlib::utf8to16(name, name16);

    return (RegSetValueExW(m_hkey, name16.c_str(), 0, sizeof(value) == 32 ? REG_DWORD : REG_QWORD,
                           reinterpret_cast<LPBYTE>(&value), sizeof(value)) == ERROR_SUCCESS);
}
