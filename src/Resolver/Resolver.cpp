#include "Resolver/Resolver.h"

#include <fmt/format.h>

#include "Errors/RuntimeError.h"
#include "Expressions/Expressions.h"
#include "Interpreter/Interpreter.h"
#include "magic_enum.hpp"
#include "utils/Overload.h"

namespace sail
{
    using namespace magic_enum::bitwise_operators;

    Resolver::Resolver(Interpreter& interpreter)
        : _interpreter(interpreter)
    {
    }

    void Resolver::resolve(std::vector<std::shared_ptr<Statement>>& statements)
    {
        for (auto& statement : statements)
        {
            resolve(statement);
        }
    }

    void Resolver::resolve(std::shared_ptr<Statement>& statement)
    {
        statement->accept(*this, statement);
    }

    void Resolver::resolve(std::shared_ptr<Expression>& expression)
    {
        expression->accept(*this, expression);
    }

    void Resolver::visitBlockStatement(Statements::Block& blockStatement,
                                       std::shared_ptr<Statement>& shared)
    {
        beginScope();
        resolve(blockStatement.statements);
        endScope();
    }

    void Resolver::visitClassStatement(Statements::Class& classStatement,
                                       std::shared_ptr<Statement>& shared)
    {
        ClassType enclosingClass = _currentClass;
        _currentClass = ClassType::eClass;

        declare(classStatement.name);
        define(classStatement.name);

        if (classStatement.superclass != nullptr)
        {
            if (classStatement.name.lexeme == classStatement.superclass->name.lexeme)
            {
                throw RuntimeError(classStatement.superclass->name,
                                   "A class cannot inherit from itself");
            }

            std::shared_ptr<Expression> superclass = classStatement.superclass;
            resolve(superclass);

            _currentClass |= ClassType::eSubclass;
        }

        beginScope();

        for (std::shared_ptr<Statements::Function>& method : classStatement.methods)
        {
            FunctionType functionType = FunctionType::eMethod;
            if (method->possibleInitializer)
            {
                functionType = FunctionType::eInitializer;
            }

            resolveFunction(*method, functionType);
        }

        endScope();

        _currentClass = enclosingClass;
    }

    void Resolver::visitExpressionStatement(Statements::Expression& expressionStatement,
                                            std::shared_ptr<Statement>& shared)
    {
        resolve(expressionStatement.expression);
    }

    void Resolver::visitFunctionStatement(Statements::Function& functionStatement,
                                          std::shared_ptr<Statement>& shared)
    {
        declare(functionStatement.name);
        define(functionStatement.name);

        resolveFunction(functionStatement, FunctionType::eFunction);
    }

    void Resolver::visitIfStatement(Statements::If& ifStatement, std::shared_ptr<Statement>& shared)
    {
        resolve(ifStatement.condition);
        resolve(ifStatement.thenBranch);
        if (ifStatement.elseBranch != nullptr)
        {
            resolve(ifStatement.elseBranch);
        }
    }

    void Resolver::visitReturnStatement(Statements::Return& returnStatement,
                                        std::shared_ptr<Statement>& shared)
    {
        if (_currentFunction == FunctionType::eNone)
        {
            throw RuntimeError(returnStatement.keyword, "Cannot return from top-level code.");
        }

        if (returnStatement.value != nullptr)
        {
            if (_currentFunction == FunctionType::eInitializer)
            {
                throw RuntimeError(returnStatement.keyword,
                                   "Cannot return a value from an initializer.");
            }

            resolve(returnStatement.value);
        }
    }

    void Resolver::visitWhileStatement(Statements::While& whileStatement,
                                       std::shared_ptr<Statement>& shared)
    {
        resolve(whileStatement.condition);
        resolve(whileStatement.body);
    }

    void Resolver::visitVariableStatement(Statements::Variable& variableStatement,
                                          std::shared_ptr<Statement>& shared)
    {
        declare(variableStatement.name);
        if (variableStatement.initializer != nullptr)
        {
            resolve(variableStatement.initializer);
        }
        define(variableStatement.name);
    }

    void Resolver::visitAssignmentExpression(Expressions::Assignment& assignmentExpression,
                                             std::shared_ptr<Expression>& shared)
    {
        resolve(assignmentExpression.value);
        resolveLocal(shared, assignmentExpression.name);
    }

    void Resolver::visitBinaryExpression(Expressions::Binary& binaryExpression,
                                         std::shared_ptr<Expression>& shared)
    {
        resolve(binaryExpression.left);
        resolve(binaryExpression.right);
    }

