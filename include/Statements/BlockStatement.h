#pragma once

#include <memory>
#include <vector>

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Block final : public Statement
    {
        std::vector<std::shared_ptr<Statement>> statements;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitBlockStatement(*this, shared);
        }

        explicit Block(std::vector<std::shared_ptr<Statement>> statements)
            : statements(std::move(statements))
        {
        }
    };
}  // namespace sail::Statements
