#pragma once

#include <variant>

namespace sail
{
    namespace Statements
    {
        struct Block;
        struct Expression;
        struct If;
        struct Print;
        struct Variable;
        struct While;
    }  // namespace Statements

    using Statement = std::variant<Statements::Block,
                                   Statements::Expression,
                                   Statements::If,
                                   Statements::Print,
                                   Statements::Variable,
                                   Statements::While>;
}  // namespace sail
