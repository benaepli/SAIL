#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct While
        : public Statement
        , public std::enable_shared_from_this<While>
    {
        std::shared_ptr<sail::Expression> condition;
        std::shared_ptr<Statement> body;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<While> shared = shared_from_this();
            visitor.visitWhileStatement(shared);
        }

        While(std::shared_ptr<sail::Expression> condition, std::shared_ptr<Statement> body)
            : condition(std::move(condition))
            , body(std::move(body))
        {
        }
    };

}  // namespace sail::Statements
