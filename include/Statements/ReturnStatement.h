#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"
#include "Token/Token.h"

namespace sail::Statements
{
    struct Return
        : public Statement
        , public std::enable_shared_from_this<Return>
    {
        std::shared_ptr<sail::Expression> value;
        Token keyword;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<Return> shared = shared_from_this();
            visitor.visitReturnStatement(shared);
        }

        Return(Token keyword, std::shared_ptr<sail::Expression> value)
            : value(std::move(value))
            , keyword(std::move(keyword))
        {
        }
    };

}  // namespace sail::Statements
