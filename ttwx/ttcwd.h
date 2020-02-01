/////////////////////////////////////////////////////////////////////////////
// Name:      ttcwd.h
// Purpose:   Class for saving, setting, and restoring current directory
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

/// @file
/// The current directory is saved when the class is constructed, and restored when the
/// class is destroyed.
///
/// SetCwd() can be called to set the current directory without affecting the restored
/// directory. Call assign() if you want to change the directory to restore.

#pragma once

#include <wx/filefn.h>

#include "ttstring.h"

class ttCwd : public ttString
{
public:
    ttCwd() { assignCwd(); }
    ~ttCwd()
    {
        std::filesystem::path cwd(c_str());
        std::filesystem::current_path(cwd);
    }

    /// You currently can't call wxSetWorkingDirectory() with a string_view, so this handles converting
    /// the view into a null-terminated string that wxSetWorkingDirectory requires.
    static bool SetCwd(std::string_view dir)
    {
        std::filesystem::path newpath(dir);

#if defined(_WIN32)
        return wxSetWorkingDirectory(newpath.wstring());
#else
        return wxSetWorkingDirectory(newpath.string());
#endif  // _WIN32
    }
};
