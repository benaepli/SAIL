#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct This
        : public Expression
        , public std::enable_shared_from_this<This>
    {
        Token keyword;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<This> shared = shared_from_this();
            visitor.visitThisExpression(shared);
        }

        explicit This(Token keyword)
            : keyword(std::move(keyword))
        {
        }
    };
}  // namespace sail::Expressions
