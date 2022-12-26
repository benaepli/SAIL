#include <algorithm>
#include <any>
#include <limits>
#include <stdexcept>
#include <utility>
#include <variant>

#include "Interpreter/Interpreter.h"

#include "../utils/Overload.h"
#include "Errors/RuntimeError.h"
#include "Expressions/Expression.h"
#include "Expressions/Expressions.h"
#include "Expressions/VariableExpression.h"
#include "Native/DefineNative.h"
#include "Statements/Statements.h"
#include "Token/Token.h"
#include "Types/Value.h"
#include "fmt/format.h"

namespace sail
{
    Interpreter::Interpreter()
        : _globalEnvironment(std::make_unique<Environment>())
        , _environment(_globalEnvironment)
    {
        defineNativeFunctions(*_globalEnvironment);
    }

    void Interpreter::interpret(
        std::vector<std::unique_ptr<Statement>>& statements)
    {
        auto each = [&](auto& statement) -> void { execute(*statement); };
        std::ranges::for_each(statements, each);
    }

    void Interpreter::execute(Statement& statement)
    {
        std::visit(
            Overload {
                [this](Statements::Block& statement) -> void
                { blockStatement(statement); },
                [this](Statements::Expression& statement) -> void
                { expressionStatement(statement); },
                [this](Statements::Function& statement) -> void
                { functionStatement(statement); },
                [this](Statements::If& statement) -> void
                { ifStatement(statement); },
                [this](Statements::Return& statement) -> void
                { returnStatement(statement); },
                [this](Statements::Variable& statement) -> void
                { variableStatement(statement); },
                [this](Statements::While& statement) -> void
                { whileStatement(statement); },
            },
            statement);
    }

    auto Interpreter::evaluate(Expression& expression) -> Value
    {
        return std::visit(
            Overload {
                [this,
                 &expression](Expressions::Assignment& assignment) -> Value
                { return assignmentExpression(assignment, expression); },
                [this](Expressions::Call& expression) -> Value
                { return callExpression(expression); },
                [this](Expressions::Literal& expression) -> Value
                { return literalExpression(expression); },
                [this](Expressions::Logical& expression) -> Value
                { return logicalExpression(expression); },
                [this](Expressions::Grouping& expression) -> Value
                { return groupingExpression(expression); },
                [this](Expressions::Unary& expression) -> Value
                { return unaryExpression(expression); },
                [this](Expressions::Binary& expression) -> Value
                { return binaryExpression(expression); },
                [this, &expression](Expressions::Variable& variable) -> Value
                { return variableExpression(variable, expression); }},
            expression);
    }

    void Interpreter::blockStatement(Statements::Block& statement)
    {
        executeBlock(statement.statements,
                     std::make_shared<Environment>(_environment));
    }

    void Interpreter::executeBlock(
        std::vector<std::unique_ptr<Statement>>& statements,
        std::shared_ptr<Environment> environment)
    {
        std::shared_ptr<Environment> previousEnvironment = _environment;
        _environment = std::move(environment);

        auto each = [&](auto& statement) -> void { execute(*statement); };
        std::ranges::for_each(statements, each);

        _environment = previousEnvironment;
    }

    void Interpreter::resolve(Expression& expression, size_t depth)
    {
        _locals[&expression] = depth;
    }

    void Interpreter::expressionStatement(Statements::Expression& statement)
    {
        evaluate(*statement.expression);
    }

    void Interpreter::functionStatement(Statements::Function& statement)
    {
        auto function = std::make_shared<Types::Function>(std::move(statement),
                                                          _environment);
        _environment->define(function->name(), function);
    }

    void Interpreter::ifStatement(Statements::If& statement)
    {
        if (evaluate(*statement.condition).isTruthy())
        {
            execute(*statement.thenBranch);
        }
        else if (statement.elseBranch != nullptr)
        {
            execute(*statement.elseBranch);
        }
    }

    void Interpreter::returnStatement(Statements::Return& statement)
    {
        Value value = Types::Null {};

        if (statement.value != nullptr)
        {
            value = evaluate(*statement.value);
        }

        throw Return {value};
    }

    void Interpreter::variableStatement(Statements::Variable& statement)
    {
        Value value = Types::Null {};

        if (statement.initializer != nullptr)
        {
            value = evaluate(*statement.initializer);
        }

        _environment->define(statement.name, value);
    }

    void Interpreter::whileStatement(Statements::While& statement)
    {
        while (evaluate(*statement.condition).isTruthy())
        {
            execute(*statement.body);
        }
    }

