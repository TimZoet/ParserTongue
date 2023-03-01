#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace pt
{
    class parser;

    class argument
    {
    public:
        friend class parser;

        argument() = delete;

        argument(const argument&) = delete;

        argument(argument&&) = delete;

        argument(char name_short, std::string name_long);

        virtual ~argument() = default;

        argument& operator=(const argument&) = delete;

        argument& operator=(argument&&) = delete;

        /**
         * \brief Set the short and long help string.
         * \param help_short Short help string that is displayed when the user
         * requests the help.
         * \param help_long Optional long help string that is displayed when
         * the user requests help for this specific argument. If no long help
         * is set, the short help is displayed instead.
         */
        void set_help(std::string help_short, std::string help_long = "");

        void add_relevant_argument(argument& arg, bool required);

        virtual void reset() = 0;

    protected:
        [[nodiscard]] std::string get_pretty_name() const;

        char        short_name = '\0';
        std::string long_name;
        std::string short_help;
        std::string long_help;

        std::vector<std::pair<argument*, bool>> relevant_arguments;
    };

    using argument_ptr = std::shared_ptr<argument>;
}  // namespace pt
