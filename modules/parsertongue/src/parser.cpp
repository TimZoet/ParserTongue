#include "parsertongue/parser.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cctype>
#include <format>

////////////////////////////////////////////////////////////////
// Platform specific includes.
////////////////////////////////////////////////////////////////

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <stringapiset.h>
#include <WinBase.h>
#else
#include <wordexp.h>
#endif

using namespace std::string_literals;

namespace pt
{
    parser::parser(const int argc, char** argv, const bool noProgramName)
    {
        arguments.reserve(noProgramName ? static_cast<size_t>(argc) : static_cast<size_t>(argc) - 1);
        for (size_t i = noProgramName ? 0 : 1; i < static_cast<size_t>(argc); i++) arguments.emplace_back(argv[i]);
    }

    parser::parser(const std::string& args, const bool noProgramName) { reset(args, noProgramName); }

    void parser::set_name(std::string app_name) { name = std::move(app_name); }

    void parser::set_version(std::string app_version) { version = std::move(app_version); }

    void parser::set_description(std::string app_description) { description = std::move(app_description); }

    flag_ptr parser::add_flag(const char short_name, const std::string& long_name)
    {
        if (parsed) throw parser_tongue_exception("Cannot add flag after running the parser"s);

        auto use_short = false;
        auto use_long  = false;

        check_names(short_name, long_name, use_short, use_long);

        // Create and store flag.
        auto ptr = std::make_shared<flag>(short_name, long_name);
        argument_objects.push_back(ptr);
        if (use_short) flags[short_name] = ptr;
        if (use_long) flags_long[long_name] = ptr;

        return ptr;
    }

    const std::vector<std::string>& parser::get_arguments() const noexcept { return arguments; }

    std::string parser::get_full_string() const
    {
        // Length of full string is length of all arguments + #arguments - 1 for the spaces.
        auto length = arguments.size() - 1;
        for (const auto& a : arguments) length += a.size();

        std::string full;
        full.reserve(length);

        // Concatenate all arguments.
        for (size_t i = 0; i < arguments.size(); i++)
        {
            if (i > 0) full.append({' '});
            full.append(arguments[i]);
        }

        return full;
    }

    const std::vector<parse_error_t>& parser::get_errors() const
    {
        if (!parsed) throw parser_tongue_exception("Cannot get errors before running the parser"s);
        return parse_errors;
    }

    bool parser::display_help(std::ostream& out, const size_t name_width, const size_t help_width) const
    {
        if (requested_version)
        {
            if (!name.empty()) out << name << '\n';
            if (!version.empty()) out << version << '\n';
            if (!description.empty()) out << description << '\n';
            return true;
        }

        if (requested_help)
        {
            // User requested help for specific argument.
            if (arguments.size() > 1)
            {
                const auto& arg        = arguments[1];
                auto        short_name = '\0';
                std::string long_name;

                // Determine which argument the user requested help for.
                // Also check for '-', in case he used e.g. -f or --long_name instead of just f or long_name.
                if (arg.size() == 1)
                    short_name = arg[0];
                else if (arg.size() == 2)
                {
                    if (arg[0] == '-')
                        short_name = arg[1];
                    else
                        long_name = arg;
                }
                else if (arg.size() >= 4)
                {
                    if (arg[0] == '-' && arg[1] == '-')
                        long_name = arg.substr(2, arg.size() - 2);
                    else
                        long_name = arg;
                }

                // Look for argument.
                argument_ptr ptr;
                if (short_name != '\0')
                {
                    if (const auto it = flags.find(short_name); it != flags.end()) ptr = it->second;
                    if (const auto it = ptr ? values.end() : values.find(short_name); it != values.end())
                        ptr = it->second;
                    if (const auto it = ptr ? lists.end() : lists.find(short_name); it != lists.end()) ptr = it->second;
                }
                else if (!long_name.empty())
                {
                    if (const auto it = flags_long.find(long_name); it != flags_long.end()) ptr = it->second;
                    if (const auto it = ptr ? values_long.end() : values_long.find(long_name); it != values_long.end())
                        ptr = it->second;
                    if (const auto it = ptr ? lists_long.end() : lists_long.find(long_name); it != lists_long.end())
                        ptr = it->second;
                }

                // Print long help if it is not empty, otherwise print short help.
                if (ptr)
                    out << (ptr->long_help.empty() ? ptr->short_help : ptr->long_help) << '\n';
                else
                    out << "Unknown argument name\n"s;

                // Print required arguments.
                out << "Required arguments:\n"s;
                for (const auto& [a, required] : ptr->relevant_arguments)
                {
                    if (required) out << a->get_pretty_name() << ' ';
                }

                // Print optional arguments.
                out << "\nOptional arguments:\n"s;
                for (const auto& [a, required] : ptr->relevant_arguments)
                {
                    if (!required) out << a->get_pretty_name() << ' ';
                }
            }
            // Print all arguments and their short help strings with some nice indentation.
            else
            {
                const auto total_width = name_width + help_width;

                out << "Available arguments:\n"s;

                for (const auto& arg : argument_objects)
                {
                    size_t col = 0;

                    // Print short name.
                    if (arg->short_name != '\0')
                    {
                        out << '-' << arg->short_name << ' ';
                        col += 3;
                    }

                    // Print long name.
                    if (!arg->long_name.empty())
                    {
                        out << "--"s << arg->long_name << ' ';
                        col += arg->long_name.size() + 3;
                    }

                    // Wrap.
                    if (col >= name_width)
                    {
                        out << '\n';
                        col = 0;
                    }

                    // Print help string.
                    for (const auto c : arg->short_help)
                    {
                        // Indent.
                        while (col < name_width)
                        {
                            out << ' ';
                            col++;
                        }

                        // Wrap.
                        if (col >= total_width && c == ' ')
                        {
                            out << '\n';
                            for (col = 0; col < name_width - 1; col++) out << ' ';
                        }

                        // Write character.
                        out << c;
                        col++;
                    }

                    out << '\n';
                }
            }
            return true;
        }

        return false;
    }