    auto Interpreter::assignmentExpression(Expressions::Assignment& assignment,
                                           Expression& expression) -> Value
    {
        Value value = evaluate(*assignment.value);

        if (_locals.contains(&expression))
        {
            size_t distance = _locals[&expression];
            _environment->assignAt(distance, assignment.name, value);
        }
        else
        {
            _environment->assign(assignment.name, value);
        }

        return value;
    }

    auto Interpreter::literalExpression(Expressions::Literal& expression)
        -> Value
    {
        Value value {};

        std::visit(
            Overload {
                [&](const std::string& str) { value = str; },
                [&](const double& num) { value = num; },
                [&](const bool& val) { value = val; },
                [&](const Types::Null&) { value = Types::Null {}; },
            },
            expression.literal);

        return value;
    }

    auto Interpreter::logicalExpression(Expressions::Logical& expression)
        -> Value
    {
        Value left = evaluate(*expression.left);
        if (expression.oper.type == TokenType::eOr)
        {
            if (left.isTruthy())
            {
                return left;
            }
        }
        else
        {
            if (!left.isTruthy())
            {
                return left;
            }
        }
        return evaluate(*expression.right);
    }

    auto Interpreter::groupingExpression(Expressions::Grouping& expression)
        -> Value
    {
        return evaluate(*expression.expression);
    }

    auto Interpreter::unaryExpression(Expressions::Unary& expression) -> Value
    {
        Value right = evaluate(*expression.right);
        switch (expression.oper.type)
        {
            case TokenType::eMinus:
            {
                std::optional<double> number = right.asNumber();
                if (number.has_value())
                {
                    return -(*number);
                }
                throw RuntimeError(expression.oper,
                                   "Cannot negate a non-number");
            }
            case TokenType::eBang:
                return !right.isTruthy();
            default:;
        }

        return {};
    }

    auto Interpreter::binaryExpression(Expressions::Binary& expression) -> Value
    {
        Value left = evaluate(*expression.left);
        Value right = evaluate(*expression.right);

        if (expression.oper.type == TokenType::ePlus)
        {
            if (left.isString() && right.isString())
            {
                return std::get<std::string>(left)
                    + std::get<std::string>(right);
            }
        }

        if (expression.oper.type == TokenType::eBangEqual)
        {
            return left != right;
        }

        if (expression.oper.type == TokenType::eEqualEqual)
        {
            return left == right;
        }

        std::optional<double> leftNumber = left.asNumber();
        std::optional<double> rightNumber = right.asNumber();

        if (!leftNumber.has_value() || !rightNumber.has_value())
        {
            throw RuntimeError(expression.oper,
                               "Cannot perform arithmetic on non-numbers");
        }

        double leftValue = *leftNumber;
        double rightValue = *rightNumber;

        switch (expression.oper.type)
        {
            case TokenType::eMinus:
                return leftValue - rightValue;
            case TokenType::eSlash:
                return leftValue / rightValue;
            case TokenType::eStar:
                return leftValue * rightValue;
            case TokenType::ePlus:
                return leftValue + rightValue;
            case TokenType::eGreater:
                return leftValue > rightValue;
            case TokenType::eGreaterEqual:
                return leftValue >= rightValue;
            case TokenType::eLess:
                return leftValue < rightValue;
            case TokenType::eLessEqual:
                return leftValue <= rightValue;
            default:;
        }

        throw RuntimeError(expression.oper, "Unknown operator");
    }

    auto Interpreter::callExpression(Expressions::Call& expression) -> Value
    {
        Value callee = evaluate(*expression.callee);
        std::vector<Value> arguments;
        auto each = [&](auto& argument) -> void
        { arguments.push_back(evaluate(*argument)); };
        std::ranges::for_each(expression.arguments, each);

        if (std::shared_ptr<Types::Callable>* callablePointer =
                std::get_if<std::shared_ptr<Types::Callable>>(&callee))
        {
            std::shared_ptr<Types::Callable> callable = *callablePointer;
            size_t arity = callable->arity();
            if (arguments.size() != arity
                && arity != std::numeric_limits<size_t>::max())
            {
                throw RuntimeError(
                    expression.paren,
                    fmt::format("Expected {} arguments but got {}",
                                callable->arity(),
                                arguments.size()));
            }
            return callable->call(*this, arguments);
        }

        throw RuntimeError(expression.paren, "Can only call functions");
    }

    auto Interpreter::variableExpression(Expressions::Variable& variable,
                                         Expression& expression) -> Value
    {
        if (_locals.contains(&expression))
        {
            size_t distance = _locals[&expression];
            return _environment->getAt(distance, variable.name);
        }

        return _environment->get(variable.name);
    }

}  // namespace sail
