#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <memory>
#include <optional>
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

    class base_value : public argument
    {
    public:
        friend class parser;

        base_value() = delete;

        base_value(const base_value&) = delete;

        base_value(base_value&&) = delete;

        base_value(const char short_name, std::string long_name) : argument(short_name, std::move(long_name)) {}

        ~base_value() noexcept override = default;

        base_value& operator=(const base_value&) = delete;

        base_value& operator=(base_value&&) = delete;

        void reset() override { valid = false; }

    protected:
        bool valid = false;

        virtual void parse(const std::string& arg, std::vector<parse_error_t>& parse_errors) noexcept = 0;
    };

    using value_ptr = std::shared_ptr<base_value>;

    template<parsable T>
    class value final : public base_value
    {
    public:
        value() = delete;

        value(const value&) = delete;

        value(value&&) = delete;

        value(const char short_name, std::string long_name) : base_value(short_name, std::move(long_name)) {}

        ~value() noexcept override = default;

        value& operator=(const value&) = delete;

        value& operator=(value&&) = delete;

        /**
         * \brief Check if the value was set. Throws an exception if the parser was not run yet.
         * \return True if the value was set, false otherwise.
         */
        [[nodiscard]] bool is_set() const
        {
            if (!valid) throw parser_tongue_exception("Cannot retrieve value before running the parser");
            return v || default_value;
        }

        /**
         * \brief Get the value that was passed to this argument. Throws an exception if the parser was not run yet, or the value was not set and there is no default value.
         * \return Value.
         */
        [[nodiscard]] const T& get_value() const
        {
            if (!valid) throw parser_tongue_exception("Cannot retrieve value before running the parser");
            if (!v)
            {
                if (!default_value) throw parser_tongue_exception(std::format("{0} was not set", get_pretty_name()));
                return *default_value;
            }
            return *v;
        }

        /**
         * \brief Set a default value that is returned by get_value when the user did not pass any value.
         * \param value Default value.
         */
        void set_default(const T& value) noexcept { default_value = value; }

        /**
         * \brief Limit the number of allowed values to all options that are added through this method.
         * \param value Value to add.
         */
        void add_option(T value) { options.emplace_back(std::move(value)); }

        /**
         * \brief Limit the number of allowed values to all options that are added through this method.
         * \tparam Ts T.
         * \param values Values to add.
         */
        template<std::same_as<T>... Ts>
        void add_options(Ts... values)
        {
            (add_option(std::move(values)), ...);
        }

        void reset() override
        {
            base_value::reset();
            v.reset();
        }

    protected:
        void parse(const std::string& arg, std::vector<parse_error_t>& parse_errors) noexcept override
        {
            try
            {
                T val;
                parse_value(arg, val);

                // If there is a limited number of allowed options, check if the passed value is valid.
                if (!options.empty())
                {
                    if (std::find(options.cbegin(), options.cend(), val) == options.end())
                        throw parser_tongue_exception(
                          std::format("{0} is not a valid option for {1}", arg, get_pretty_name()));
                }

                v = val;
            }
            catch (std::exception& e)
            {
                parse_errors.emplace_back(parse_error::parsing_error, arg, e.what());
            }
        }

    private:
        std::optional<T> v;
        std::optional<T> default_value;
        std::vector<T>   options;
    };
}  // namespace pt