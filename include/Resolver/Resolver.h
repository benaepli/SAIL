#pragma once

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "Expressions/Expression.h"
#include "Statements/Statements.h"
#include "ankerl/unordered_dense.h"

namespace sail
{
    class Interpreter;

    enum class ClassType
    {
        eNone = 1U << 0U,
        eClass = 1U << 1U,
        eSubclass = 1U << 2U,
    };

    enum class FunctionType
    {
        eNone,
        eFunction,
        eMethod,
        eInitializer,
    };

    class Resolver
        : public ExpressionVisitor
        , public StatementVisitor
    {
      public:
        explicit Resolver(Interpreter& interpreter);

        void resolve(std::vector<std::shared_ptr<Statement>>& statements);
        void resolve(std::shared_ptr<Statement>& statement);
        void resolve(std::shared_ptr<Expression>& expression);

      private:
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
        void beginScope();
        void endScope();
        void declare(const Token& name);
        void define(const Token& name);
        void resolveFunction(Statements::Function& functionStatement, FunctionType type);
        void resolveLocal(std::shared_ptr<Expression>& expression, const Token& name);

        Interpreter& _interpreter;
        std::vector<ankerl::unordered_dense::map<std::string, bool>> _scopes;
        ClassType _currentClass = ClassType::eNone;
        FunctionType _currentFunction = FunctionType::eNone;
    };
}  // namespace sail