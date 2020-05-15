/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::openfile
// Purpose:   Wrapper around Windows GetOpenFileName() API
// Author:    Ralph Walden
// Copyright: Copyright (c) 2002-2020 KeyWorks Software (Ralph Walden)
// License:   MIT License (see %lic_name%)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(_WIN32)
    #error "This header file can only be used when compiling for Windows"
#endif

#if !(__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
    #error "The contents of <ttopenfile.h> are available only with C++17 or later."
#endif

#include <vector>

#include <commdlg.h>

#include "ttcstr.h"      // cstr -- Classes for handling zero-terminated char strings.
#include "ttdebug.h"     // for ttASSERTS
#include "ttmultibtn.h"  // ttlib::MultiBtn

#ifndef OFN_DONTADDTORECENT
    #define OFN_DONTADDTORECENT 0x02000000
#endif

namespace ttlib
{
    UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uMsg, WPARAM /* wParam */, LPARAM lParam);

    class openfile : public OPENFILENAMEW
    {
    public:
        openfile(HWND hwndParent = NULL);

        /// call this to launch the Windows OpenFile dialog box
        bool GetOpenName();
        /// call this to launch the Windows SaveFile dialog box
        bool GetSaveName();

        ttlib::cstr& filename() { return m_filename; }

        const char* c_str() const noexcept { return m_filename.c_str(); }
        operator const char*() const noexcept { return m_filename.c_str(); }
        operator std::string_view() const noexcept { return m_filename.subview(0, m_filename.length()); }

        /// Separate filters names with a '|' character as in "My Files|*.cpp;*.h|Your Files|*.c"
        void SetFilter(std::string_view filters);

        /// Separate filters names with a '|' character as in "My Files|*.cpp;*.h|Your Files|*.c"
        void SetFilter(WORD idResource) { SetFilter(LoadStringEx(idResource)); };

        void SetInitialDir(std::string_view dir);
        void SetInitialFileName(std::string_view filename);

        /// Call this if you want 3D shaded buttons with an Icon on the Open/Cancel buttons. Note that
        /// the dialog template used will be different as well (uses an older dialog style).
        void EnableShadeBtns(bool Enable = true);

        void SetOpenIcon(UINT idIcon) { m_idOpenIcon = idIcon; }
        void SetCancelIcon(UINT idIcon) { m_idCancelIcon = idIcon; }

        // The following flags are set in the constructor. You can either modify the Flags structure member
        // or you can modify them using the functions below.
        //
        // Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT;

        /// Default behaviour is NOT to add filename to Windows recent filenames, calling
        /// this will add it.
        void AddToRecent() { Flags &= ~OFN_DONTADDTORECENT; }

        void ShowCreatePrompt()
        {
            Flags &= ~OFN_FILEMUSTEXIST;
            Flags |= OFN_CREATEPROMPT;
        }

        void ShowReadOnlyBox() { Flags &= ~OFN_HIDEREADONLY; }

        void RestoreDirectory() { Flags |= OFN_NOCHANGEDIR; }

        // The class constructor sets this to OFN_FILEMUSTEXIST
        void SetFileMustExist(bool MustExist = true)
        {
            if (MustExist)
                Flags |= OFN_FILEMUSTEXIST;
            else
                Flags &= ~OFN_FILEMUSTEXIST;
        }

    private:
        friend UINT_PTR CALLBACK ttlib::OFNHookProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

        // Windows both reads and writes to the filename buffer, so we use a vector to store the buffer,
        // and ttlib::utf8to16()/ttlib::utf16to8() to convert to/from UTF8/UTF16 strings.
        std::unique_ptr<wchar_t []> m_filename16;

        std::wstring m_initialDir16;
        std::wstring m_filters16;

        ttlib::cstr m_filename;

        ttlib::MultiBtn m_ShadedBtns;
        UINT m_idOpenIcon { static_cast<UINT>(-1) };
        UINT m_idCancelIcon { static_cast<UINT>(-1) };
        bool m_ShadeBtns { false };
    };

}  // namespace ttlib
