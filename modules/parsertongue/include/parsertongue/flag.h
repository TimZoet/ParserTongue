#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "parsertongue/argument.h"

namespace pt
{
    class parser;

    class flag final : public argument
    {
    public:
        friend class parser;

        flag() = delete;

        flag(const flag&) = delete;

        flag(flag&&) = delete;

        flag(char short_name, std::string long_name);

        ~flag() noexcept override = default;

        flag& operator=(const flag&) = delete;

        flag& operator=(flag&&) = delete;

        /**
         * \brief Check if the flag was set. Throws an exception if the parser was not run yet.
         * \return True if the flag was set, false otherwise.
         */
        [[nodiscard]] bool is_set() const;

        void reset() override;

    private:
        bool valid = false;
        bool value = false;
    };

    using flag_ptr = std::shared_ptr<flag>;
}  // namespace pt
