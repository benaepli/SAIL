#pragma once

#include <memory>

#include "Expression.h"

namespace sail::Expressions
{
    struct Assignment final : public Expression
    {
        Token name;
        std::shared_ptr<Expression> value;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitAssignmentExpression(*this, shared);
        }

        Assignment(Token name, std::shared_ptr<Expression> value)
            : name(std::move(name))
            , value(std::move(value))
        {
        }
    };
}  // namespace sail::Expressions
