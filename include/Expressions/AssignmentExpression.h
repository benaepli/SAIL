#pragma once

#include <memory>

#include "Expression.h"

namespace sail::Expressions
{
    struct Assignment
        : public Expression
        , public std::enable_shared_from_this<Assignment>
    {
        Token name;
        std::shared_ptr<Expression> value;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Assignment> shared = shared_from_this();
            visitor.visitAssignmentExpression(shared);
        }

        Assignment(Token name, std::shared_ptr<Expression> value)
            : name(std::move(name))
            , value(std::move(value))
        {
        }
    };
}  // namespace sail::Expressions
