#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{

    struct If final : public Statement
    {
        std::shared_ptr<sail::Expression> condition;
        std::shared_ptr<Statement> thenBranch;
        std::shared_ptr<Statement> elseBranch;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitIfStatement(*this, shared);
        }

        If(std::shared_ptr<sail::Expression> condition,
           std::shared_ptr<Statement> thenBranch,
           std::shared_ptr<Statement> elseBranch)
            : condition(std::move(condition))
            , thenBranch(std::move(thenBranch))
            , elseBranch(std::move(elseBranch))
        {
        }
    };
}  // namespace sail::Statements
