#pragma once

#include <memory>
#include <vector>

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Block
        : public Statement
        , public std::enable_shared_from_this<Block>
    {
        std::vector<std::shared_ptr<Statement>> statements;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<Block> shared = shared_from_this();
            visitor.visitBlockStatement(shared);
        }

        explicit Block(std::vector<std::shared_ptr<Statement>> statements)
            : statements(std::move(statements))
        {
        }
    };
}  // namespace sail::Statements
