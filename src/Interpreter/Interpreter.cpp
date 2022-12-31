#include <algorithm>
#include <any>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <variant>

#include "Interpreter/Interpreter.h"

#include "Errors/RuntimeError.h"
#include "Expressions/Expression.h"
#include "Expressions/Expressions.h"
#include "Expressions/VariableExpression.h"
#include "Native/DefineNative.h"
#include "Statements/Statement.h"
#include "Statements/Statements.h"
#include "Token/Token.h"
#include "Types/CallableType.h"
#include "Types/FunctionType.h"
#include "Types/Types.h"
#include "Types/Value.h"
#include "fmt/format.h"
#include "utils/Overload.h"

namespace sail
{
    Interpreter::Interpreter()
        : _globalEnvironment(std::make_unique<Environment>())
          , _environment(_globalEnvironment)
    {
        defineNativeFunctions(*_globalEnvironment);
    }

    void Interpreter::interpret(std::vector<Statement>& statements)
    {
        auto each = [&](auto& statement) -> void { execute(statement); };
        std::ranges::for_each(statements, each);
    }

    void Interpreter::execute(Statement& statement)
    {
        std::visit(
            Overload{
                [this](std::shared_ptr<Statements::Block>& statement) -> void
                {
                    blockStatement(statement);
                },
                [this](std::shared_ptr<Statements::Class>& statement) -> void
                {
                    classStatement(statement);
                },
                [this](std::shared_ptr<Statements::Expression>& statement) -> void
                {
                    expressionStatement(statement);
                },
                [this](std::shared_ptr<Statements::Function>& statement) -> void
                {
                    functionStatement(statement);
                },
                [this](std::shared_ptr<Statements::If>& statement) -> void
                {
                    ifStatement(statement);
                },
                [this](std::shared_ptr<Statements::Return>& statement) -> void
                {
                    returnStatement(statement);
                },
                [this](std::shared_ptr<Statements::Variable>& statement) -> void
                {
                    variableStatement(statement);
                },
                [this](std::shared_ptr<Statements::While>& statement) -> void
                {
                    whileStatement(statement);
                },
            },
            statement);
    }

    auto Interpreter::evaluate(Expression& expression) -> Value
    {
        return std::visit(
            Overload{
                [this](std::shared_ptr<Expressions::Assignment>& assignment) -> Value
                {
                    return assignmentExpression(assignment);
                },
                [this](std::shared_ptr<Expressions::Binary>& binary) -> Value
                {
                    return binaryExpression(binary);
                },
                [this](std::shared_ptr<Expressions::Call>& call) -> Value
                {
                    return callExpression(call);
                },
                [this](std::shared_ptr<Expressions::Get>& get) -> Value
                {
                    return getExpression(get);
                },
                [this](std::shared_ptr<Expressions::Grouping>& grouping) -> Value
                {
                    return groupingExpression(grouping);
                },
                [this](std::shared_ptr<Expressions::Literal>& literal) -> Value
                {
                    return literalExpression(literal);
                },
                [this](std::shared_ptr<Expressions::Logical>& logical) -> Value
                {
                    return logicalExpression(logical);
                },
                [this](std::shared_ptr<Expressions::Set>& set) -> Value
                {
                    return setExpression(set);
                },
                [this](std::shared_ptr<Expressions::Super>& super) -> Value
                {
                    return superExpression(super);
                },
                [this](std::shared_ptr<Expressions::This>& this_) -> Value
                {
                    return thisExpression(this_);
                },
                [this](std::shared_ptr<Expressions::Unary>& unary) -> Value
                {
                    return unaryExpression(unary);
                },
                [this](std::shared_ptr<Expressions::Variable>& variable) -> Value
                {
                    return variableExpression(variable);
                },

            },
            expression);
    }

    void Interpreter::blockStatement(std::shared_ptr<Statements::Block>& statement)
    {
        executeBlock(statement->statements, std::make_shared<Environment>(_environment));
    }

    void Interpreter::executeBlock(std::vector<Statement>& statements,
                                   std::shared_ptr<Environment> environment)
    {
        std::shared_ptr<Environment> previousEnvironment = std::move(_environment);
        _environment = std::move(environment);

        auto each = [&](auto& statement) -> void { execute(statement); };
        std::ranges::for_each(statements, each);

        _environment = std::move(previousEnvironment);
    }

    void Interpreter::resolve(Expression& expression, size_t depth)
    {
        _locals[expression] = depth;
    }

