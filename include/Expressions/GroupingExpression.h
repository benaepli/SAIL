#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Grouping final : public Expression
    {
        std::shared_ptr<Expression> expression;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitGroupingExpression(*this, shared);
        }

        explicit Grouping(std::shared_ptr<Expression> expression)
            : expression(std::move(expression))
        {
        }
    };
}  // namespace sail::Expressions
