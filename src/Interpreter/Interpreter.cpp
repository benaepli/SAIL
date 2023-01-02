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
        statement->accept(*this, statement);
    }

    auto Interpreter::evaluate(std::shared_ptr<Expression>& expression) -> Value&
    {
        expression->accept(*this, expression);
        return _returnValue;
    }

    void Interpreter::resolve(const std::shared_ptr<Expression>& expression, size_t depth)
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

    void Interpreter::visitBlockStatement(Statements::Block& blockStatement,
                                          std::shared_ptr<Statement>& shared)
    {
        executeBlock(blockStatement.statements, std::make_shared<Environment>(_environment));
    }

    void Interpreter::visitClassStatement(Statements::Class& classStatement,
                                          std::shared_ptr<Statement>& shared)
    {
        std::shared_ptr<Types::Class> superclass = nullptr;
        if (classStatement.superclass != nullptr)
        {
            std::shared_ptr<Expression> superclassExpression = classStatement.superclass;
            Value value = evaluate(superclassExpression);
            auto* superclassCallable = std::get_if<std::shared_ptr<Types::Callable>>(&value);
            if (superclassCallable == nullptr) [[unlikely]]
            {
                throw RuntimeError(classStatement.superclass->name,
                                   "Superclass must be a class (1)");
            }

            superclass = std::dynamic_pointer_cast<Types::Class>(*superclassCallable);
            if (!superclass) [[unlikely]]
            {
                throw RuntimeError(classStatement.superclass->name,
                                   "Superclass must be a class (2)");
            }
        }

        _environment->define(classStatement.name, Types::Null {});

        ankerl::unordered_dense::map<std::string, std::shared_ptr<Types::Function>> methods;
        for (std::shared_ptr<Statements::Function>& method : classStatement.methods)
        {
            auto function = std::make_shared<Types::Function>(
                method, _environment, method->possibleInitializer);
            methods[function->name()] = function;
        }

        auto klass =
            std::make_shared<Types::Class>(classStatement.name.lexeme, superclass, methods);
        _environment->assign(classStatement.name, klass);
    }

    void Interpreter::visitExpressionStatement(Statements::Expression& expressionStatement,
                                               std::shared_ptr<Statement>& shared)
    {
        evaluate(expressionStatement.expression);
    }

    void Interpreter::visitFunctionStatement(Statements::Function& functionStatement,
                                             std::shared_ptr<Statement>& shared)
    {
        auto functionStatementPointer = std::dynamic_pointer_cast<Statements::Function>(shared);
        std::shared_ptr<Types::Function> function =
            std::make_shared<Types::Function>(functionStatementPointer, _environment, false);
        _environment->define(functionStatement.name, function);
    }

    void Interpreter::visitIfStatement(Statements::If& ifStatement,
                                       std::shared_ptr<Statement>& shared)
    {
        if (evaluate(ifStatement.condition).isTruthy())
        {
            execute(ifStatement.thenBranch);
        }
        else if (ifStatement.elseBranch != nullptr)
        {
            execute(ifStatement.elseBranch);
        }
    }

    void Interpreter::visitReturnStatement(Statements::Return& returnStatement,
                                           std::shared_ptr<Statement>& shared)
    {
        Value value = Types::Null {};
        if (returnStatement.value != nullptr)
        {
            value = evaluate(returnStatement.value);
        }

        throw Return(value);
    }

    void Interpreter::visitVariableStatement(Statements::Variable& variableStatement,
                                             std::shared_ptr<Statement>& shared)
    {
        Value value = Types::Null {};
        if (variableStatement.initializer != nullptr) [[likely]]
        {
            value = evaluate(variableStatement.initializer);
        }

        _environment->define(variableStatement.name, value);
    }

    void Interpreter::visitWhileStatement(Statements::While& whileStatement,
                                          std::shared_ptr<Statement>& shared)
    {
        while (evaluate(whileStatement.condition).isTruthy())
        {
            execute(whileStatement.body);
        }
    }

    void Interpreter::visitAssignmentExpression(Expressions::Assignment& assignmentExpression,
                                                std::shared_ptr<Expression>& shared)
    {
        Value value = evaluate(assignmentExpression.value);

        auto it = _locals.find(shared);
        if (it != _locals.end()) [[likely]]
        {
            _environment->assignAt(it->second, assignmentExpression.name, value);
        }
        else
        {
            _globalEnvironment->assign(assignmentExpression.name, value);
        }

        _returnValue = std::move(value);
    }

    void Interpreter::visitBinaryExpression(Expressions::Binary& binaryExpression,
                                            std::shared_ptr<Expression>& shared)
    {
        Value left = evaluate(binaryExpression.left);
        Value right = evaluate(binaryExpression.right);

        if (binaryExpression.op.type == TokenType::ePlus)
        {
            if (left.isString() && right.isString())
            {
                _returnValue = std::get<std::string>(left) + std::get<std::string>(right);
                return;
            }
        }

        if (binaryExpression.op.type == TokenType::eBangEqual)
        {
            _returnValue = left != right;
            return;
        }

        if (binaryExpression.op.type == TokenType::eEqualEqual)
        {
            _returnValue = left == right;
            return;
        }

        std::optional<double> leftNumber = left.asNumber();
        std::optional<double> rightNumber = right.asNumber();

        if (!leftNumber.has_value() || !rightNumber.has_value()) [[unlikely]]
        {
            throw RuntimeError(binaryExpression.op, "Cannot perform arithmetic on non-numbers");
        }

        double leftValue = *leftNumber;
        double rightValue = *rightNumber;

        switch (binaryExpression.op.type)
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
            default:
                [[unlikely]] break;
        }

        throw RuntimeError(binaryExpression.op, "Unknown operator");
    }

    void Interpreter::visitCallExpression(Expressions::Call& callExpression,
                                          std::shared_ptr<Expression>& shared)
    {
        Value callee = evaluate(callExpression.callee);

        std::vector<Value> arguments;
        for (auto& argument : callExpression.arguments)
        {
            arguments.push_back(evaluate(argument));
        }

        auto* callablePointer = std::get_if<std::shared_ptr<Types::Callable>>(&callee);
        if (callablePointer == nullptr) [[unlikely]]
        {
            throw RuntimeError(callExpression.paren, "Can only call functions and classes");
        }

        auto callable = *callablePointer;
        if (callable == nullptr) [[unlikely]]
        {
            throw RuntimeError(callExpression.paren, "Can only call functions and classes");
        }

        size_t arity = callable->arity();
        if (arguments.size() != arity && arity != std::numeric_limits<size_t>::max()) [[unlikely]]
        {
            throw RuntimeError(
                callExpression.paren,
                fmt::format(
                    "Expected {} arguments but got {}", callable->arity(), arguments.size()));
        }

        _returnValue = callable->call(*this, arguments);
    }

    void Interpreter::visitGetExpression(Expressions::Get& getExpression,
                                         std::shared_ptr<Expression>& shared)
    {
        Value object = evaluate(getExpression.object);
        auto* instance = std::get_if<std::shared_ptr<Types::Instance>>(&object);
        if (instance == nullptr) [[unlikely]]
        {
            throw RuntimeError(getExpression.name, "Only instances have properties");
        }

        _returnValue = (*instance)->get(getExpression.name);
    }

    void Interpreter::visitGroupingExpression(Expressions::Grouping& groupingExpression,
                                              std::shared_ptr<Expression>& shared)
    {
        _returnValue = evaluate(groupingExpression.expression);
    }

    void Interpreter::visitLiteralExpression(Expressions::Literal& literalExpression,
                                             std::shared_ptr<Expression>& shared)
    {
        Value value {};

        std::visit(
            Overload {
                [&](const std::string& str) { value = str; },
                [&](const double& num) { value = num; },
                [&](const bool& val) { value = val; },
                [&](const Types::Null&) { value = Types::Null {}; },
            },
            literalExpression.literal);

        _returnValue = std::move(value);
    }

    void Interpreter::visitLogicalExpression(Expressions::Logical& logicalExpression,
                                             std::shared_ptr<Expression>& shared)
    {
        Value left = evaluate(logicalExpression.left);

        if (logicalExpression.op.type == TokenType::eOr)
        {
            if (left.isTruthy())
            {
                _returnValue = std::move(left);
                return;
            }
        }
        else
        {
            if (!left.isTruthy())
            {
                _returnValue = std::move(left);
                return;
            }
        }

        _returnValue = evaluate(logicalExpression.right);
    }

    void Interpreter::visitSetExpression(Expressions::Set& setExpression,
                                         std::shared_ptr<Expression>& shared)
    {
        Value object = evaluate(setExpression.object);

        auto* instance = std::get_if<std::shared_ptr<Types::Instance>>(&object);
        if (instance == nullptr) [[unlikely]]
        {
            throw RuntimeError(setExpression.name, "Only instances have fields");
        }

        Value value = evaluate(setExpression.value);
        (*instance)->set(setExpression.name, value);
        _returnValue = std::move(value);
    }

    void Interpreter::visitSuperExpression(Expressions::Super& superExpression,
                                           std::shared_ptr<Expression>& shared)
    {
        size_t distance = _locals[shared];
        Value superclassValue = _environment->getAt(distance, "super");
        auto* superclassCallable = std::get_if<std::shared_ptr<Types::Callable>>(&superclassValue);
        if (superclassCallable == nullptr) [[unlikely]]
        {
            throw RuntimeError(superExpression.keyword, "Superclass must be a class");
        }

        auto superclass = std::dynamic_pointer_cast<Types::Class>(*superclassCallable);
        if (superclass == nullptr) [[unlikely]]
        {
            throw RuntimeError(superExpression.keyword, "Superclass must be a class");
        }

        Value objectValue = _environment->getAt(distance, "this");
        auto* objectInstance = std::get_if<std::shared_ptr<Types::Instance>>(&objectValue);
        if (objectInstance == nullptr) [[unlikely]]
        {
            throw RuntimeError(superExpression.keyword, "Superclass must be a class");
        }

        std::shared_ptr<Types::Callable> method =
            superclass->findMemberFunction(superExpression.method.lexeme);
        if (method == nullptr) [[unlikely]]
        {
            throw RuntimeError(superExpression.method, "Undefined property");
        }

        _returnValue = method;
    }

    void Interpreter::visitThisExpression(Expressions::This& thisExpression,
                                          std::shared_ptr<Expression>& shared)
    {
        _returnValue = lookupVariable(thisExpression.keyword, shared);
    }

    void Interpreter::visitUnaryExpression(Expressions::Unary& unaryExpression,
                                           std::shared_ptr<Expression>& shared)
    {
        Value right = evaluate(unaryExpression.right);
        switch (unaryExpression.op.type)
        {
            case TokenType::eMinus:
            {
                std::optional<double> number = right.asNumber();
                if (number.has_value()) [[likely]]
                {
                    _returnValue = -*number;
                    return;
                }
                throw RuntimeError(unaryExpression.op, "Cannot negate a non-number");
            }
            case TokenType::eBang:
            {
                _returnValue = !right.isTruthy();
                return;
            }
            default:
                [[unlikely]];
        }

        _returnValue = Types::Null {};
    }

    void Interpreter::visitVariableExpression(Expressions::Variable& variableExpression,
                                              std::shared_ptr<Expression>& shared)
    {
        _returnValue = lookupVariable(variableExpression.name, shared);
    }

    auto Interpreter::lookupVariable(const Token& name,
                                     const std::shared_ptr<Expression>& expression) -> Value
    {
        auto it = _locals.find(expression);
        if (it != _locals.end()) [[likely]]
        {
            return _environment->getAt(it->second, name.lexeme);
        }
        return _globalEnvironment->get(name);
    }
}  // namespace sail
