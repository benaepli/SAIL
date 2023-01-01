#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Grouping
        : public Expression
        , public std::enable_shared_from_this<Grouping>
    {
        std::shared_ptr<Expression> expression;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Grouping> shared = shared_from_this();
            visitor.visitGroupingExpression(shared);
        }

        explicit Grouping(std::shared_ptr<Expression> expression)
            : expression(std::move(expression))
        {
        }
    };
}  // namespace sail::Expressions
