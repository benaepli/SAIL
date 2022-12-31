#include "Resolver/Resolver.h"

#include <fmt/format.h>

#include "Errors/RuntimeError.h"
#include "Expressions/Expressions.h"
#include "Interpreter/Interpreter.h"
#include "utils/Overload.h"

namespace sail
{
    Resolver::Resolver(Interpreter& interpreter)
        : _interpreter(interpreter)
    {
    }

    void Resolver::resolve(std::vector<Statement>& statements)
    {
        for (auto& statement : statements)
        {
            resolve(statement);
        }
    }

    void Resolver::resolve(Statement& statement)
    {
        std::visit(
            Overload {
                [this](std::shared_ptr<Statements::Block>& statement) -> void
                { blockStatement(statement); },
                [this](std::shared_ptr<Statements::Class>& statement) -> void
                { classStatement(statement); },
                [this](std::shared_ptr<Statements::Expression>& statement) -> void
                { expressionStatement(statement); },
                [this](std::shared_ptr<Statements::Function>& statement) -> void
                { functionStatement(statement); },
                [this](std::shared_ptr<Statements::If>& statement) -> void
                { ifStatement(statement); },
                [this](std::shared_ptr<Statements::Return>& statement) -> void
                { returnStatement(statement); },
                [this](std::shared_ptr<Statements::Variable>& statement) -> void
                { variableStatement(statement); },
                [this](std::shared_ptr<Statements::While>& statement) -> void
                { whileStatement(statement); },
            },
            statement);
    }

    void Resolver::resolve(Expression& expression)
    {
        std::visit(
            Overload {
                [this](std::shared_ptr<Expressions::Assignment>& expression) -> void
                { assignmentExpression(expression); },
                [this](std::shared_ptr<Expressions::Binary>& expression) -> void
                { binaryExpression(expression); },
                [this](std::shared_ptr<Expressions::Call>& expression) -> void
                { callExpression(expression); },
                [this](std::shared_ptr<Expressions::Get>& expression) -> void
                { getExpression(expression); },
                [this](std::shared_ptr<Expressions::Grouping>& expression) -> void
                { groupingExpression(expression); },
                [this](std::shared_ptr<Expressions::Literal>& expression) -> void
                { literalExpression(expression); },
                [this](std::shared_ptr<Expressions::Logical>& expression) -> void
                { logicalExpression(expression); },
                [this](std::shared_ptr<Expressions::Set>& expression) -> void
                { setExpression(expression); },
                [this](std::shared_ptr<Expressions::This>& expression) -> void
                { thisExpression(expression); },
                [this](std::shared_ptr<Expressions::Unary>& expression) -> void
                { unaryExpression(expression); },
                [this](std::shared_ptr<Expressions::Variable>& expression) -> void
                { variableExpression(expression); },
            },
            expression);
    }

    void Resolver::blockStatement(std::shared_ptr<Statements::Block>& block)
    {
        beginScope();
        resolve(block->statements);
        endScope();
    }

    void Resolver::classStatement(std::shared_ptr<Statements::Class>& classStatement)
    {
        ClassType enclosingClass = _currentClass;
        _currentClass = ClassType::eClass;

        declare(classStatement->name);
        define(classStatement->name);

        beginScope();
        _scopes.back().emplace("this", true);

        for (std::shared_ptr<Statements::Function>& method : classStatement->methods)
        {
            FunctionType functionType = FunctionType::eMethod;
            resolveFunction(method, functionType);
        }

        endScope();

        _currentClass = enclosingClass;
    }

    void Resolver::expressionStatement(std::shared_ptr<Statements::Expression>& expression)
    {
        resolve(expression->expression);
    }

    void Resolver::functionStatement(std::shared_ptr<Statements::Function>& function)
    {
        declare(function->name);
        define(function->name);
        resolveFunction(function, FunctionType::eFunction);
    }

    void Resolver::ifStatement(std::shared_ptr<Statements::If>& ifStatement)
    {
        resolve(ifStatement->condition);
        resolve(ifStatement->thenBranch);
        if (!statementIsNullptr(ifStatement->elseBranch))
        {
            resolve(ifStatement->elseBranch);
        }
    }

    void Resolver::returnStatement(std::shared_ptr<Statements::Return>& returnStatement)
    {
        if (_currentFunction == FunctionType::eNone)
        {
            throw RuntimeError(returnStatement->keyword, "Cannot return from top-level code.");
        }

        if (!expressionIsNullptr(returnStatement->value))
        {
            resolve(returnStatement->value);
        }
    }

    void Resolver::whileStatement(std::shared_ptr<Statements::While>& whileStatement)
    {
        resolve(whileStatement->condition);
        resolve(whileStatement->body);
    }

    void Resolver::variableStatement(std::shared_ptr<Statements::Variable>& variable)
    {
        declare(variable->name);
        if (!expressionIsNullptr(variable->initializer))
        {
            resolve(variable->initializer);
        }
        define(variable->name);
    }

    void Resolver::assignmentExpression(std::shared_ptr<Expressions::Assignment>& assignment)
    {
        resolve(assignment->value);

        Expression expression = assignment;
        resolveLocal(expression, assignment->name);
    }

    void Resolver::binaryExpression(std::shared_ptr<Expressions::Binary>& binary)
    {
        resolve(binary->left);
        resolve(binary->right);
    }

    void Resolver::callExpression(std::shared_ptr<Expressions::Call>& call)
    {
        resolve(call->callee);
        for (auto& argument : call->arguments)
        {
            resolve(argument);
        }
    }

    void Resolver::getExpression(std::shared_ptr<Expressions::Get>& get)
    {
        resolve(get->object);
    }

    void Resolver::groupingExpression(std::shared_ptr<Expressions::Grouping>& grouping)
    {
        resolve(grouping->expression);
    }

    void Resolver::literalExpression(std::shared_ptr<Expressions::Literal>& literal)
    {
        // Do nothing
    }

    void Resolver::logicalExpression(std::shared_ptr<Expressions::Logical>& logical)
    {
        resolve(logical->left);
        resolve(logical->right);
    }

    void Resolver::setExpression(std::shared_ptr<Expressions::Set>& set)
    {
        resolve(set->value);
        resolve(set->object);
    }

    void Resolver::thisExpression(std::shared_ptr<Expressions::This>& thisExpr)
    {
        if (_currentClass == ClassType::eNone)
        {
            throw RuntimeError(thisExpr->keyword, "Cannot use 'this' outside of a class.");
        }

        Expression expression = thisExpr;
        resolveLocal(expression, thisExpr->keyword);
    }

    void Resolver::unaryExpression(std::shared_ptr<Expressions::Unary>& unary)
    {
        resolve(unary->right);
    }

    void Resolver::variableExpression(std::shared_ptr<Expressions::Variable>& variable)
    {
        if (!_scopes.empty() && _scopes.back().contains(variable->name.lexeme)
            && !_scopes.back()[variable->name.lexeme])
        {
            throw RuntimeError(variable->name,
                               "Cannot read local variable in its own initializer.");
        }

        Expression expression = variable;
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

    void Resolver::resolveLocal(Expression& expression, const Token& name)
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
        resolve(function->body);
        endScope();

        _currentFunction = enclosingFunction;
    }
}  // namespace sail
