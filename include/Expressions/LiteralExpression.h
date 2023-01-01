#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Literal
        : public Expression
        , public std::enable_shared_from_this<Literal>
    {
        LiteralType literal;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Literal> shared = shared_from_this();
            visitor.visitLiteralExpression(shared);
        }

        explicit Literal(LiteralType literal)
            : literal(std::move(literal))
        {
        }
    };
}  // namespace sail::Expressions