    void Interpreter::classStatement(std::shared_ptr<Statements::Class>& statement)
    {
        std::shared_ptr<Types::Class> superclass = nullptr;
        if (statement->superclass)
        {
            Value superclassObject = Types::Null{};
            Expression superclassExpression = statement->superclass;
            superclassObject = evaluate(superclassExpression);
            auto* callable = std::get_if<std::shared_ptr<Types::Callable>>(&superclassObject);
            if (callable == nullptr)
            {
                throw RuntimeError(statement->superclass->name, "Superclass must be a class. (1)");
            }

            superclass = std::dynamic_pointer_cast<Types::Class>(*callable);
            if (superclass == nullptr)
            {
                throw RuntimeError(statement->superclass->name, "Superclass must be a class. (2)");
            }
        }

        _environment->define(statement->name.lexeme, Types::Null{});

        ankerl::unordered_dense::map<std::string, std::shared_ptr<Types::Function>> methods;
        for (std::shared_ptr<Statements::Function>& method : statement->methods)
        {
            auto function = std::make_shared<Types::Function>(
                method,
                _environment,
                method->possibleInitializer);
            methods[function->name()] = function;
        }

        auto klass = std::make_shared<Types::Class>(statement->name.lexeme, superclass, methods);
        _environment->assign(statement->name, klass);
    }

    void Interpreter::expressionStatement(std::shared_ptr<Statements::Expression>& statement)
    {
        evaluate(statement->expression);
    }

    void Interpreter::functionStatement(std::shared_ptr<Statements::Function>& statement)
    {
        auto function = std::make_shared<Types::Function>(statement, _environment, false);
        _environment->define(function->name(), function);
    }

    void Interpreter::ifStatement(std::shared_ptr<Statements::If>& statement)
    {
        if (evaluate(statement->condition).isTruthy())
        {
            execute(statement->thenBranch);
        }
        else if (!statementIsNullptr(statement->elseBranch))
        {
            execute(statement->elseBranch);
        }
    }

    void Interpreter::returnStatement(std::shared_ptr<Statements::Return>& statement)
    {
        Value value = Types::Null{};

        if (!expressionIsNullptr(statement->value))
        {
            value = evaluate(statement->value);
        }

        throw Return{value};
    }

    void Interpreter::variableStatement(std::shared_ptr<Statements::Variable>& statement)
    {
        Value value = Types::Null{};

        if (!expressionIsNullptr(statement->initializer))
        {
            value = evaluate(statement->initializer);
        }

        _environment->define(statement->name, value);
    }

    void Interpreter::whileStatement(std::shared_ptr<Statements::While>& statement)
    {
        while (evaluate(statement->condition).isTruthy())
        {
            execute(statement->body);
        }
    }

    auto Interpreter::assignmentExpression(std::shared_ptr<Expressions::Assignment>& assignment)
    -> Value
    {
        Value value = evaluate(assignment->value);

        if (_locals.contains(assignment))
        {
            size_t distance = _locals[assignment];
            _environment->assignAt(distance, assignment->name, value);
        }
        else
        {
            _globalEnvironment->assign(assignment->name, value);
        }

        return value;
    }

    auto Interpreter::literalExpression(std::shared_ptr<Expressions::Literal>& expression) -> Value
    {
        Value value{};

        std::visit(
            Overload{
                [&](const std::string& str) { value = str; },
                [&](const double& num) { value = num; },
                [&](const bool& val) { value = val; },
                [&](const Types::Null&) { value = Types::Null{}; },
            },
            expression->literal);

        return value;
    }

    auto Interpreter::logicalExpression(std::shared_ptr<Expressions::Logical>& expression) -> Value
    {
        Value left = evaluate(expression->left);
        if (expression->oper.type == TokenType::eOr)
        {
            if (left.isTruthy())
            {
                return std::move(left);
            }
        }
        else
        {
            if (!left.isTruthy())
            {
                return std::move(left);
            }
        }
        return evaluate(expression->right);
    }

    auto Interpreter::groupingExpression(std::shared_ptr<Expressions::Grouping>& expression)
    -> Value
    {
        return evaluate(expression->expression);
    }

    auto Interpreter::unaryExpression(std::shared_ptr<Expressions::Unary>& expression) -> Value
    {
        Value right = evaluate(expression->right);
        switch (expression->oper.type)
        {
            case TokenType::eMinus:
            {
                std::optional<double> number = right.asNumber();
                if (number.has_value())
                {
                    return -(*number);
                }
                throw RuntimeError(expression->oper, "Cannot negate a non-number");
            }
            case TokenType::eBang:
                return !right.isTruthy();
            default: ;
        }

        return {};
    }

