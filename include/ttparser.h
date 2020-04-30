/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::cmd
// Purpose:   Command line parser
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

// This code is loosely based on cl::Cmdline which is Copyright (c) 2019 Alexander Bolz and also released under
// an Apache Liecense. The original work is at https://github.com/abolz/CmdLine2

#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#if defined(_WIN32)
    #include <ttdebug.h>
#else
    #include <cassert>
#endif

#include <ttcstr.h>      // cstr -- Classes for handling zero-terminated char strings.
#include <ttcvector.h>   // cstrVector -- Vector of ttlib::cstr strings
#include <ttlibspace.h>  // Contains the ttlib namespace functions/declarations common to all ttLib libraries

namespace ttlib
{
    class cmd
    {
    public:
        enum : size_t
        {
            required = 1 << 0,  // option is required
            needsarg = 1 << 1,  // option is followed by an argument
            help = 1 << 2,      // option indicates user is requesting help
        };

        /// result of parsing the command line
        enum class Result : size_t
        {
            success,
            unknown_opt,  // command line contained an argument not specified
            missing,      // a required option did not appear on the command line
            noarg,        // option expected an argument, but no argument was provided on the command line
            invalid_arg,  // expected a string, an option was specified instead (string started with - or /)
        };

        cmd(int argc, char** argv, std::string_view description);

        void addOption(std::string_view name, std::string_view description, size_t flags = 0);

        /// Call this to parse whatever command line was passed to the constructor
        ///
        /// If this returns false, then call getResults() to get a vector of all the errors
        /// that occurred.
        bool parse();

        /// If true, it means a -? option was encountered, or an option with the cmd::help
        /// flag was encountered
        bool isHelpRequested() const { return m_HelpRequested; }

        /// For every error that occurred, a cmd::Result:: value will be added to this vector
        const std::vector<Result>& getResults() const { return m_results; }

        /// Returns true if the option appeared on the command line.
        ///
        /// Use getOption() if you need to retrieve an argument associated with the option
        /// (cmd::arg used when the option was added).
        bool isOption(std::string_view option) const;

        /// Call this to get the argument after the option. If the option was added without
        /// cmd::arg it will contain "true" if the option was specified on the command line.
        std::optional<ttlib::cstr> getOption(std::string_view option);

        /// Call this to get a vector of argmuments that were not associated with an option
        ttlib::cstrVector& getExtras() { return m_extras; }

        /// Call this to get a vector of sorted option names and their descriptions.
        ///
        /// The first entry is the description passed to the constructor. Additional entries
        /// start with four spaces, and there is padding between the option name and it's
        /// descirption so that the descriptions all align.
        std::vector<ttlib::cstr> getUsage();

    private:
        ttlib::cstrVector m_extras;  // arguments specified that were not associated with an option
        ttlib::cstr m_description;
        std::vector<Result> m_results;
        ttlib::cstrVector m_originalArgs;

        struct Option
        {
        public:
            ttlib::cstr m_description;
            ttlib::cstr m_result;  // for boolean options, this will be "true", "false", or empty() if not encountered

            size_t m_flags;
        };

        std::map<std::string, std::string> m_shortlong;  // maps short name to long name
        std::map<std::string, std::unique_ptr<Option>> m_options;

        bool m_HelpRequested { false };

    protected:
        ttlib::cstr shortlong(std::string_view name);
        Option* findOption(std::string_view option) const;
    };
}  // namespace ttlib
