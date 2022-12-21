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

namespace sail::Expressions
{

    struct Binary
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        Token oper;
    };

    struct Grouping
    {
        std::unique_ptr<Expression> expression;
    };

    struct Literal
    {
        LiteralType literal;
    };

    struct Unary
    {
        std::unique_ptr<Expression> right;
        Token oper;
    };
}  // namespace sail::Expressions
