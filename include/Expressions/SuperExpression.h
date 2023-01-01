#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Super
        : public Expression
        , public std::enable_shared_from_this<Super>
    {
        Token keyword;
        Token method;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Super> shared = shared_from_this();
            visitor.visitSuperExpression(shared);
        }

        Super(Token keyword, Token method)
            : keyword(std::move(keyword))
            , method(std::move(method))
        {
        }
    };
}  // namespace sail::Expressions
