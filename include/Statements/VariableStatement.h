#pragma once

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Variable final : public Statement
    {
        Token name;
        std::shared_ptr<sail::Expression> initializer;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitVariableStatement(*this, shared);
        }

        Variable(Token name, std::shared_ptr<sail::Expression> initializer)
            : name(std::move(name))
            , initializer(std::move(initializer))
        {
        }
    };
}  // namespace sail::Statements