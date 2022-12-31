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
        void interpret(std::vector<Statement>& statements);

        void executeBlock(std::vector<Statement>& statements,
                          std::shared_ptr<Environment> environment);

        void resolve(Expression& expression, size_t depth);

      private:
        auto evaluate(Expression& expression) -> Value;

        // Statement execution
        auto blockStatement(std::shared_ptr<Statements::Block>& statement) -> void;
        auto classStatement(std::shared_ptr<Statements::Class>& statement) -> void;
        auto expressionStatement(std::shared_ptr<Statements::Expression>& statement) -> void;
        auto functionStatement(std::shared_ptr<Statements::Function>& statement) -> void;
        auto ifStatement(std::shared_ptr<Statements::If>& statement) -> void;
        auto returnStatement(std::shared_ptr<Statements::Return>& statement) -> void;
        auto variableStatement(std::shared_ptr<Statements::Variable>& statement) -> void;
        auto whileStatement(std::shared_ptr<Statements::While>& statement) -> void;

        // Expression evaluation
        auto assignmentExpression(std::shared_ptr<Expressions::Assignment>& assignment) -> Value;
        auto callExpression(std::shared_ptr<Expressions::Call>& call) -> Value;
        auto getExpression(std::shared_ptr<Expressions::Get>& get) -> Value;
        auto setExpression(std::shared_ptr<Expressions::Set>& set) -> Value;
        auto literalExpression(std::shared_ptr<Expressions::Literal>& literal) -> Value;
        auto logicalExpression(std::shared_ptr<Expressions::Logical>& logical) -> Value;
        auto groupingExpression(std::shared_ptr<Expressions::Grouping>& grouping) -> Value;
        auto unaryExpression(std::shared_ptr<Expressions::Unary>& unary) -> Value;
        auto binaryExpression(std::shared_ptr<Expressions::Binary>& binary) -> Value;
        auto variableExpression(std::shared_ptr<Expressions::Variable>& variable) -> Value;
        auto thisExpression(std::shared_ptr<Expressions::This>& thisExpr) -> Value;
        auto superExpression(std::shared_ptr<Expressions::Super>& super) -> Value;

        auto lookupVariable(Token& name, Expression& expression) -> Value;

        std::shared_ptr<Environment> _globalEnvironment;
        std::shared_ptr<Environment> _environment;
        std::unordered_map<Expression, size_t> _locals;
    };
}  // namespace sail
