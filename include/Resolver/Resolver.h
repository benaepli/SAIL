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

        void beginScope();
        void endScope();
        void declare(const Token& name);
        void define(const Token& name);
        void resolveFunction(std::shared_ptr<Statements::Function>& function, FunctionType type);
        void resolveLocal(std::shared_ptr<Expression>& expression, const Token& name);

        Interpreter& _interpreter;
        std::vector<ankerl::unordered_dense::map<std::string, bool>> _scopes;
        ClassType _currentClass = ClassType::eNone;
        FunctionType _currentFunction = FunctionType::eNone;
    };
}  // namespace sail