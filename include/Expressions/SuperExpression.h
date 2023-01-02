#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Super final : public Expression
    {
        Token keyword;
        Token method;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitSuperExpression(*this, shared);
        }

        Super(Token keyword, Token method)
            : keyword(std::move(keyword))
            , method(std::move(method))
        {
        }
    };
}  // namespace sail::Expressions
