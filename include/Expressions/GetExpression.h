#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Get final : public Expression
    {
        std::shared_ptr<Expression> object;
        Token name;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitGetExpression(*this, shared);
        }

        Get(std::shared_ptr<Expression> object, Token name)
            : object(std::move(object))
            , name(std::move(name))
        {
        }
    };
}  // namespace sail::Expressions
