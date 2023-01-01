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
            : _value(std::move(value))
        {
        }
        auto value() const -> Value { return _value; }

      private:
        Value _value;
    };

    class Interpreter
        : public ExpressionVisitor
        , public StatementVisitor
    {
      public:
        Interpreter();

        void execute(std::shared_ptr<Statement>& statement);
        void interpret(std::vector<std::shared_ptr<Statement>>& statements);

        void executeBlock(std::vector<std::shared_ptr<Statement>>& statements,
                          std::shared_ptr<Environment> environment);

        void resolve(std::shared_ptr<Expression>& expression, size_t depth);

      private:
        auto evaluate(std::shared_ptr<Expression>& expression) -> Value&;

        void visitBlockStatement(std::shared_ptr<Statements::Block>& block) override;
        void visitClassStatement(std::shared_ptr<Statements::Class>& classStatement) override;
        void visitExpressionStatement(std::shared_ptr<Statements::Expression>& expression) override;
        void visitFunctionStatement(std::shared_ptr<Statements::Function>& function) override;
        void visitIfStatement(std::shared_ptr<Statements::If>& ifStatement) override;
        void visitReturnStatement(std::shared_ptr<Statements::Return>& returnStatement) override;
        void visitVariableStatement(std::shared_ptr<Statements::Variable>& variable) override;
        void visitWhileStatement(std::shared_ptr<Statements::While>& whileStatement) override;

        void visitAssignmentExpression(
            std::shared_ptr<Expressions::Assignment>& assignment) override;
        void visitBinaryExpression(std::shared_ptr<Expressions::Binary>& binary) override;
        void visitCallExpression(std::shared_ptr<Expressions::Call>& call) override;
        void visitGetExpression(std::shared_ptr<Expressions::Get>& get) override;
        void visitGroupingExpression(std::shared_ptr<Expressions::Grouping>& grouping) override;
        void visitLiteralExpression(std::shared_ptr<Expressions::Literal>& literal) override;
        void visitLogicalExpression(std::shared_ptr<Expressions::Logical>& logical) override;
        void visitSetExpression(std::shared_ptr<Expressions::Set>& set) override;
        void visitSuperExpression(std::shared_ptr<Expressions::Super>& super) override;
        void visitThisExpression(std::shared_ptr<Expressions::This>& thisExpr) override;
        void visitUnaryExpression(std::shared_ptr<Expressions::Unary>& unary) override;
        void visitVariableExpression(std::shared_ptr<Expressions::Variable>& variable) override;

        auto lookupVariable(Token& name, std::shared_ptr<Expression>& expression) -> Value;

        std::shared_ptr<Environment> _globalEnvironment;
        std::shared_ptr<Environment> _environment;
        ankerl::unordered_dense::map<std::shared_ptr<Expression>, size_t> _locals;

        Value _returnValue;
    };
}  // namespace sail
