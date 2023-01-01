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

    void Interpreter::interpret(std::vector<std::shared_ptr<Statement>>& statements)
    {
        auto each = [&](auto& statement) -> void { execute(statement); };
        std::ranges::for_each(statements, each);
    }

    void Interpreter::execute(std::shared_ptr<Statement>& statement)
    {
        statement->accept(*this);
    }

    auto Interpreter::evaluate(std::shared_ptr<Expression>& expression) -> Value&
    {
        expression->accept(*this);
        return _returnValue;
    }

    void Interpreter::resolve(std::shared_ptr<Expression>& expression, size_t depth)
    {
        _locals[expression] = depth;
    }

    void Interpreter::executeBlock(std::vector<std::shared_ptr<Statement>>& statements,
                                   std::shared_ptr<Environment> environment)
    {
        std::shared_ptr<Environment> previousEnvironment = std::move(_environment);
        _environment = std::move(environment);

        auto each = [&](auto& statement) -> void { execute(statement); };
        std::ranges::for_each(statements, each);

        _environment = std::move(previousEnvironment);
    }

    void Interpreter::visitBlockStatement(std::shared_ptr<Statements::Block>& statement)
    {
        executeBlock(statement->statements, std::make_shared<Environment>(_environment));
    }

    void Interpreter::visitClassStatement(std::shared_ptr<Statements::Class>& statement)
    {
        std::shared_ptr<Types::Class> superclass = nullptr;
        if (statement->superclass)
        {
            Value superclassObject = Types::Null {};
            std::shared_ptr<Expression> superclassExpression = statement->superclass;
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

        _environment->define(statement->name.lexeme, Types::Null {});

        ankerl::unordered_dense::map<std::string, std::shared_ptr<Types::Function>> methods;
        for (std::shared_ptr<Statements::Function>& method : statement->methods)
        {
            auto function = std::make_shared<Types::Function>(
                method, _environment, method->possibleInitializer);
            methods[function->name()] = function;
        }

        auto klass = std::make_shared<Types::Class>(statement->name.lexeme, superclass, methods);
        _environment->assign(statement->name, klass);
    }

    void Interpreter::visitExpressionStatement(std::shared_ptr<Statements::Expression>& statement)
    {
        evaluate(statement->expression);
    }

    void Interpreter::visitFunctionStatement(std::shared_ptr<Statements::Function>& statement)
    {
        auto function = std::make_shared<Types::Function>(statement, _environment, false);
        _environment->define(function->name(), function);
    }

    void Interpreter::visitIfStatement(std::shared_ptr<Statements::If>& statement)
    {
        if (evaluate(statement->condition).isTruthy())
        {
            execute(statement->thenBranch);
        }
        else if (statement->elseBranch != nullptr)
        {
            execute(statement->elseBranch);
        }
    }

    void Interpreter::visitReturnStatement(std::shared_ptr<Statements::Return>& statement)
    {
        Value value = Types::Null {};

        if (statement->value != nullptr)
        {
            value = evaluate(statement->value);
        }

        throw Return {value};
    }

    void Interpreter::visitVariableStatement(std::shared_ptr<Statements::Variable>& statement)
    {
        Value value = Types::Null {};

        if (statement->initializer != nullptr)
        {
            value = evaluate(statement->initializer);
        }

        _environment->define(statement->name, value);
    }

    void Interpreter::visitWhileStatement(std::shared_ptr<Statements::While>& statement)
    {
        while (evaluate(statement->condition).isTruthy())
        {
            execute(statement->body);
        }
    }

    void Interpreter::visitAssignmentExpression(
        std::shared_ptr<Expressions::Assignment>& assignment)

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

        _returnValue = value;
    }

    void Interpreter::visitBinaryExpression(std::shared_ptr<Expressions::Binary>& expression)
    {
        Value left = evaluate(expression->left);
        Value right = evaluate(expression->right);

        if (expression->op.type == TokenType::ePlus)
        {
            if (left.isString() && right.isString())
            {
                _returnValue = std::get<std::string>(left) + std::get<std::string>(right);
                return;
            }
        }

        if (expression->op.type == TokenType::eBangEqual)
        {
            _returnValue = left != right;
            return;
        }

        if (expression->op.type == TokenType::eEqualEqual)
        {
            _returnValue = left == right;
            return;
        }

        std::optional<double> leftNumber = left.asNumber();
        std::optional<double> rightNumber = right.asNumber();

        if (!leftNumber.has_value() || !rightNumber.has_value())
        {
            throw RuntimeError(expression->op, "Cannot perform arithmetic on non-numbers");
        }

        double leftValue = *leftNumber;
        double rightValue = *rightNumber;

        switch (expression->op.type)
        {
            case TokenType::eMinus:
                _returnValue = leftValue - rightValue;
                return;
            case TokenType::eSlash:
                _returnValue = leftValue / rightValue;
                return;
            case TokenType::eStar:
                _returnValue = leftValue * rightValue;
                return;
            case TokenType::ePlus:
                _returnValue = leftValue + rightValue;
                return;
            case TokenType::eGreater:
                _returnValue = leftValue > rightValue;
                return;
            case TokenType::eGreaterEqual:
                _returnValue = leftValue >= rightValue;
                return;
            case TokenType::eLess:
                _returnValue = leftValue < rightValue;
                return;
            case TokenType::eLessEqual:
                _returnValue = leftValue <= rightValue;
                return;
            default:;
        }

        throw RuntimeError(expression->op, "Unknown operator");
    }

    void Interpreter::visitCallExpression(std::shared_ptr<Expressions::Call>& expression)
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
                        "Expected {} arguments but got {}", callable->arity(), arguments.size()));
            }
            _returnValue = callable->call(*this, arguments);
            return;
        }

        throw RuntimeError(expression->paren, "Can only call functions and classes");
    }

    void Interpreter::visitGetExpression(std::shared_ptr<Expressions::Get>& expression)
    {
        Value object = evaluate(expression->object);
        auto* instance = std::get_if<std::shared_ptr<Types::Instance>>(&object);
        if (instance != nullptr)
        {
            _returnValue = (*instance)->get(expression->name);
            return;
        }
        throw RuntimeError(expression->name, "Only instances have properties");
    }

    void Interpreter::visitGroupingExpression(std::shared_ptr<Expressions::Grouping>& expression)

    {
        _returnValue = evaluate(expression->expression);
    }

    void Interpreter::visitLiteralExpression(std::shared_ptr<Expressions::Literal>& expression)
    {
        Value value {};

        std::visit(
            Overload {
                [&](const std::string& str) { value = str; },
                [&](const double& num) { value = num; },
                [&](const bool& val) { value = val; },
                [&](const Types::Null&) { value = Types::Null {}; },
            },
            expression->literal);

        _returnValue = value;
    }

    void Interpreter::visitLogicalExpression(std::shared_ptr<Expressions::Logical>& expression)
    {
        Value left = evaluate(expression->left);
        if (expression->op.type == TokenType::eOr)
        {
            if (left.isTruthy())
            {
                _returnValue = left;
                return;
            }
        }
        else
        {
            if (!left.isTruthy())
            {
                _returnValue = left;
                return;
            }
        }
        _returnValue = evaluate(expression->right);
    }

    void Interpreter::visitSetExpression(std::shared_ptr<Expressions::Set>& expression)
    {
        Value object = evaluate(expression->object);
        auto* instance = std::get_if<std::shared_ptr<Types::Instance>>(&object);
        if (instance != nullptr)
        {
            Value value = evaluate(expression->value);
            (*instance)->set(expression->name, value);
            _returnValue = value;
            return;
        }
        throw RuntimeError(expression->name, "Only instances have fields");
    }

    void Interpreter::visitSuperExpression(std::shared_ptr<Expressions::Super>& super)
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
            _returnValue = method;
            return;
        }

        throw RuntimeError(super->keyword, "Superclass must be a class (6)");
    }

    void Interpreter::visitThisExpression(std::shared_ptr<Expressions::This>& thisExpr)
    {
        std::shared_ptr<Expression> expression = thisExpr;
        _returnValue = lookupVariable(thisExpr->keyword, expression);
    }

    void Interpreter::visitUnaryExpression(std::shared_ptr<Expressions::Unary>& expression)
    {
        Value right = evaluate(expression->right);
        switch (expression->op.type)
        {
            case TokenType::eMinus:
            {
                std::optional<double> number = right.asNumber();
                if (number.has_value())
                {
                    _returnValue = -*number;
                    return;
                }
                throw RuntimeError(expression->op, "Cannot negate a non-number");
            }
            case TokenType::eBang:
            {
                _returnValue = !right.isTruthy();
                return;
            }
            default:;
        }

        _returnValue = Types::Null {};
    }

    void Interpreter::visitVariableExpression(std::shared_ptr<Expressions::Variable>& variable)
    {
        std::shared_ptr<Expression> expression = variable;
        _returnValue = lookupVariable(variable->name, expression);
    }

    auto Interpreter::lookupVariable(Token& name, std::shared_ptr<Expression>& expression) -> Value
    {
        if (_locals.contains(expression))
        {
            size_t distance = _locals.at(expression);
            return _environment->getAt(distance, name);
        }
        return _globalEnvironment->get(name);
    }
}  // namespace sail
