#pragma once

#include "Expression.h"
#include "Token/Token.h"

namespace sail::Expressions
{
    struct Logical
        : public Expression
        , public std::enable_shared_from_this<Logical>
    {
        std::shared_ptr<Expression> left;
        Token op;
        std::shared_ptr<Expression> right;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Logical> shared = shared_from_this();
            visitor.visitLogicalExpression(shared);
        }

        Logical(std::shared_ptr<Expression> left, Token op, std::shared_ptr<Expression> right)
            : left(std::move(left))
            , op(std::move(op))
            , right(std::move(right))
        {
        }
    };

}  // namespace sail::Expressions
