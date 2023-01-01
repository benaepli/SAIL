#pragma once

#include <memory>
#include <vector>

#include "Expression.h"

namespace sail::Expressions
{
    struct Call
        : public Expression
        , public std::enable_shared_from_this<Call>
    {
        std::shared_ptr<Expression> callee;
        Token paren;
        std::vector<std::shared_ptr<Expression>> arguments;

        void accept(ExpressionVisitor& visitor) override
        {
            std::shared_ptr<Call> shared = shared_from_this();
            visitor.visitCallExpression(shared);
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
