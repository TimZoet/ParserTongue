#include "parsertongue/parse_error.h"

using namespace std::string_literals;

namespace pt
{
    std::ostream& operator<<(std::ostream& out, const parse_error_t& e)
    {
        out << "A parse error occurred:\n  "s;

        switch (std::get<0>(e))
        {
        case parse_error::invalid_short_name: out << "invalid_short_name"s; break;
        case parse_error::invalid_long_name: out << "invalid_long_name"s; break;
        case parse_error::unknown_short_name: out << "unknown_short_name"s; break;
        case parse_error::unknown_long_name: out << "unknown_long_name"s; break;
        case parse_error::missing_value: out << "missing_value"s; break;
        case parse_error::parsing_error: out << "parsing_error"s; break;
        }

        out << ": "s << std::get<2>(e) << '\n';

        out << "  while parsing \""s << std::get<1>(e) << "\"\n"s;
        return out;
    }
}  // namespace pt
