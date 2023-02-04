#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <sstream>
#include <string>
#include <type_traits>

namespace pt
{
    template<typename T>
    concept parsable = std::convertible_to<T, std::string> || requires(T val, std::stringstream s)
    {
        {s >> val};
    };

    template<parsable T>
    void parse_value(const std::string& arg, T& value)
    {
        // Target value is string, assign directly.
        if constexpr (std::is_same_v<T, std::string>) value = arg;
        // Target value is concertible to string, cast.
        else if constexpr (std::is_convertible_v<std::string, T>)
            value = static_cast<T>(arg);
        // Target value can be parsed from stringstream.
        else
        {
            std::stringstream s(arg);
            s >> value;
        }
    }
}  // namespace pt
