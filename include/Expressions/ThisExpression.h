#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct This final : public Expression
    {
        Token keyword;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitThisExpression(*this, shared);
        }

        explicit This(Token keyword)
            : keyword(std::move(keyword))
        {
        }
    };
}  // namespace sail::Expressions
