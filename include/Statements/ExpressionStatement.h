#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Expression final : public Statement
    {
        std::shared_ptr<sail::Expression> expression;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitExpressionStatement(*this, shared);
        }

        explicit Expression(std::shared_ptr<sail::Expression> expression)
            : expression(std::move(expression))
        {
        }
    };
}  // namespace sail::Statements
