#pragma once

#include <memory>
#include <vector>

#include "Environment/Environment.h"
#include "Expressions/Expressions.h"
#include "Statements/Statements.h"
#include "Types/Types.h"

namespace sail
{
    class Return : public std::exception
    {
    public:
        explicit Return(Value value)
            : _value(std::move(value)) { }

        auto value() const -> Value { return _value; }

    private:
        Value _value;
    };

    class Interpreter final
        : public ExpressionVisitor
          , public StatementVisitor
    {
    public:
        Interpreter();

        void execute(std::shared_ptr<Statement>& statement);
        void interpret(std::vector<std::shared_ptr<Statement>>& statements);

        void executeBlock(std::vector<std::shared_ptr<Statement>>& statements,
                          std::shared_ptr<Environment> environment);

        void resolve(const std::shared_ptr<Expression>& expression, size_t depth);

    private:
        auto evaluate(std::shared_ptr<Expression>& expression) -> Value&;

        void visitBlockStatement(Statements::Block& blockStatement,
                                 std::shared_ptr<Statement>& shared) override;
        void visitClassStatement(Statements::Class& classStatement,
                                 std::shared_ptr<Statement>& shared) override;
        void visitExpressionStatement(Statements::Expression& expressionStatement,
                                      std::shared_ptr<Statement>& shared) override;
        void visitFunctionStatement(Statements::Function& functionStatement,
                                    std::shared_ptr<Statement>& shared) override;
        void visitIfStatement(Statements::If& ifStatement,
                              std::shared_ptr<Statement>& shared) override;
        void visitReturnStatement(Statements::Return& returnStatement,
                                  std::shared_ptr<Statement>& shared) override;
        void visitVariableStatement(Statements::Variable& variableStatement,
                                    std::shared_ptr<Statement>& shared) override;
        void visitWhileStatement(Statements::While& whileStatement,
                                 std::shared_ptr<Statement>& shared) override;

        void visitAssignmentExpression(Expressions::Assignment& assignmentExpression,
                                       std::shared_ptr<Expression>& shared) override;
        void visitBinaryExpression(Expressions::Binary& binaryExpression,
                                   std::shared_ptr<Expression>& shared) override;
        void visitCallExpression(Expressions::Call& callExpression,
                                 std::shared_ptr<Expression>& shared) override;
        void visitGetExpression(Expressions::Get& getExpression,
                                std::shared_ptr<Expression>& shared) override;
        void visitGroupingExpression(Expressions::Grouping& groupingExpression,
                                     std::shared_ptr<Expression>& shared) override;
        void visitLiteralExpression(Expressions::Literal& literalExpression,
                                    std::shared_ptr<Expression>& shared) override;
        void visitLogicalExpression(Expressions::Logical& logicalExpression,
                                    std::shared_ptr<Expression>& shared) override;
        void visitSetExpression(Expressions::Set& setExpression,
                                std::shared_ptr<Expression>& shared) override;
        void visitSuperExpression(Expressions::Super& superExpression,
                                  std::shared_ptr<Expression>& shared) override;
        void visitThisExpression(Expressions::This& thisExpression,
                                 std::shared_ptr<Expression>& shared) override;
        void visitUnaryExpression(Expressions::Unary& unaryExpression,
                                  std::shared_ptr<Expression>& shared) override;
        void visitVariableExpression(Expressions::Variable& variableExpression,
                                     std::shared_ptr<Expression>& shared) override;

        auto lookupVariable(const Token& name,
                            const std::shared_ptr<Expression>& expression) -> Value;

        std::shared_ptr<Environment> _globalEnvironment;
        std::shared_ptr<Environment> _environment;
        ankerl::unordered_dense::map<std::shared_ptr<Expression>, size_t> _locals;

        Value _returnValue;
    };
} // namespace sail