    void parser::display_errors(std::ostream& out) const
    {
        if (!parsed) throw parser_tongue_exception("Cannot display errors before running the parser"s);
        for (const auto& e : parse_errors) out << e;
    }

    const std::vector<std::string>& parser::get_operands() const
    {
        if (!parsed) throw parser_tongue_exception("Cannot get operands before running the parser"s);
        return operands;
    }

    bool parser::operator()(std::string& error)
    {
        if (parsed) throw parser_tongue_exception("Cannot run the parser multiple times"s);
        parsed = true;

        try
        {
            run();
        }
        catch (std::exception& e)
        {
            error = e.what();
            return false;
        }

        return true;
    }

    void parser::reset(const std::string& args, const bool noProgramName)
    {
        parsed = false;
        arguments.clear();
        operands.clear();
        parse_errors.clear();
        requested_version = false;
        requested_help    = false;

        for (const auto& arg : argument_objects) arg->reset();

#ifdef WIN32
        const auto   wchars_num = MultiByteToWideChar(CP_UTF8, 0, args.c_str(), -1, nullptr, 0);
        std::wstring wargs(wchars_num, 0);
        MultiByteToWideChar(CP_UTF8, 0, args.c_str(), -1, wargs.data(), wchars_num);
        int32_t argc = 0;
        auto*   x    = CommandLineToArgvW(wargs.c_str(), &argc);
        arguments.reserve(noProgramName ? static_cast<size_t>(argc) : static_cast<size_t>(argc) - 1);
        for (int32_t i = noProgramName ? 0 : 1; i < argc; i++)
        {
            const auto  num = WideCharToMultiByte(CP_UTF8, 0, x[i], -1, nullptr, 0, nullptr, nullptr);
            std::string y(num - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, x[i], -1, y.data(), num - 1, nullptr, nullptr);
            arguments.emplace_back(std::move(y));
        }

        LocalFree(x);
#else
        wordexp_t words;

        if (wordexp(args.c_str(), &words, 0) == 0)
        {
            for (size_t i = noProgramName ? 0 : 1; i < words.we_wordc; i++)
                arguments.emplace_back(std::string(words.we_wordv[i]));
        }
        else
        {
            wordfree(&words);
            throw parser_tongue_exception("Failed not parse string: \"{}\" "s, args);
        }

        wordfree(&words);
#endif
    }

