#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{

    struct If
        : public Statement
        , public std::enable_shared_from_this<If>
    {
        std::shared_ptr<sail::Expression> condition;
        std::shared_ptr<Statement> thenBranch;
        std::shared_ptr<Statement> elseBranch;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<If> shared = shared_from_this();
            visitor.visitIfStatement(shared);
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
