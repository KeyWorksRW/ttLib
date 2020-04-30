/////////////////////////////////////////////////////////////////////////////
// Name:      ttlib::cmd
// Purpose:   Command line parser
// Author:    Ralph Walden
// Copyright: Copyright (c) 2020 KeyWorks Software (Ralph Walden)
// License:   Apache License (see ../LICENSE)
/////////////////////////////////////////////////////////////////////////////

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
            required = 1 << 0,    // option is required
            needsarg = 1 << 1,    // option is followed by an argument
            shared_val = 1 << 2,  // option sets a specific value

            help = 1 << 31,  // option indicates user is requesting help
        };

        /// result of parsing the command line
        enum class Result : size_t
        {
            success,
            unknown_opt,      // command line contained an argument not specified
            missing,          // a required option did not appear on the command line
            noarg,            // option expected an argument, but no argument was provided on the command line
            invalid_arg,      // expected a string, an option was specified instead (string started with - or /)
        };

        cmd(int argc, char** argv);

        /// Adds an option that isn't followed by an argument. Call isOption(name) to
        /// find out if the option was specified.
        void addOption(std::string_view name, std::string_view description);

        /// Adds an option with with one or more bit-flags set (required, needsrg, shared_val)
        void addOption(std::string_view name, std::string_view description, size_t flags);

        /// If this option appears on the command line, cmd::isHelpRequested() will return
        /// true.
        void addHelpOption(std::string_view name, std::string_view description) { addOption(name, description, cmd::help); }

        /// Call this with flags set to cmd::shared_val and if the option is encountered, it
        /// will set a shared value to setvalue.
        ///
        /// The first option encountered will set the value. If additional options are
        /// encountered, the value is OR'd with the previous value, allowing you to use
        /// options to set bit flags.
        void addOption(std::string_view name, std::string_view description, size_t flags, size_t setvalue);

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
        /// (cmd::needsarg used when the option was added).
        bool isOption(std::string_view option) const;

        /// Call this to get the argument after the option. If the option was added without
        /// cmd::needsarg it will contain "true" if the option was specified on the command line.
        std::optional<ttlib::cstr> getOption(std::string_view option);

        /// Call this to retrieve the value set by any option encountered that was added
        /// with the flag cmd::shared_val.
        ///
        /// Will return tt::npos (-1) if no command line option changed it.
        size_t getSharedValue() const { return m_sharedvalue; }

        /// Call this to get a vector of argmuments that were not associated with an option
        ttlib::cstrVector& getExtras() { return m_extras; }

        /// Call this to get a vector of sorted option names and their descriptions.
        ///
        /// The first entry is the description passed to the constructor. Additional entries
        /// start with four spaces, and there is padding between the option name and it's
        /// descirption so that the descriptions all align.
        std::vector<ttlib::cstr> getUsage();

        /// Get a vector of all arguments, available immediately after the constructor.
        ///
        /// If the constructor was a UNICODE string or array, the arguments will have been
        /// converted to UTF8.
        ttlib::cstrVector& getAllArgs() { return m_originalArgs; }

    private:
        ttlib::cstrVector m_extras;  // arguments specified that were not associated with an option
        std::vector<Result> m_results;
        ttlib::cstrVector m_originalArgs;

        struct Option
        {
        public:
            ttlib::cstr m_description;
            ttlib::cstr m_result;  // for boolean options, this will be "true", "false", or empty() if not encountered

            size_t m_flags;
            size_t m_setvalue;
        };

        std::map<std::string, std::string> m_shortlong;  // maps short name to long name
        std::map<std::string, std::unique_ptr<Option>> m_options;

        size_t m_sharedvalue { tt::npos };

        bool m_HelpRequested { false };

    protected:
        ttlib::cstr shortlong(std::string_view name);
        Option* findOption(std::string_view option) const;
    };
}  // namespace ttlib
