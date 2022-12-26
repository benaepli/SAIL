#pragma once

#include <variant>

namespace sail
{
    namespace Statements
    {
        struct Block;
        struct Expression;
        struct Function;
        struct If;
        struct Return;
        struct Variable;
        struct While;
    }  // namespace Statements

    using Statement = std::variant<Statements::Block,
                                   Statements::Expression,
                                   Statements::Function,
                                   Statements::If,
                                   Statements::Variable,
                                   Statements::Return,
                                   Statements::While>;
}  // namespace sail
