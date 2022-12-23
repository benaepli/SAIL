#pragma once

#include <variant>

namespace sail
{
    namespace Statements
    {
        struct Block;
        struct Expression;
        struct Print;
        struct Variable;
    }  // namespace Statements

    using Statement = std::variant<Statements::Block,
                                   Statements::Expression,
                                   Statements::Print,
                                   Statements::Variable>;
}  // namespace sail
