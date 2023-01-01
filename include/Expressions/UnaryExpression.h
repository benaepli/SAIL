#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Unary
        : public Expression
        , public std::enable_shared_from_this<Unary>
    {
        Token op;
        std::shared_ptr<Expression> right;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Unary> shared = shared_from_this();
            visitor.visitUnaryExpression(shared);
        }

        Unary(Token op, std::shared_ptr<Expression> right)
            : op(std::move(op))
            , right(std::move(right))
        {
        }
    };
}  // namespace sail::Expressions
