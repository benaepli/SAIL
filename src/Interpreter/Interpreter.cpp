#include <algorithm>
#include <any>
#include <variant>

#include "Interpreter/Interpreter.h"

#include "../utils/Overload.h"
#include "Errors/RuntimeError.h"
#include "Expressions/Expression.h"
#include "Expressions/Expressions.h"
#include "Expressions/VariableExpression.h"
#include "Statements/Statements.h"
#include "Token/Token.h"
#include "fmt/format.h"

namespace sail
{
    Interpreter::Interpreter()
        : _globalEnvironment(std::make_unique<Environment>())
        , _environment(_globalEnvironment)
    {
    }

    void Interpreter::interpret(std::vector<Statement>& statements)
    {
        auto each = [&](auto& statement) -> void { execute(statement); };

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
                [this](Statements::Print& statement) -> void
                { printStatement(statement); },
                [this](Statements::Variable& statement) -> void
                { variableStatement(statement); },
            },
            statement);
    }

    auto Interpreter::evaluate(Expression& expression) -> LiteralType
    {
        return std::visit(
            Overload {[this](Expressions::Assignment& expression) -> LiteralType
                      { return assignmentExpression(expression); },
                      [this](Expressions::Literal& expression) -> LiteralType
                      { return literalExpression(expression); },
                      [this](Expressions::Grouping& expression) -> LiteralType
                      { return groupingExpression(expression); },
                      [this](Expressions::Unary& expression) -> LiteralType
                      { return unaryExpression(expression); },
                      [this](Expressions::Binary& expression) -> LiteralType
                      { return binaryExpression(expression); },
                      [this](Expressions::Variable& expression) -> LiteralType
                      { return variableExpression(expression); }},
            expression);
    }

    void Interpreter::blockStatement(Statements::Block& statement)
    {
        std::shared_ptr<Environment> previousEnvironment = _environment;
        _environment = std::make_shared<Environment>(_environment);

        auto each = [&](auto& statement) -> void { execute(statement); };
        std::ranges::for_each(statement.statements, each);

        _environment = previousEnvironment;
    }

    void Interpreter::expressionStatement(Statements::Expression& statement)
    {
        evaluate(*statement.expression);
    }

    void Interpreter::printStatement(Statements::Print& statement)
    {
        LiteralType value = evaluate(*statement.expression);
        std::cout << value << std::endl;
    }

    void Interpreter::variableStatement(Statements::Variable& statement)
    {
        LiteralType value = LiteralNull {};

        if (statement.initializer != nullptr)
        {
            value = evaluate(*statement.initializer);
        }

        _environment->define(statement.name, value);
    }

    auto Interpreter::assignmentExpression(Expressions::Assignment& expression)
        -> LiteralType
    {
        LiteralType value = evaluate(*expression.value);
        _environment->assign(expression.name, value);
        return value;
    }

    auto Interpreter::literalExpression(Expressions::Literal& expression)
        -> LiteralType
    {
        return expression.literal;
    }

    auto Interpreter::groupingExpression(Expressions::Grouping& expression)
        -> LiteralType
    {
        return evaluate(*expression.expression);
    }

    auto Interpreter::unaryExpression(Expressions::Unary& expression)
        -> LiteralType
    {
        LiteralType right = evaluate(*expression.right);
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

    auto Interpreter::binaryExpression(Expressions::Binary& expression)
        -> LiteralType
    {
        LiteralType left = evaluate(*expression.left);
        LiteralType right = evaluate(*expression.right);

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

    auto Interpreter::variableExpression(Expressions::Variable& expression)
        -> LiteralType
    {
        return _environment->get(expression.name);
    }

}  // namespace sail