    void parser::run()
    {
        // Mark all objects as valid.
        for (auto& [k, v] : flags) v->valid = true;
        for (auto& [k, v] : flags_long) v->valid = true;
        for (auto& [k, v] : values) v->valid = true;
        for (auto& [k, v] : values_long) v->valid = true;
        for (auto& [k, v] : lists) { v->valid = true; }
        for (auto& [k, v] : lists_long) { v->valid = true; }

        if (arguments.empty()) return;

        const auto& first_arg = arguments.front();

        if (first_arg == "-v"s || first_arg == "--version"s || first_arg == "version"s)
        {
            requested_version = true;
            return;
        }

        if (first_arg == "-h"s || first_arg == "--help"s || first_arg == "help"s)
        {
            requested_help = true;
            return;
        }

        value_ptr active_value = nullptr;
        list_ptr  active_list  = nullptr;

        for (const auto& arg : arguments)
        {
            auto short_name = false;
            auto long_name  = false;

            // Arguments starting with a single '-' are short names.
            // Arguments starting with a double '--' are long names.
            if (arg[0] == '-')
            {
                active_value.reset();
                active_list.reset();

                if (arg.size() == 1)
                {
                    parse_errors.emplace_back(
                      parse_error::invalid_short_name, arg, "single '-' character without short name"s);
                    continue;
                }

                if (arg[1] == '-')
                {
                    if (arg.size() < 4)
                    {
                        parse_errors.emplace_back(
                          parse_error::invalid_long_name, arg, "long name should be at least 2 characters long"s);
                        continue;
                    }
                    long_name = true;
                }
                else
                    short_name = true;
            }
            // Other arguments are values.
            else
            {
                // Previous argument was a value, try to parse.
                if (active_value)
                {
                    active_value->parse(arg, parse_errors);
                    active_value.reset();
                }
                // Previous argument was a list, try to parse.
                else if (active_list)
                    active_list->parse(arg, parse_errors);
                // Collect operands.
                else
                    operands.push_back(arg);
                continue;
            }

            if (short_name)
                parse_short_name(arg, active_value, active_list);
            else if (long_name)
                parse_long_name(arg, active_value, active_list);
        }
    }

    void parser::check_names(const char         short_name,
                             const std::string& long_name,
                             bool&              use_short_name,
                             bool&              use_long_name) const
    {
        if (short_name != '\0')
        {
            // Verify short name is an alphabetic character.
            if (!std::isalpha(static_cast<unsigned char>(short_name)))
                throw parser_tongue_exception("The short name should be an alphabetic character"s);

            // Verify none of the reserved characters are used.
            if (short_name == 'v' || short_name == 'h')
                throw parser_tongue_exception("The short name should not be one of the reserved characters v and h"s);

            use_short_name = true;
        }

        if (!long_name.empty())
        {
            // Verify length > 1.
            if (long_name.size() == 1)
                throw parser_tongue_exception("The long name should be at least 2 characters long"s);

            // Verify first character is alphabetic.
            if (!std::isalpha(static_cast<unsigned char>(long_name[0])))
                throw parser_tongue_exception("The first character of a long name should be an alphabetic character"s);

            // Verify remaining characters are alphabetic or _.
            if (!std::all_of(long_name.begin() + 1, long_name.end(), [](const char c) {
                    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
                }))
                throw parser_tongue_exception("A long name should consist of alphabetic characters and _"s);

            if (long_name == "version" || long_name == "help")
                throw parser_tongue_exception(
                  "The long name should not be one of the reserved names version and help"s);

            use_long_name = true;
        }

        if (!use_short_name && !use_long_name) throw parser_tongue_exception("Must pass at least one name"s);

        // Check if names are in use.
        if (use_short_name && (flags.contains(short_name) || values.contains(short_name) || lists.contains(short_name)))
            throw parser_tongue_exception("The short name is already in use"s);
        if (use_long_name &&
            (flags_long.contains(long_name) || values_long.contains(long_name) || lists_long.contains(long_name)))
            throw parser_tongue_exception("The long name is already in use"s);
    }

