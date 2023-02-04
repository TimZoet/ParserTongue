#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <sstream>
#include <vector>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "parsertongue/argument.h"
#include "parsertongue/parsable.h"
#include "parsertongue/parser_tongue_exception.h"
#include "parsertongue/parse_error.h"

namespace pt
{
    class parser;

    class base_list : public argument
    {
    public:
        friend class parser;

        base_list() = delete;

        base_list(const base_list&) = delete;

        base_list(base_list&&) = delete;

        base_list(const char short_name, std::string long_name) : argument(short_name, std::move(long_name)) {}

        virtual ~base_list() = default;

        base_list& operator=(const base_list&) = delete;

        base_list& operator=(base_list&&) = delete;

    protected:
        bool valid = false;

        virtual void parse(const std::string& arg, std::vector<parse_error_t>& parse_errors) noexcept = 0;
    };

    using list_ptr = std::shared_ptr<base_list>;

    template<parsable T>
    class list final : public base_list
    {
    public:
        friend class parser;

        list() = delete;

        list(const list&) = delete;

        list(list&&) = delete;

        list(const char short_name, std::string long_name) : base_list(short_name, std::move(long_name)) {}

        virtual ~list() = default;

        list& operator=(const list&) = delete;

        list& operator=(list&&) = delete;

        /**
         * \brief Check if the list was set. Throws an exception if the parser was not run yet.
         * \return True if the list was set, false otherwise.
         */
        [[nodiscard]] bool is_set() const
        {
            if (!valid) throw parser_tongue_exception("Cannot retrieve value before running the parser");
            return !values.empty();
        }

        /**
         * \brief Get the list of values that was passed to this argument. Throws an exception if the parser was not run yet or no values were set.
         * \return List of values.
         */
        [[nodiscard]] const std::vector<T>& get_values() const
        {
            if (!is_set()) throw parser_tongue_exception("Cannot retrieve value before running the parser");
            return values;
        }

        /**
         * \brief Set the delimiter that is used to split arguments when using = to assign values.
         * \param c Delimiter.
         */
        void set_delimiter(const char c) noexcept { delimiter = c; }

    protected:
        void parse(const std::string& arg, std::vector<parse_error_t>& parse_errors) noexcept override
        {
            try
            {
                std::stringstream stream(arg);
                std::string       str;
                T                 value;
                while (std::getline(stream, str, delimiter))
                {
                    parse_value(str, value);
                    values.push_back(value);
                }
            }
            catch (std::exception& e)
            {
                parse_errors.emplace_back(parse_error::parsing_error, arg, e.what());
            }
        }

    private:
        std::vector<T> values;
        char           delimiter = ',';
    };


}  // namespace pt