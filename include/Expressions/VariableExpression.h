#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Variable
        : public Expression
        , public std::enable_shared_from_this<Variable>
    {
        Token name;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Variable> shared = shared_from_this();
            visitor.visitVariableExpression(shared);
        }

        explicit Variable(Token name)
            : name(std::move(name))
        {
        }
    };

}  // namespace sail::Expressions
