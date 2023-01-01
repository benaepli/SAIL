#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Get
        : public Expression
        , public std::enable_shared_from_this<Get>
    {
        std::shared_ptr<Expression> object;
        Token name;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Get> shared = shared_from_this();
            visitor.visitGetExpression(shared);
        }

        Get(std::shared_ptr<Expression> object, Token name)
            : object(std::move(object))
            , name(std::move(name))
        {
        }
    };
}  // namespace sail::Expressions
