/////////////////////////////////////////////////////////////////////////////
// Name:      ttassert.h
// Purpose:   Provides an alternative to wxASSERT macros
// Author:    Ralph Walden
// Copyright: Copyright (c) 2019-2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

/**
 * @file
 * The main difference between these and wxASSERT macros is that this one will display the last system error (if
 * any) rather than a stack trace, and on Windows it will not offer to break into the debugger if you are not
 * actually running a debugger.
 */

/// In non-debug builds this will simply return.
void ttAssert(std::string_view file, int line, std::string_view func, std::string_view cond,
              std::string_view msg = "");

#if !defined(NDEBUG)  // Starts debug section.

namespace ttdbg
{
    extern bool bNoAssert;  /// Setting this to true will cause AssertionMsg to return without doing anything
}

    #define ttDISABLE_ASSERTS (ttdbg::bNoAssert = true)
    #define ttENABLE_ASSERTS  (ttdbg::bNoAssert = false)

    #define ttASSERT(cond)                                                            \
        wxSTATEMENT_MACRO_BEGIN                                                       \
            if (cond)                                                                 \
            {                                                                         \
            }                                                                         \
            else if ((ttAssert(__FILE__, __LINE__, __func__, #cond), wxTrapInAssert)) \
            {                                                                         \
                wxTrapInAssert = false;                                               \
                wxTrap();                                                             \
            }                                                                         \
        wxSTATEMENT_MACRO_END

    #define ttASSERT_MSG(cond, pszMsg)                                                        \
        wxSTATEMENT_MACRO_BEGIN                                                               \
            if (cond)                                                                         \
            {                                                                                 \
            }                                                                                 \
            else if ((ttAssert(__FILE__, __LINE__, __func__, #cond, pszMsg), wxTrapInAssert)) \
            {                                                                                 \
                wxTrapInAssert = false;                                                       \
                wxTrap();                                                                     \
            }                                                                                 \
        wxSTATEMENT_MACRO_END

    #define ttFAIL_MSG(pszMsg)                                                    \
        wxSTATEMENT_MACRO_BEGIN                                                   \
            if ((ttAssert(__FILE__, __LINE__, __func__, pszMsg), wxTrapInAssert)) \
            {                                                                     \
                wxTrapInAssert = false;                                           \
                wxTrap();                                                         \
            }                                                                     \
        wxSTATEMENT_MACRO_END

    /// This still executes the condition in non-DEBUG build, it just doesn't check the result.
    #define ttVERIFY(cond)                                                            \
        wxSTATEMENT_MACRO_BEGIN                                                       \
            if (cond)                                                                 \
            {                                                                         \
            }                                                                         \
            else if ((ttAssert(__FILE__, __LINE__, __func__, #cond), wxTrapInAssert)) \
            {                                                                         \
                wxTrapInAssert = false;                                               \
                wxTrap();                                                             \
            }                                                                         \
        wxSTATEMENT_MACRO_END

    /// In _DEBUG builds this will display an assertion dialog first,
    /// then it will throw an excpetion. In Release builds, only the
    /// exception is thrown.
    #define ttTHROW(msg)                                                       \
        wxSTATEMENT_MACRO_BEGIN                                                \
            if ((ttAssert(__FILE__, __LINE__, __func__, msg), wxTrapInAssert)) \
            {                                                                  \
                wxTrapInAssert = false;                                        \
                wxTrap();                                                      \
            }                                                                  \
            throw msg;                                                         \
        wxSTATEMENT_MACRO_END

#else  // Starts non-Debug section.

    #define ttDISABLE_ASSERTS
    #define ttENABLE_ASSERTS

    #define ttASSERT(cond)
    #define ttASSERT_MSG(exp, pszMsg)
    #define ttFAIL_MSG(pszMsg)

    #define ttVERIFY(cond) ((void) (cond))

    #define ttTHROW(msg) \
        {                \
            throw msg;   \
        }

#endif  // End Debug/non-Debug sections.
