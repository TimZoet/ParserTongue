#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "parsertongue/flag.h"
#include "parsertongue/list.h"
#include "parsertongue/parse_error.h"
#include "parsertongue/value.h"

namespace pt
{
    class parser
    {
    public:
        parser() = delete;

        /**
         * \brief Construct a new parser.
         * \param argc Argument count.
         * \param argv Argument character strings.
         * \param noProgramName If false, the first argument in argv is the program name.
         * If true, the first argument is part of the complete argument string.
         */
        parser(int argc, char** argv, bool noProgramName = false);

        /**
         * \brief Construct a new parser from a std::string. Will perform parsing of the string using the current platform implementation.
         * \param args Argument string.
         * \param noProgramName If false, the first argument in args is the program name.
         * If true, the first argument is part of the complete argument string.
         */
        explicit parser(const std::string& args, bool noProgramName = false);

        parser(const parser&) = delete;

        parser(parser&&) = default;

        ~parser() = default;

        parser& operator=(const parser&) = delete;

        parser& operator=(parser&&) = default;

        /**
         * \brief Set the application name that is displayed when the user requests the version information.
         * \param app_name Application name.
         */
        void set_name(std::string app_name);

        /**
         * \brief Set the application version that is displayed when the user requests the version information.
         * \param app_version Application version.
         */
        void set_version(std::string app_version);

        /**
         * \brief Set the application description that is displayed when the user requests the version information.
         * \param app_description Application description.
         */
        void set_description(std::string app_description);

        /**
         * \brief Add a new flag that can be set by the user with either -f or --long_name.
         * Passing already in use names will result in an exception.
         * \param short_name Optional short name. Must be an alphabetic character. Set to null character to disable.
         * \param long_name Optional long name.
         * Must start with alphabetic character.
         * Remaining characters can be alphabetic or underscores.
         * Length must be at least 2.
         * Leave empty to disable.
         * \return Pointer to flag.
         */
        flag_ptr add_flag(char short_name = '\0', const std::string& long_name = "");

        /**
         * \brief Add a new value that can be set by the user with either -f or --long_name.
         * Passing already in use names will result in an exception.
         * \tparam T Value type.
         * \param short_name Optional short name. Must be an alphabetic character. Set to null character to disable.
         * \param long_name Optional long name.
         * Must start with alphabetic character.
         * Remaining characters can be alphabetic or underscores.
         * Length must be at least 2.
         * Leave empty to disable.
         * \return Pointer to value.
         */
        template<typename T>
        std::shared_ptr<value<T>> add_value(char short_name = '\0', const std::string& long_name = "");

        /**
         * \brief Add a new list that can be set by the user with either -f or --long_name.
         * Passing already in use names will result in an exception.
         * \tparam T Value type.
         * \param short_name Optional short name. Must be an alphabetic character. Set to null character to disable.
         * \param long_name Optional long name.
         * Must start with alphabetic character.
         * Remaining characters can be alphabetic or underscores.
         * Length must be at least 2.
         * Leave empty to disable.
         * \return Pointer to list.
         */
        template<typename T>
        std::shared_ptr<list<T>> add_list(char short_name = '\0', const std::string& long_name = "");

        /**
         * \brief Get the list of arguments that was passed by the user.
         * \return List of arguments.
         */
        [[nodiscard]] const std::vector<std::string>& get_arguments() const noexcept;

        /**
         * \brief Get the full string that was passed by the user.
         * \return String.
         */
        [[nodiscard]] std::string get_full_string() const;

        /**
         * \brief Get the list of all errors that occurred during parsing.
         * \return List of parse errors.
         */
        [[nodiscard]] const std::vector<parse_error_t>& get_errors() const;

        /**
         * \brief Returns whether the help or version info was requested and prints to the ostream.
         * \param out ostream.
         * \param name_width Width of the name column.
         * \param help_width Width of the help string column.
         * \return True if help was requested.
         */
        [[nodiscard]] bool display_help(std::ostream& out, size_t name_width = 20, size_t help_width = 60) const;

        /**
         * \brief Print all parsing errors to the ostream.
         * \param out ostream.
         */
        void display_errors(std::ostream& out) const;

        /**
         * \brief Get the list of all operands (values not belonging to an argument) that were passed by the user.
         * \return List of strings.
         */
        [[nodiscard]] const std::vector<std::string>& get_operands() const;

        /**
         * \brief Run the parser.
         * \param error Error string that is set when return value is false.
         * \return False if there were internal errors and parsing failed.
         */
        bool operator()(std::string& error);

        /**
         * \brief Reset the parser with a new string. Will perform parsing of the string using the current platform implementation. All arguments are reset as well. Parser must be run again.
         * \param args Argument string.
         * \param noProgramName If false, the first argument in args is the program name.
         * If true, the first argument is part of the complete argument string.
         */
        void reset(const std::string& args, bool noProgramName);

    private:
        void run();

        void
          check_names(char short_name, const std::string& long_name, bool& use_short_name, bool& use_long_name) const;

        void parse_short_name(const std::string& arg, value_ptr& active_value, list_ptr& active_list);

        void parse_long_name(const std::string& arg, value_ptr& active_value, list_ptr& active_list);

        bool                                       parsed = false;
        std::string                                name;
        std::string                                version;
        std::string                                description;
        std::vector<std::string>                   arguments;
        std::vector<argument_ptr>                  argument_objects;
        std::unordered_map<char, flag_ptr>         flags;
        std::unordered_map<std::string, flag_ptr>  flags_long;
        std::unordered_map<char, value_ptr>        values;
        std::unordered_map<std::string, value_ptr> values_long;
        std::unordered_map<char, list_ptr>         lists;
        std::unordered_map<std::string, list_ptr>  lists_long;
        std::vector<std::string>                   operands;
        std::vector<parse_error_t>                 parse_errors;
        bool                                       requested_version = false;
        bool                                       requested_help    = false;
    };

    template<typename T>
    std::shared_ptr<value<T>> parser::add_value(const char short_name, const std::string& long_name)
    {
        if (parsed) throw parser_tongue_exception("Cannot add value after running the parser");

        auto use_short = false;
        auto use_long  = false;

        check_names(short_name, long_name, use_short, use_long);

        // Create and store value.
        auto ptr = std::make_shared<value<T>>(short_name, long_name);
        argument_objects.push_back(ptr);
        if (use_short) values[short_name] = ptr;
        if (use_long) values_long[long_name] = ptr;

        return ptr;
    }

    template<typename T>
    std::shared_ptr<list<T>> parser::add_list(const char short_name, const std::string& long_name)
    {
        if (parsed) throw parser_tongue_exception("Cannot add list after running the parser");

        auto use_short = false;
        auto use_long  = false;

        check_names(short_name, long_name, use_short, use_long);

        // Create and store value.
        auto ptr = std::make_shared<list<T>>(short_name, long_name);
        argument_objects.push_back(ptr);
        if (use_short) lists[short_name] = ptr;
        if (use_long) lists_long[long_name] = ptr;

        return ptr;
    }
}  // namespace pt
