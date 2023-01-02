#pragma once

#include <memory>

#include "Statement.h"
#include "Token/Token.h"

namespace sail::Statements
{
    struct Function final : public Statement
    {
        Token name;
        std::vector<Token> parameters;
        std::vector<std::shared_ptr<Statement>> body;
        bool possibleInitializer;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitFunctionStatement(*this, shared);
        }

        Function(Token name,
                 std::vector<Token> parameters,
                 std::vector<std::shared_ptr<Statement>> body,
                 bool possibleInitializer = false)
            : name(std::move(name))
            , parameters(std::move(parameters))
            , body(std::move(body))
            , possibleInitializer(possibleInitializer)
        {
        }
    };

}  // namespace sail::Statements