    auto Interpreter::binaryExpression(std::shared_ptr<Expressions::Binary>& expression) -> Value
    {
        Value left = evaluate(expression->left);
        Value right = evaluate(expression->right);

        if (expression->oper.type == TokenType::ePlus)
        {
            if (left.isString() && right.isString())
            {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
        }

        if (expression->oper.type == TokenType::eBangEqual)
        {
            return left != right;
        }

        if (expression->oper.type == TokenType::eEqualEqual)
        {
            return left == right;
        }

        std::optional<double> leftNumber = left.asNumber();
        std::optional<double> rightNumber = right.asNumber();

        if (!leftNumber.has_value() || !rightNumber.has_value())
        {
            throw RuntimeError(expression->oper, "Cannot perform arithmetic on non-numbers");
        }

        double leftValue = *leftNumber;
        double rightValue = *rightNumber;

        switch (expression->oper.type)
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
            default: ;
        }

        throw RuntimeError(expression->oper, "Unknown operator");
    }

    auto Interpreter::callExpression(std::shared_ptr<Expressions::Call>& expression) -> Value
    {
        Value callee = evaluate(expression->callee);
        std::vector<Value> arguments;
        auto each = [&](auto& argument) -> void { arguments.push_back(evaluate(argument)); };
        std::ranges::for_each(expression->arguments, each);

        auto* callablePointer = std::get_if<std::shared_ptr<Types::Callable>>(&callee);
        if (callablePointer != nullptr)
        {
            std::shared_ptr<Types::Callable> callable = *callablePointer;
            size_t arity = callable->arity();
            if (arguments.size() != arity && arity != std::numeric_limits<size_t>::max())
            {
                throw RuntimeError(
                    expression->paren,
                    fmt::format(
                        "Expected {} arguments but got {}",
                        callable->arity(),
                        arguments.size()));
            }
            return callable->call(*this, arguments);
        }

        throw RuntimeError(expression->paren, "Can only call functions and classes");
    }

    auto Interpreter::variableExpression(std::shared_ptr<Expressions::Variable>& variable) -> Value
    {
        Expression expression = variable;
        return lookupVariable(variable->name, expression);
    }

    auto Interpreter::getExpression(std::shared_ptr<Expressions::Get>& expression) -> Value
    {
        Value object = evaluate(expression->object);
        auto* instance = std::get_if<std::shared_ptr<Types::Instance>>(&object);
        if (instance != nullptr)
        {
            return (*instance)->get(expression->name);
        }
        throw RuntimeError(expression->name, "Only instances have properties");
    }

    auto Interpreter::setExpression(std::shared_ptr<Expressions::Set>& expression) -> Value
    {
        Value object = evaluate(expression->object);
        auto* instance = std::get_if<std::shared_ptr<Types::Instance>>(&object);
        if (instance != nullptr)
        {
            Value value = evaluate(expression->value);
            (*instance)->set(expression->name, value);
            return value;
        }
        throw RuntimeError(expression->name, "Only instances have fields");
    }

    auto Interpreter::superExpression(std::shared_ptr<Expressions::Super>& super) -> Value
    {
        if (_locals.contains(super))
        {
            size_t distance = _locals.at(super);
            Value superClassValue = _environment->getAt(distance, "super");
            auto* superclassCallable =
                std::get_if<std::shared_ptr<Types::Callable>>(&superClassValue);
            if (superclassCallable == nullptr)
            {
                throw RuntimeError(super->keyword, "Superclass must be a class (3)");
            }

            auto superclass = std::dynamic_pointer_cast<Types::Class>(*superclassCallable);
            if (!superclass)
            {
                throw RuntimeError(super->keyword, "Superclass must be a class (4)");
            }

            Value object = _environment->getAt(distance, "this");
            auto* instance = std::get_if<std::shared_ptr<Types::Instance>>(&object);
            if (instance == nullptr)
            {
                throw RuntimeError(super->keyword, "Superclass must be a class (5)");
            }

            std::shared_ptr<Types::Function> memberFunction =
                superclass->findMemberFunction(super->method.lexeme);

            std::shared_ptr<Types::Method> method =
                std::make_shared<Types::Method>(*instance, memberFunction);
            return method;
        }

        throw RuntimeError(super->keyword, "Superclass must be a class (6)");
    }

    auto Interpreter::thisExpression(std::shared_ptr<Expressions::This>& thisExpr) -> Value
    {
        Expression expression = thisExpr;
        return lookupVariable(thisExpr->keyword, expression);
    }

    auto Interpreter::lookupVariable(Token& name, Expression& expression) -> Value
    {
        if (_locals.contains(expression))
        {
            size_t distance = _locals.at(expression);
            return _environment->getAt(distance, name);
        }
        return _globalEnvironment->get(name);
    }
} // namespace sail
