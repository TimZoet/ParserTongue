#include "parsertongue/parser_tongue_exception.h"

namespace pt
{
    parser_tongue_exception::parser_tongue_exception(std::string message) : message(std::move(message)) {}

    char const* parser_tongue_exception::what() const noexcept { return message.c_str(); }
}  // namespace pt
