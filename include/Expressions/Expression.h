#pragma once

#include <memory>

#include "Token/Token.h"

namespace sail
{
    namespace Expressions
    {
        struct Binary;
        struct Grouping;
        struct Literal;
        struct Unary;
    }  // namespace Expressions

    using Expression = std::variant<Expressions::Binary,
                                    Expressions::Grouping,
                                    Expressions::Literal,
                                    Expressions::Unary>;

    void printExpression(const Expression& expression);

}  // namespace sail