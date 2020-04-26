/////////////////////////////////////////////////////////////////////////////
// Name:      include\ttcmap.h
// Purpose:   std::map<template, ttlib::cstr>
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

/**

    This header-only class is used to create a std::map that uses ttlib::cstr for the value.

    Sample usage:

    cmap<int> test;
    test.insert({ 1, "foo" });

    // The traditional lookup method

    if (auto found = test.find(1); found != test.end())
        std::cout << found->second << '\n';

    // Using std::optional

    if (auto found = test.viewValue(1); found)
        std::cout << found.value() << '\n';

    // Using structured binding

    if (auto [found, value] = test.getValue(1); found)
        std::cout << value << '\n';

**/

#include <map>
#include <optional>
#include <utility>

#include "ttcstr.h"      // cstr -- Classes for handling zero-terminated char strings.
#include "ttcview.h"     // cview -- string_view functionality on a zero-terminated char string.
#include "ttlibspace.h"  // Contains the ttlib namespace functions/declarations common to all ttLib libraries

namespace ttlib
{
    template<typename T>
    class cmap : public std::map<T, ttlib::cstr>
    {
    public:
        std::pair<bool, const ttlib::cstr&> getValue(T key) const
        {
            if (auto found = find(key); found != end())
                return { true, found->second };
            else
                return { false, ttlib::emptystring };
        }

        std::optional<ttlib::cview> viewValue(T key) const
        {
            if (auto found = find(key); found != end())
                return { found->second.subview() };
            else
                return {};
        }
    };
};  // namespace ttlib
