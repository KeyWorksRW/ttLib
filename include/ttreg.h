/////////////////////////////////////////////////////////////////////////////
// Name:		ttRegistry class
// Purpose:		Header-only class for working with the Windows registry
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TTLIB_CREG_H__
#define __TTLIB_CREG_H__

#ifndef _WINDOWS_
	#error This code will only work on Windows
#endif

// Header-only class

class ttRegistry
{
public:
	ttRegistry() { m_hkey = NULL; }
	ttRegistry(PCSTR pszKey, bool fWrite = true) {
		if (RegOpenKeyEx(HKEY_CURRENT_USER, pszKey, 0, fWrite ? KEY_ALL_ACCESS : KEY_READ, &m_hkey) != ERROR_SUCCESS)
			m_hkey = NULL;
	}
	~ttRegistry() { if (m_hkey) RegCloseKey(m_hkey); }

	bool Open(HKEY hkeyBase, PCSTR pszKey, bool fWrite = true) { if (m_hkey) RegCloseKey(m_hkey); return (RegOpenKeyEx(hkeyBase, pszKey, 0, fWrite ? KEY_ALL_ACCESS : KEY_READ, &m_hkey) == ERROR_SUCCESS); }
	bool Create(HKEY hkeyBase, PCSTR pszKey) { if (m_hkey) RegCloseKey(m_hkey); return (RegCreateKeyEx(hkeyBase, pszKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,	NULL, &m_hkey, NULL) == ERROR_SUCCESS); }
	void Close() { if (m_hkey) RegCloseKey(m_hkey), m_hkey = NULL; }

	bool WriteString(PCSTR pszKey, PCSTR pszValue) { return (RegSetValueEx(m_hkey, pszKey, 0, REG_SZ, (PBYTE) pszValue, (DWORD) strlen(pszValue) + 1) == ERROR_SUCCESS); }
	bool ReadString(PCSTR pszName, PSTR pszDst, DWORD cbDst = MAX_PATH) { DWORD type; return (RegQueryValueEx(m_hkey, pszName, NULL, &type, (LPBYTE) pszDst, &cbDst) == ERROR_SUCCESS); }

	bool WriteInt(PCSTR pszKey, int val) { return (RegSetValueEx(m_hkey, pszKey, 0, REG_DWORD, (PBYTE) &val, sizeof(val)) == ERROR_SUCCESS); }
	bool ReadInt(PCSTR pszKey, int* pval) { DWORD type; DWORD cbDst = sizeof(int); return (RegQueryValueEx(m_hkey, pszKey, NULL, &type, (LPBYTE) pval, &cbDst) == ERROR_SUCCESS); }

	bool WriteData(PCSTR pszKey, void* pData, DWORD cbData) { return (RegSetValueEx(m_hkey, pszKey, 0, REG_BINARY, (PBYTE) pData, cbData) == ERROR_SUCCESS); }
	bool ReadData(PCSTR pszKey, void* pData, DWORD* pcbData) { DWORD type; return (RegQueryValueEx(m_hkey, pszKey, NULL, &type, (LPBYTE) pData, pcbData) == ERROR_SUCCESS); }

	bool EnumKey(DWORD item, PSTR pszKey, DWORD cbKey = MAX_PATH) { return (RegEnumKey(m_hkey, item, pszKey, cbKey) == ERROR_SUCCESS); }
	bool EnumValue(DWORD item, PSTR pszKey, PSTR pszDst, DWORD cbKey = MAX_PATH, DWORD cbDst = MAX_PATH) { DWORD type; return (RegEnumValue(m_hkey, item, pszKey, &cbKey, NULL, &type, (PBYTE) pszDst, &cbDst) == ERROR_SUCCESS); }

	bool DeleteKey(PCSTR pszKey) { return (RegDeleteKey(m_hkey, pszKey) == ERROR_SUCCESS); }
	bool DeleteValue(PCSTR pszKey) { return (RegDeleteValue(m_hkey, pszKey) == ERROR_SUCCESS); }

	bool isOpen() { return m_hkey != NULL; }

	operator HKEY() const { return m_hkey; }

	HKEY m_hkey;
};

#endif	// __TTLIB_CREG_H__