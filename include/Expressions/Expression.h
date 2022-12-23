#pragma once

#include <memory>

#include "Token/Token.h"

namespace sail
{
    namespace Expressions
    {
        struct Assignment;
        struct Binary;
        struct Grouping;
        struct Literal;
        struct Unary;
        struct Variable;
    }  // namespace Expressions

    using Expression = std::variant<Expressions::Assignment,
                                    Expressions::Binary,
                                    Expressions::Grouping,
                                    Expressions::Literal,
                                    Expressions::Unary,
                                    Expressions::Variable>;

    void printExpression(const Expression& expression);

}  // namespace sail