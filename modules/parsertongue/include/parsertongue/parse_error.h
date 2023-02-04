#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cstdint>
#include <ostream>
#include <string>
#include <tuple>

namespace pt
{
    enum class parse_error : uint32_t
    {
        invalid_short_name,
        invalid_long_name,
        unknown_short_name,
        unknown_long_name,
        missing_value,
        parsing_error
    };

    /**
     * \brief (error type, argument string, error message)
     */
    using parse_error_t = std::tuple<parse_error, std::string, std::string>;

    std::ostream& operator<<(std::ostream& out, const parse_error_t& e);
}