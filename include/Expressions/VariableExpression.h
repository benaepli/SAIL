#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Variable final : public Expression
    {
        Token name;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitVariableExpression(*this, shared);
        }

        explicit Variable(Token name)
            : name(std::move(name))
        {
        }
    };

}  // namespace sail::Expressions
