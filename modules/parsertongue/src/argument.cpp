#include "parsertongue/argument.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

namespace pt
{
    argument::argument(const char name_short, std::string name_long) :
        short_name(name_short), long_name(std::move(name_long))
    {
    }

    void argument::set_help(std::string help_short, std::string help_long)
    {
        short_help = std::move(help_short);
        long_help  = std::move(help_long);
    }

    void argument::add_relevant_argument(argument& arg, bool required)
    {
        relevant_arguments.emplace_back(&arg, required);
    }

    std::string argument::get_pretty_name() const
    {
        return std::format("[{0}, {1}]", short_name == '\0' ? '_' : short_name, long_name.empty() ? "_" : long_name);
    }

}  // namespace pt
