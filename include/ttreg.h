/////////////////////////////////////////////////////////////////////////////
// Name:      ttCRegistry class
// Purpose:   Header-only class for working with the Windows registry
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(_WIN32)

// Header-only class

class ttCRegistry
{
public:
    ttCRegistry() { m_hkey = NULL; }
    ttCRegistry(const char* pszKey, bool fWrite = true)
    {
        if (RegOpenKeyExA(HKEY_CURRENT_USER, pszKey, 0, fWrite ? KEY_ALL_ACCESS : KEY_READ, &m_hkey) != ERROR_SUCCESS)
            m_hkey = NULL;
    }
    ~ttCRegistry()
    {
        if (m_hkey)
            RegCloseKey(m_hkey);
    }

    bool Open(HKEY hkeyBase, const char* pszKey, bool fWrite = true)
    {
        if (m_hkey)
            RegCloseKey(m_hkey);
        return (RegOpenKeyExA(hkeyBase, pszKey, 0, fWrite ? KEY_ALL_ACCESS : KEY_READ, &m_hkey) == ERROR_SUCCESS);
    }
    bool Create(HKEY hkeyBase, const char* pszKey)
    {
        if (m_hkey)
            RegCloseKey(m_hkey);
        return (RegCreateKeyExA(hkeyBase, pszKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hkey,
                                NULL) == ERROR_SUCCESS);
    }
    void Close()
    {
        if (m_hkey)
            RegCloseKey(m_hkey), m_hkey = NULL;
    }

    bool WriteString(const char* pszKey, const char* pszValue)
    {
        return (RegSetValueExA(m_hkey, pszKey, 0, REG_SZ, (PBYTE) pszValue, (DWORD) ttStrLen(pszValue) + 1) ==
                ERROR_SUCCESS);
    }
    bool ReadString(const char* pszName, PSTR pszDst, DWORD cbDst = MAX_PATH)
    {
        DWORD type;
        return (RegQueryValueExA(m_hkey, pszName, NULL, &type, (LPBYTE) pszDst, &cbDst) == ERROR_SUCCESS);
    }

    bool WriteInt(const char* pszKey, int val)
    {
        return (RegSetValueExA(m_hkey, pszKey, 0, REG_DWORD, (PBYTE) &val, sizeof(val)) == ERROR_SUCCESS);
    }
    bool ReadInt(const char* pszKey, int* pval)
    {
        DWORD type;
        DWORD cbDst = sizeof(int);
        return (RegQueryValueExA(m_hkey, pszKey, NULL, &type, (LPBYTE) pval, &cbDst) == ERROR_SUCCESS);
    }

    bool WriteData(const char* pszKey, void* pData, DWORD cbData)
    {
        return (RegSetValueExA(m_hkey, pszKey, 0, REG_BINARY, (PBYTE) pData, cbData) == ERROR_SUCCESS);
    }
    bool ReadData(const char* pszKey, void* pData, DWORD* pcbData)
    {
        DWORD type;
        return (RegQueryValueExA(m_hkey, pszKey, NULL, &type, (LPBYTE) pData, pcbData) == ERROR_SUCCESS);
    }

    bool EnumKey(DWORD item, PSTR pszKey, DWORD cbKey = MAX_PATH)
    {
        return (RegEnumKeyA(m_hkey, item, pszKey, cbKey) == ERROR_SUCCESS);
    }
    bool EnumValue(DWORD item, PSTR pszKey, PSTR pszDst, DWORD cbKey = MAX_PATH, DWORD cbDst = MAX_PATH)
    {
        DWORD type;
        return (RegEnumValueA(m_hkey, item, pszKey, &cbKey, NULL, &type, (PBYTE) pszDst, &cbDst) == ERROR_SUCCESS);
    }

    bool DeleteKey(const char* pszKey) { return (RegDeleteKeyA(m_hkey, pszKey) == ERROR_SUCCESS); }
    bool DeleteValue(const char* pszKey) { return (RegDeleteValueA(m_hkey, pszKey) == ERROR_SUCCESS); }

    bool IsOpen() { return m_hkey != NULL; }

    operator HKEY() const { return m_hkey; }

    HKEY m_hkey;
};

#endif  // defined(_WIN32)
