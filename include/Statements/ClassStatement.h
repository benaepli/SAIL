#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Class
        : public Statement
        , public std::enable_shared_from_this<Class>
    {
        Token name;
        std::shared_ptr<Expressions::Variable> superclass;
        std::vector<std::shared_ptr<Statements::Function>> methods;

        void accept(StatementVisitor& visitor) override
        {
            std::shared_ptr<Class> shared = shared_from_this();
            visitor.visitClassStatement(shared);
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