    void parser::parse_short_name(const std::string& arg, value_ptr& active_value, list_ptr& active_list)
    {
        // Argument is just a short name.
        if (arg.size() == 2)
        {
            if (!std::isalpha(static_cast<unsigned char>(arg[1])))
            {
                parse_errors.emplace_back(
                  parse_error::invalid_short_name, arg, "short name should be an alphabetic character"s);
                return;
            }

            // Try to find flag.
            if (const auto it = flags.find(arg[1]); it != flags.end())
            {
                it->second->value = true;
                return;
            }

            // Try to find value.
            if (const auto it = values.find(arg[1]); it != values.end())
            {
                active_value = it->second;
                return;
            }

            // Try to find list.
            if (const auto it = lists.find(arg[1]); it != lists.end())
            {
                active_list = it->second;
                return;
            }

            parse_errors.emplace_back(
              parse_error::unknown_short_name, arg, "unknown short name "s + std::string(1, arg[1]));
        }
        // Argument can be a list of 2 or more flags or a value or list followed directly by its value(s).
        else
        {
            // Argument is a value or list.
            if (const auto equals = arg.find_first_of('='); equals != std::string::npos)
            {
                if (equals == arg.size() - 1)
                {
                    parse_errors.emplace_back(parse_error::missing_value, arg, "missing values after = character"s);
                    return;
                }

                // Name consists of more than 1 character. e.g. -xy=
                if (equals != 2)
                {
                    parse_errors.emplace_back(
                      parse_error::invalid_short_name, arg, "short name should be a single character"s);
                    return;
                }

                // Try to find value.
                if (const auto it = values.find(arg[1]); it != values.end())
                {
                    it->second->parse(arg.substr(3, arg.size() - 3), parse_errors);
                    return;
                }

                // Try to find list.
                if (const auto it = lists.find(arg[1]); it != lists.end())
                {
                    it->second->parse(arg.substr(3, arg.size() - 3), parse_errors);
                    return;
                }

                parse_errors.emplace_back(
                  parse_error::unknown_short_name, arg, "unknown short name "s + std::string(1, arg[1]));
                return;
            }

            // Argument is a list of flags.
            for (size_t i = 1; i < arg.size(); i++)
            {
                if (!std::isalpha(static_cast<unsigned char>(arg[i])))
                {
                    parse_errors.emplace_back(
                      parse_error::invalid_short_name, arg, "short name should be an alphabetic character"s);
                    continue;
                }

                // Enable flag.
                if (auto it = flags.find(arg[i]); it != flags.end())
                {
                    it->second->value = true;
                    continue;
                }

                parse_errors.emplace_back(
                  parse_error::unknown_short_name, arg, "unknown short name "s + std::string(1, arg[i]));
            }
        }
    }

    void parser::parse_long_name(const std::string& arg, value_ptr& active_value, list_ptr& active_list)
    {
        if (!std::isalpha(static_cast<unsigned char>(arg[2])))
        {
            parse_errors.emplace_back(
              parse_error::invalid_long_name, arg, "long name should start with an alphabetic character"s);
            return;
        }

        // Argument is value or list followed directly by its value(s).
        if (const auto equals = arg.find_first_of('='); equals != std::string::npos)
        {
            if (!std::all_of(arg.begin() + 3,
                             arg.begin() + static_cast<std::make_signed_t<size_t>>(equals),
                             [](const char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }))
            {
                parse_errors.emplace_back(parse_error::invalid_long_name,
                                          arg,
                                          "long name should consist of alphabetic and underscore characters"s);
                return;
            }

            if (equals == arg.size() - 1)
            {
                parse_errors.emplace_back(parse_error::missing_value, arg, "missing values after = character"s);
                return;
            }

            const auto long_name = arg.substr(2, equals - 2);

            // Try to find value.
            if (const auto it = values_long.find(long_name); it != values_long.end())
            {
                it->second->parse(arg.substr(equals + 1, arg.size() - equals - 1), parse_errors);
                return;
            }

            // Try to find list.
            if (const auto it = lists_long.find(long_name); it != lists_long.end())
            {
                it->second->parse(arg.substr(equals + 1, arg.size() - equals - 1), parse_errors);
                return;
            }

            parse_errors.emplace_back(parse_error::unknown_long_name, arg, "unknown long name "s + long_name);
        }
        // Argument can be flag, value or list.
        else
        {
            if (!std::all_of(arg.begin() + 3, arg.end(), [](const char c) {
                    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
                }))
            {
                parse_errors.emplace_back(parse_error::invalid_long_name,
                                          arg,
                                          "long name should consist of alphabetic and underscore characters"s);
                return;
            }

            const auto long_name = arg.substr(2, arg.size() - 2);

            // Try to find flag.
            if (const auto it = flags_long.find(long_name); it != flags_long.end())
            {
                it->second->value = true;
                return;
            }

            // Try to find value.
            if (const auto it = values_long.find(long_name); it != values_long.end())
            {
                active_value = it->second;
                return;
            }

            // Try to find list.
            if (const auto it = lists_long.find(long_name); it != lists_long.end())
            {
                active_list = it->second;
                return;
            }

            parse_errors.emplace_back(parse_error::unknown_long_name, arg, "unknown long name "s + long_name);
        }
    }
}  // namespace pt
