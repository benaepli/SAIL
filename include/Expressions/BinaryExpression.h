#pragma once

#include <memory>

#include "Expression.h"

namespace sail::Expressions
{
    struct Binary
        : public Expression
        , public std::enable_shared_from_this<Binary>
    {
        std::shared_ptr<Expression> left;
        Token op;
        std::shared_ptr<Expression> right;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Binary> shared = shared_from_this();
            visitor.visitBinaryExpression(shared);
        }

        Binary(std::shared_ptr<Expression> left, Token op, std::shared_ptr<Expression> right)
            : left(std::move(left))
            , op(std::move(op))
            , right(std::move(right))
        {
        }
    };

}  // namespace sail::Expressions
