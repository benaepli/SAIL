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

    class Statement;

    class StatementVisitor
    {
      public:
        StatementVisitor() = default;
        virtual ~StatementVisitor() = default;

        SAIL_DEFAULT_COPY_MOVE(StatementVisitor);

        virtual void visitBlockStatement(Statements::Block& blockStatement,
                                         std::shared_ptr<Statement>& shared) = 0;
        virtual void visitClassStatement(Statements::Class& classStatement,
                                         std::shared_ptr<Statement>& shared) = 0;
        virtual void visitExpressionStatement(Statements::Expression& expressionStatement,
                                              std::shared_ptr<Statement>& shared) = 0;
        virtual void visitFunctionStatement(Statements::Function& functionStatement,
                                            std::shared_ptr<Statement>& shared) = 0;
        virtual void visitIfStatement(Statements::If& ifStatement,
                                      std::shared_ptr<Statement>& shared) = 0;
        virtual void visitReturnStatement(Statements::Return& returnStatement,
                                          std::shared_ptr<Statement>& shared) = 0;
        virtual void visitVariableStatement(Statements::Variable& variableStatement,
                                            std::shared_ptr<Statement>& shared) = 0;
        virtual void visitWhileStatement(Statements::While& whileStatement,
                                         std::shared_ptr<Statement>& shared) = 0;
    };

    class Statement
    {
      public:
        Statement() = default;
        virtual ~Statement() = default;

        SAIL_DEFAULT_COPY_MOVE(Statement);

        virtual void accept(StatementVisitor& visitor, std::shared_ptr<Statement>& shared) = 0;
    };
}  // namespace sail
