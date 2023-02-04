#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <exception>
#include <string>

namespace pt
{
    class parser_tongue_exception final : public std::exception
    {
    public:
        explicit parser_tongue_exception(std::string message);

        [[nodiscard]] char const* what() const noexcept override;
    private:
        std::string message;
    };
}