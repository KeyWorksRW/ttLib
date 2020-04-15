/////////////////////////////////////////////////////////////////////////////
// Name:      ttCRegistry class
// Purpose:   Class for working with the Windows registry
// Author:    Ralph Walden
// Copyright: Copyright (c) 1998-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#include <winerror.h>
#include <winreg.h>

#include <string>

#include "ttenumstr.h"  // enumstr -- Enumerate through substrings in a string

namespace ttlib
{
    class registry
    {
    public:
        registry() = default;
        ~registry()
        {
            if (m_hkey)
                RegCloseKey(m_hkey);
        }

        /// If a key was already open, this will automatically close it first.
        bool Open(std::string_view name, REGSAM access = KEY_READ, HKEY hkey = HKEY_CURRENT_USER);

        /// If a key was already open, this will automatically close it first.
        bool OpenLocal(std::string_view name, REGSAM access = KEY_READ)
        {
            return Open(name, access, HKEY_LOCAL_MACHINE);
        }

        /// If a key was already open, this will automatically close it first.
        bool OpenClasses(std::string_view name, REGSAM access = KEY_READ)
        {
            return Open(name, access, HKEY_CLASSES_ROOT);
        }

        /// Use this call if you need to write to the regisry.
        ///
        /// If a key was already open, this will automatically close it first.
        bool OpenAllAccess(std::string_view name, HKEY hkey = HKEY_CURRENT_USER)
        {
            return Open(name, KEY_ALL_ACCESS, hkey);
        }

        /// If a key was already open, this will automatically close it first.
        bool Create(std::string_view name, HKEY hkey = HKEY_CURRENT_USER);

        bool isopen() { return m_hkey != NULL; }

        /// Read a REG_SZ or REG_EXPAND_SZ type of resource.
        ///
        /// Returned string will be empty if an error occurred.
        std::string ReadString(std::string_view name);

        /// You must first call OpenAllAccess(), or specify KEY_ALL_ACCESS in a call to Open().
        ///
        /// value is assumed to be a UTF8 string
        bool WriteString(std::string_view name, std::string_view value);

        // The following can be used to read/write vectors of strings instead of using REG_MULTI_SZ

        ttlib::enumstr ReadEnumstr(std::string_view name);
        bool WriteEnumstr(std::string_view name, const ttlib::enumstr& estr);

        /// This will read a REG_QWORD value in 64-bit builds, and REG_DWORD in 32-bit builds
        ///
        /// Will return tt::npos (-1) if an error occurred
        size_t ReadValue(std::string_view name);

        /// This will write a REG_QWORD value in 64-bit builds, and REG_DWORD in 32-bit builds
        bool WriteValue(std::string_view name, size_t value);

        operator HKEY() const { return m_hkey; }

    private:
        HKEY m_hkey { NULL };
    };
}  // namespace ttlib
