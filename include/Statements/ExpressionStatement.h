#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Expression
        : public Statement
        , public std::enable_shared_from_this<Expression>
    {
        std::shared_ptr<sail::Expression> expression;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<Expression> shared = shared_from_this();
            visitor.visitExpressionStatement(shared);
        }

        explicit Expression(std::shared_ptr<sail::Expression> expression)
            : expression(std::move(expression))
        {
        }
    };
}  // namespace sail::Statements
