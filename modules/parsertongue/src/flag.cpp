#include "parsertongue/flag.h"

#include "parsertongue/parser_tongue_exception.h"

using namespace std::string_literals;

namespace pt
{
    flag::flag(const char short_name, std::string long_name) : argument(short_name, std::move(long_name)) {}

    bool flag::is_set() const
    {
        if (!valid) throw parser_tongue_exception("Cannot retrieve value before running the parser"s);
        return value;
    }
}  // namespace pt