#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct While final : public Statement
    {
        std::shared_ptr<sail::Expression> condition;
        std::shared_ptr<Statement> body;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitWhileStatement(*this, shared);
        }

        While(std::shared_ptr<sail::Expression> condition, std::shared_ptr<Statement> body)
            : condition(std::move(condition))
            , body(std::move(body))
        {
        }
    };

}  // namespace sail::Statements
