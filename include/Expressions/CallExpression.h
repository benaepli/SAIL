#pragma once

#include <memory>
#include <vector>

#include "Expression.h"

namespace sail::Expressions
{
    struct Call final : public Expression
    {
        std::shared_ptr<Expression> callee;
        Token paren;
        std::vector<std::shared_ptr<Expression>> arguments;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitCallExpression(*this, shared);
        }

        Call(std::shared_ptr<Expression> callee,
             Token paren,
             std::vector<std::shared_ptr<Expression>> arguments)
            : callee(std::move(callee))
            , paren(std::move(paren))
            , arguments(std::move(arguments))
        {
        }
    };
}  // namespace sail::Expressions
