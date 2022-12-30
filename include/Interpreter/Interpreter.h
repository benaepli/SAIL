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
    {
      public:
        Interpreter();

        void execute(Statement& statement);
        void interpret(std::vector<std::unique_ptr<Statement>>& statements);

        void executeBlock(std::vector<std::unique_ptr<Statement>>& statements,
                          std::shared_ptr<Environment> environment);

        void resolve(Expression& expression, size_t depth);

      private:
        auto evaluate(Expression& expression) -> Value;

        // Statement execution
        auto blockStatement(Statements::Block& statement) -> void;
        auto classStatement(Statements::Class& statement) -> void;
        auto expressionStatement(Statements::Expression& statement) -> void;
        auto functionStatement(Statements::Function& statement) -> void;
        auto ifStatement(Statements::If& statement) -> void;
        auto returnStatement(Statements::Return& statement) -> void;
        auto variableStatement(Statements::Variable& statement) -> void;
        auto whileStatement(Statements::While& statement) -> void;

        // Expression evaluation
        auto assignmentExpression(Expressions::Assignment& assignment,
                                  Expression& expression) -> Value;
        auto callExpression(Expressions::Call& expression) -> Value;
        auto getExpression(Expressions::Get& expression) -> Value;
        auto setExpression(Expressions::Set& expression) -> Value;
        auto literalExpression(Expressions::Literal& expression) -> Value;
        auto logicalExpression(Expressions::Logical& expression) -> Value;
        auto groupingExpression(Expressions::Grouping& expression) -> Value;
        auto unaryExpression(Expressions::Unary& expression) -> Value;
        auto binaryExpression(Expressions::Binary& expression) -> Value;
        auto variableExpression(Expressions::Variable& variable,
                                Expression& expression) -> Value;
        auto thisExpression(Expressions::This& thisExpr, Expression& expression)
            -> Value;

        auto lookupVariable(Token& name, Expression& expression) -> Value;

        std::shared_ptr<Environment> _globalEnvironment;
        std::shared_ptr<Environment> _environment;
        ankerl::unordered_dense::map<Expression, size_t> _locals;
    };
}  // namespace sail
