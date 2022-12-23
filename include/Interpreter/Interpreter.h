#pragma once

#include <memory>
#include <vector>

#include "Environment/Environment.h"
#include "Expressions/Expressions.h"
#include "Statements/Statements.h"

namespace sail
{
    class Interpreter
    {
      public:
        Interpreter();

        void execute(Statement& statement);
        void interpret(std::vector<Statement>& statements);

      private:
        auto evaluate(Expression& expression) -> LiteralType;

        // Statement execution
        auto blockStatement(Statements::Block& statement) -> void;
        auto expressionStatement(Statements::Expression& statement) -> void;
        auto printStatement(Statements::Print& statement) -> void;
        auto variableStatement(Statements::Variable& statement) -> void;

        // Expression evaluation
        auto assignmentExpression(Expressions::Assignment& expression)
            -> LiteralType;
        auto literalExpression(Expressions::Literal& expression) -> LiteralType;
        auto groupingExpression(Expressions::Grouping& expression)
            -> LiteralType;
        auto unaryExpression(Expressions::Unary& expression) -> LiteralType;
        auto binaryExpression(Expressions::Binary& expression) -> LiteralType;
        auto variableExpression(Expressions::Variable& expression)
            -> LiteralType;

        std::shared_ptr<Environment> _globalEnvironment;
        std::shared_ptr<Environment> _environment;
    };
}  // namespace sail
