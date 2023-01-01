#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Set
        : public Expression
        , public std::enable_shared_from_this<Set>
    {
        std::shared_ptr<Expression> object;
        Token name;
        std::shared_ptr<Expression> value;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Set> shared = shared_from_this();
            visitor.visitSetExpression(shared);
        }

        Set(std::shared_ptr<Expression> object, Token name, std::shared_ptr<Expression> value)
            : object(std::move(object))
            , name(std::move(name))
            , value(std::move(value))
        {
        }
    };

}  // namespace sail::Expressions
