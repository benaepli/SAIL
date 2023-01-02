#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Set final : public Expression
    {
        std::shared_ptr<Expression> object;
        Token name;
        std::shared_ptr<Expression> value;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitSetExpression(*this, shared);
        }

        Set(std::shared_ptr<Expression> object, Token name, std::shared_ptr<Expression> value)
            : object(std::move(object))
            , name(std::move(name))
            , value(std::move(value))
        {
        }
    };

}  // namespace sail::Expressions
