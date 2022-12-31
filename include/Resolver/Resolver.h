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
    {
      public:
        explicit Resolver(Interpreter& interpreter);

        void resolve(std::vector<Statement>& statements);
        void resolve(Statement& statement);
        void resolve(Expression& expression);

      private:
        void blockStatement(std::shared_ptr<Statements::Block>& block);
        void classStatement(std::shared_ptr<Statements::Class>& classStatement);
        void expressionStatement(std::shared_ptr<Statements::Expression>& expression);
        void functionStatement(std::shared_ptr<Statements::Function>& function);
        void ifStatement(std::shared_ptr<Statements::If>& ifStatement);
        void returnStatement(std::shared_ptr<Statements::Return>& returnStatement);
        void variableStatement(std::shared_ptr<Statements::Variable>& variable);
        void whileStatement(std::shared_ptr<Statements::While>& whileStatement);

        void assignmentExpression(std::shared_ptr<Expressions::Assignment>& assignment);
        void binaryExpression(std::shared_ptr<Expressions::Binary>& binary);
        void callExpression(std::shared_ptr<Expressions::Call>& call);
        void getExpression(std::shared_ptr<Expressions::Get>& get);
        void groupingExpression(std::shared_ptr<Expressions::Grouping>& grouping);
        void literalExpression(std::shared_ptr<Expressions::Literal>& literal);
        void logicalExpression(std::shared_ptr<Expressions::Logical>& logical);
        void setExpression(std::shared_ptr<Expressions::Set>& set);
        void superExpression(std::shared_ptr<Expressions::Super>& super);
        void thisExpression(std::shared_ptr<Expressions::This>& thisExpr);
        void unaryExpression(std::shared_ptr<Expressions::Unary>& unary);
        void variableExpression(std::shared_ptr<Expressions::Variable>& variable);

        void beginScope();
        void endScope();
        void declare(const Token& name);
        void define(const Token& name);
        void resolveFunction(std::shared_ptr<Statements::Function>& function, FunctionType type);
        void resolveLocal(Expression& expression, const Token& name);

        Interpreter& _interpreter;
        std::vector<ankerl::unordered_dense::map<std::string, bool>> _scopes;
        ClassType _currentClass = ClassType::eNone;
        FunctionType _currentFunction = FunctionType::eNone;
    };
}  // namespace sail