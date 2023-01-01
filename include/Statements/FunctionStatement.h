#pragma once

#include <memory>

#include "Statement.h"
#include "Token/Token.h"

namespace sail::Statements
{
    struct Function
        : public Statement
        , public std::enable_shared_from_this<Function>
    {
        Token name;
        std::vector<Token> parameters;
        std::vector<std::shared_ptr<Statement>> body;
        bool possibleInitializer;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<Function> shared = shared_from_this();
            visitor.visitFunctionStatement(shared);
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
