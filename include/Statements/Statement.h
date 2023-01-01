#pragma once

#include <memory>
#include <variant>

#include "utils/classes.h"

namespace sail
{
    namespace Statements
    {
        struct Block;
        struct Class;
        struct Expression;
        struct Function;
        struct If;
        struct Return;
        struct Variable;
        struct While;
    }  // namespace Statements

    // refactor to classes
    class StatementVisitor
    {
      public:
        StatementVisitor() = default;
        virtual ~StatementVisitor() = default;

        SAIL_DEFAULT_COPY_MOVE(StatementVisitor);

        virtual void visitBlockStatement(std::shared_ptr<Statements::Block>& statement) = 0;
        virtual void visitClassStatement(std::shared_ptr<Statements::Class>& statement) = 0;
        virtual void visitExpressionStatement(
            std::shared_ptr<Statements::Expression>& statement) = 0;
        virtual void visitFunctionStatement(std::shared_ptr<Statements::Function>& statement) = 0;
        virtual void visitIfStatement(std::shared_ptr<Statements::If>& statement) = 0;
        virtual void visitReturnStatement(std::shared_ptr<Statements::Return>& statement) = 0;
        virtual void visitVariableStatement(std::shared_ptr<Statements::Variable>& statement) = 0;
        virtual void visitWhileStatement(std::shared_ptr<Statements::While>& statement) = 0;
    };

    class Statement
    {
      public:
        Statement() = default;
        virtual ~Statement() = default;

        SAIL_DEFAULT_COPY_MOVE(Statement);

        virtual void accept(StatementVisitor& visitor) = 0;
    };
}  // namespace sail
