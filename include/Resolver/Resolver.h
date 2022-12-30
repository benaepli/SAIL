#pragma once

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "Statements/Statements.h"
#include "ankerl/unordered_dense.h"

namespace sail
{
    class Interpreter;

    enum class ClassType
    {
        eNone,
        eClass,
    };

    enum class FunctionType
    {
        eNone,
        eFunction,
        eMethod,
    };

    class Resolver
    {
      public:
        explicit Resolver(Interpreter& interpreter);

        void resolve(std::vector<std::unique_ptr<Statement>>& statements);
        void resolve(Statement& statement);
        void resolve(Expression& expression);

      private:
        void blockStatement(Statements::Block& block);
        void classStatement(Statements::Class& classStatement);
        void expressionStatement(Statements::Expression& expression);
        void functionStatement(Statements::Function& function);
        void ifStatement(Statements::If& ifStatement);
        void returnStatement(Statements::Return& returnStatement);
        void variableStatement(Statements::Variable& variable);
        void whileStatement(Statements::While& whileStatement);

        void assignmentExpression(Expressions::Assignment& assignment,
                                  Expression& expression);
        void binaryExpression(Expressions::Binary& binary);
        void callExpression(Expressions::Call& call);
        void getExpression(Expressions::Get& get);
        void groupingExpression(Expressions::Grouping& grouping);
        void literalExpression(Expressions::Literal& literal);
        void logicalExpression(Expressions::Logical& logical);
        void setExpression(Expressions::Set& set);
        void thisExpression(Expressions::This& thisExpression,
                            Expression& expression);
        void unaryExpression(Expressions::Unary& unary);
        void variableExpression(Expressions::Variable& variable,
                                Expression& expression);

        void beginScope();
        void endScope();
        void declare(const Token& name);
        void define(const Token& name);
        void resolveFunction(Statements::Function& function, FunctionType type);
        void resolveLocal(Expression& expression, const Token& name);

        Interpreter& _interpreter;
        std::stack<ankerl::unordered_dense::map<std::string, bool>> _scopes;
        ClassType _currentClass = ClassType::eNone;
        FunctionType _currentFunction = FunctionType::eNone;
    };
}  // namespace sail