    void Resolver::visitCallExpression(Expressions::Call& callExpression,
                                       std::shared_ptr<Expression>& shared)
    {
        resolve(callExpression.callee);

        for (auto& argument : callExpression.arguments)
        {
            resolve(argument);
        }
    }

    void Resolver::visitGetExpression(Expressions::Get& getExpression,
                                      std::shared_ptr<Expression>& shared)
    {
        resolve(getExpression.object);
    }

    void Resolver::visitGroupingExpression(Expressions::Grouping& groupingExpression,
                                           std::shared_ptr<Expression>& shared)
    {
        resolve(groupingExpression.expression);
    }

    void Resolver::visitLiteralExpression(Expressions::Literal& literalExpression,
                                          std::shared_ptr<Expression>& shared)
    {
    }

    void Resolver::visitLogicalExpression(Expressions::Logical& logicalExpression,
                                          std::shared_ptr<Expression>& shared)
    {
        resolve(logicalExpression.left);
        resolve(logicalExpression.right);
    }

    void Resolver::visitSetExpression(Expressions::Set& setExpression,
                                      std::shared_ptr<Expression>& shared)
    {
        resolve(setExpression.value);
        resolve(setExpression.object);
    }

    void Resolver::visitSuperExpression(Expressions::Super& superExpression,
                                        std::shared_ptr<Expression>& shared)
    {
        const bool isInClass = static_cast<bool>(_currentClass & ClassType::eClass);
        if (!isInClass)
        {
            throw RuntimeError(superExpression.keyword, "Cannot use 'super' outside of a class.");
        }

        const bool isInSubclass = static_cast<bool>(_currentClass & ClassType::eSubclass);
        if (!isInSubclass)
        {
            throw RuntimeError(superExpression.keyword,
                               "Cannot use 'super' in a class with no superclass.");
        }

        resolveLocal(shared, superExpression.keyword);
    }

    void Resolver::visitThisExpression(Expressions::This& thisExpression,
                                       std::shared_ptr<Expression>& shared)
    {
        const bool isInClass = static_cast<bool>(_currentClass & ClassType::eClass);
        if (!isInClass)
        {
            throw RuntimeError(thisExpression.keyword, "Cannot use 'this' outside of a class.");
        }

        resolveLocal(shared, thisExpression.keyword);
    }

    void Resolver::visitUnaryExpression(Expressions::Unary& unaryExpression,
                                        std::shared_ptr<Expression>& shared)
    {
        resolve(unaryExpression.right);
    }

    void Resolver::visitVariableExpression(Expressions::Variable& variableExpression,
                                           std::shared_ptr<Expression>& shared)
    {
        if (!_scopes.empty() && _scopes.back().contains(variableExpression.name.lexeme)
            && !_scopes.back()[variableExpression.name.lexeme])
        {
            throw RuntimeError(variableExpression.name,
                               "Cannot read local variable in its own initializer.");
        }

        resolveLocal(shared, variableExpression.name);
    }

    void Resolver::beginScope()
    {
        _scopes.emplace_back();
    }

    void Resolver::endScope()
    {
        _scopes.pop_back();
    }

    void Resolver::declare(const Token& name)
    {
        if (_scopes.empty())
        {
            return;
        }
        auto& scope = _scopes.back();
        if (scope.contains(name.lexeme))
        {
            throw RuntimeError(
                name,
                fmt::format("Variable with name '{}' already declared in this scope.",
                            name.lexeme));
        }
        scope.emplace(name.lexeme, false);
    }

    void Resolver::define(const Token& name)
    {
        if (_scopes.empty())
        {
            return;
        }
        _scopes.back()[name.lexeme] = true;
    }

    void Resolver::resolveLocal(std::shared_ptr<Expression>& expression, const Token& name)
    {
        for (int64_t i = static_cast<int64_t>(_scopes.size()) - 1; i >= 0; i--)
        {
            if (_scopes[i].contains(name.lexeme))
            {
                _interpreter.resolve(expression, _scopes.size() - 1 - i);
                return;
            }
        }
    }

    void Resolver::resolveFunction(Statements::Function& function, FunctionType type)
    {
        FunctionType enclosingFunction = _currentFunction;
        _currentFunction = type;

        beginScope();
        for (Token& param : function.parameters)
        {
            declare(param);
            define(param);
        }

        if (type == FunctionType::eMethod || type == FunctionType::eInitializer)
        {
            _scopes.back().emplace("this", true);

            const bool isSubclass = static_cast<bool>(_currentClass & ClassType::eSubclass);
            if (isSubclass)
            {
                _scopes.back().emplace("super", true);
            }
        }

        resolve(function.body);
        endScope();

        _currentFunction = enclosingFunction;
    }
}  // namespace sail
