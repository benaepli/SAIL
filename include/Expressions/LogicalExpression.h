#pragma once

#include "Expression.h"
#include "Token/Token.h"

namespace sail::Expressions
{
    struct Logical final : public Expression
    {
        std::shared_ptr<Expression> left;
        Token op;
        std::shared_ptr<Expression> right;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitLogicalExpression(*this, shared);
        }

        Logical(std::shared_ptr<Expression> left, Token op, std::shared_ptr<Expression> right)
            : left(std::move(left))
            , op(std::move(op))
            , right(std::move(right))
        {
        }
    };

}  // namespace sail::Expressions
