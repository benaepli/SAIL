#pragma once

#include <variant>

namespace sail
{
    namespace Statements
    {
        struct Expression;
        struct Variable;
    }  // namespace Statements

    using Statement =
        std::variant<Statements::Expression, Statements::Variable>;
}  // namespace sail
