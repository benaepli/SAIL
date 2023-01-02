#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Unary final : public Expression
    {
        Token op;
        std::shared_ptr<Expression> right;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitUnaryExpression(*this, shared);
        }

        Unary(Token op, std::shared_ptr<Expression> right)
            : op(std::move(op))
            , right(std::move(right))
        {
        }
    };
}  // namespace sail::Expressions
