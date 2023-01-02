#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"
#include "Token/Token.h"

namespace sail::Statements
{
    struct Return final : public Statement

    {
        std::shared_ptr<sail::Expression> value;
        Token keyword;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitReturnStatement(*this, shared);
        }

        Return(Token keyword, std::shared_ptr<sail::Expression> value)
            : value(std::move(value))
            , keyword(std::move(keyword))
        {
        }
    };

}  // namespace sail::Statements
