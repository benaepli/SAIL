#pragma once

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Variable
        : public Statement
        , public std::enable_shared_from_this<Variable>
    {
        Token name;
        std::shared_ptr<sail::Expression> initializer;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<Variable> shared = shared_from_this();
            visitor.visitVariableStatement(shared);
        }

        Variable(Token name, std::shared_ptr<sail::Expression> initializer)
            : name(std::move(name))
            , initializer(std::move(initializer))
        {
        }
    };
}  // namespace sail::Statements