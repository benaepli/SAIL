#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Literal final : public Expression
    {
        LiteralType literal;

        void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) override
        {
            visitor.visitLiteralExpression(*this, shared);
        }

        explicit Literal(LiteralType literal)
            : literal(std::move(literal))
        {
        }
    };
}  // namespace sail::Expressions
