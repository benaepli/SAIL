#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Class final : public Statement
    {
        Token name;
        std::shared_ptr<Expressions::Variable> superclass;
        std::vector<std::shared_ptr<Statements::Function>> methods;

        void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) override
        {
            visitor.visitClassStatement(*this, shared);
        }

        Class(Token name,
              std::shared_ptr<Expressions::Variable> superclass,
              std::vector<std::shared_ptr<Statements::Function>> methods)
            : name(std::move(name))
            , superclass(std::move(superclass))
            , methods(std::move(methods))
        {
        }
    };
}  // namespace sail::Statements
