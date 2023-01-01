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
        statement->accept(*this);
    }

    void Resolver::resolve(std::shared_ptr<Expression>& expression)
    {
        expression->accept(*this);
    }

    void Resolver::visitBlockStatement(std::shared_ptr<Statements::Block>& block)
    {
        beginScope();
        resolve(block->statements);
        endScope();
    }

    void Resolver::visitClassStatement(std::shared_ptr<Statements::Class>& classStatement)
    {
        ClassType enclosingClass = _currentClass;
        _currentClass = ClassType::eClass;

        declare(classStatement->name);
        define(classStatement->name);

        if (classStatement->superclass != nullptr)
        {
            if (classStatement->name.lexeme == classStatement->superclass->name.lexeme)
            {
                throw RuntimeError(classStatement->superclass->name,
                                   "A class cannot inherit from itself");
            }

            std::shared_ptr<Expression> superclass = classStatement->superclass;
            resolve(superclass);

            _currentClass |= ClassType::eSubclass;
        }

        beginScope();

        for (std::shared_ptr<Statements::Function>& method : classStatement->methods)
        {
            FunctionType functionType = FunctionType::eMethod;
            if (method->possibleInitializer)
            {
                functionType = FunctionType::eInitializer;
            }

            resolveFunction(method, functionType);
        }

        endScope();

        _currentClass = enclosingClass;
    }

    void Resolver::visitExpressionStatement(std::shared_ptr<Statements::Expression>& expression)
    {
        resolve(expression->expression);
    }

    void Resolver::visitFunctionStatement(std::shared_ptr<Statements::Function>& function)
    {
        declare(function->name);
        define(function->name);
        resolveFunction(function, FunctionType::eFunction);
    }

    void Resolver::visitIfStatement(std::shared_ptr<Statements::If>& ifStatement)
    {
        resolve(ifStatement->condition);
        resolve(ifStatement->thenBranch);
        if (ifStatement->elseBranch != nullptr)
        {
            resolve(ifStatement->elseBranch);
        }
    }

    void Resolver::visitReturnStatement(std::shared_ptr<Statements::Return>& returnStatement)
    {
        if (_currentFunction == FunctionType::eNone)
        {
            throw RuntimeError(returnStatement->keyword, "Cannot return from top-level code.");
        }

        if (returnStatement->value != nullptr)
        {
            if (_currentFunction == FunctionType::eInitializer)
            {
                throw RuntimeError(returnStatement->keyword,
                                   "Cannot return a value from an initializer.");
            }

            resolve(returnStatement->value);
        }
    }

    void Resolver::visitWhileStatement(std::shared_ptr<Statements::While>& whileStatement)
    {
        resolve(whileStatement->condition);
        resolve(whileStatement->body);
    }

    void Resolver::visitVariableStatement(std::shared_ptr<Statements::Variable>& variable)
    {
        declare(variable->name);
        if (variable->initializer != nullptr)
        {
            resolve(variable->initializer);
        }
        define(variable->name);
    }

    void Resolver::visitAssignmentExpression(std::shared_ptr<Expressions::Assignment>& assignment)
    {
        resolve(assignment->value);

        std::shared_ptr<Expression> expression = assignment;
        resolveLocal(expression, assignment->name);
    }

    void Resolver::visitBinaryExpression(std::shared_ptr<Expressions::Binary>& binary)
    {
        resolve(binary->left);
        resolve(binary->right);
    }

    void Resolver::visitCallExpression(std::shared_ptr<Expressions::Call>& call)
    {
        resolve(call->callee);
        for (auto& argument : call->arguments)
        {
            resolve(argument);
        }
    }

    void Resolver::visitGetExpression(std::shared_ptr<Expressions::Get>& get)
    {
        resolve(get->object);
    }

    void Resolver::visitGroupingExpression(std::shared_ptr<Expressions::Grouping>& grouping)
    {
        resolve(grouping->expression);
    }

    void Resolver::visitLiteralExpression(std::shared_ptr<Expressions::Literal>& literal)
    {
        // Do nothing
    }

    void Resolver::visitLogicalExpression(std::shared_ptr<Expressions::Logical>& logical)
    {
        resolve(logical->left);
        resolve(logical->right);
    }

    void Resolver::visitSetExpression(std::shared_ptr<Expressions::Set>& set)
    {
        resolve(set->value);
        resolve(set->object);
    }

    void Resolver::visitSuperExpression(std::shared_ptr<Expressions::Super>& super)
    {
        const bool isInClass = static_cast<bool>(_currentClass & ClassType::eClass);
        if (!isInClass)
        {
            throw RuntimeError(super->keyword, "Cannot use 'super' outside of a class.");
        }

        const bool isInSubclass = static_cast<bool>(_currentClass & ClassType::eSubclass);
        if (!isInSubclass)
        {
            throw RuntimeError(super->keyword, "Cannot use 'super' in a class with no superclass.");
        }

        std::shared_ptr<Expression> expression = super;
        resolveLocal(expression, super->keyword);
    }

    void Resolver::visitThisExpression(std::shared_ptr<Expressions::This>& thisExpr)
    {
        const bool isInClass = static_cast<bool>(_currentClass & ClassType::eClass);
        if (!isInClass)
        {
            throw RuntimeError(thisExpr->keyword, "Cannot use 'this' outside of a class.");
        }

        std::shared_ptr<Expression> expression = thisExpr;
        resolveLocal(expression, thisExpr->keyword);
    }

    void Resolver::visitUnaryExpression(std::shared_ptr<Expressions::Unary>& unary)
    {
        resolve(unary->right);
    }

    void Resolver::visitVariableExpression(std::shared_ptr<Expressions::Variable>& variable)
    {
        if (!_scopes.empty() && _scopes.back().contains(variable->name.lexeme)
            && !_scopes.back()[variable->name.lexeme])
        {
            throw RuntimeError(variable->name,
                               "Cannot read local variable in its own initializer.");
        }

        std::shared_ptr<Expression> expression = variable;
        resolveLocal(expression, variable->name);
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

    void Resolver::resolveFunction(std::shared_ptr<Statements::Function>& function,
                                   FunctionType type)
    {
        FunctionType enclosingFunction = _currentFunction;
        _currentFunction = type;

        beginScope();
        for (Token& param : function->parameters)
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

        resolve(function->body);
        endScope();

        _currentFunction = enclosingFunction;
    }
}  // namespace sail
