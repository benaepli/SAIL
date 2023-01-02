#pragma once

#include <memory>

#include "Expression.h"

namespace sail::Expressions
{
    struct Binary final : public Expression
    {
        std::shared_ptr<Expression> left;
        Token op;
        std::shared_ptr<Expression> right;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitBinaryExpression(*this, shared);
        }

        Binary(std::shared_ptr<Expression> left, Token op, std::shared_ptr<Expression> right)
            : left(std::move(left))
            , op(std::move(op))
            , right(std::move(right))
        {
        }
    };

}  // namespace sail::Expressions
