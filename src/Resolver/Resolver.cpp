#include "Resolver/Resolver.h"

#include <fmt/format.h>

#include "Errors/RuntimeError.h"
#include "Interpreter/Interpreter.h"
#include "utils/Overload.h"

namespace sail
{
    Resolver::Resolver(Interpreter& interpreter)
        : _interpreter(interpreter)
    {
    }

    void Resolver::resolve(std::vector<std::unique_ptr<Statement>>& statements)
    {
        for (auto& statement : statements)
        {
            resolve(*statement);
        }
    }

    void Resolver::resolve(Statement& statement)
    {
        std::visit(
            Overload {
                [this](Statements::Block& statement) -> void
                { blockStatement(statement); },
                [this](Statements::Class& statement) -> void
                { classStatement(statement); },
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

    void Resolver::resolve(Expression& expression)
    {
        std::visit(
            Overload {
                [this, &expression](Expressions::Assignment& assignment) -> void
                { assignmentExpression(assignment, expression); },
                [this](Expressions::Binary& expression) -> void
                { binaryExpression(expression); },
                [this](Expressions::Call& expression) -> void
                { callExpression(expression); },
                [this](Expressions::Get& expression) -> void
                { getExpression(expression); },
                [this](Expressions::Grouping& expression) -> void
                { groupingExpression(expression); },
                [this](Expressions::Literal& expression) -> void
                { literalExpression(expression); },
                [this](Expressions::Logical& expression) -> void
                { logicalExpression(expression); },
                [this](Expressions::Set& expression) -> void
                { setExpression(expression); },
                [this, &expression](Expressions::This& thisExpr) -> void
                { thisExpression(thisExpr, expression); },
                [this](Expressions::Unary& expression) -> void
                { unaryExpression(expression); },
                [this, &expression](Expressions::Variable& variable) -> void
                { variableExpression(variable, expression); },
            },
            expression);
    }

    void Resolver::blockStatement(Statements::Block& block)
    {
        beginScope();
        resolve(block.statements);
        endScope();
    }

    void Resolver::classStatement(Statements::Class& classStatement)
    {
        ClassType enclosingClass = _currentClass;
        _currentClass = ClassType::eClass;

        declare(classStatement.name);
        define(classStatement.name);

        beginScope();
        _scopes.top()["this"] = true;

        for (std::unique_ptr<Statement>& method : classStatement.methods)
        {
            FunctionType functionType = FunctionType::eMethod;
            auto& function = std::get<Statements::Function>(*method);
            resolveFunction(function, functionType);
        }

        endScope();

        _currentClass = enclosingClass;
    }

    void Resolver::expressionStatement(Statements::Expression& expression)
    {
        resolve(*expression.expression);
    }

    void Resolver::functionStatement(Statements::Function& function)
    {
        declare(function.name);
        define(function.name);
        resolveFunction(function, FunctionType::eFunction);
    }

    void Resolver::ifStatement(Statements::If& ifStatement)
    {
        resolve(*ifStatement.condition);
        resolve(*ifStatement.thenBranch);
        if (ifStatement.elseBranch != nullptr)
        {
            resolve(*ifStatement.elseBranch);
        }
    }

    void Resolver::returnStatement(Statements::Return& returnStatement)
    {
        if (_currentFunction == FunctionType::eNone)
        {
            throw RuntimeError(returnStatement.keyword,
                               "Cannot return from top-level code.");
        }

        if (returnStatement.value != nullptr)
        {
            resolve(*returnStatement.value);
        }
    }

    void Resolver::whileStatement(Statements::While& whileStatement)
    {
        resolve(*whileStatement.condition);
        resolve(*whileStatement.body);
    }

    void Resolver::variableStatement(Statements::Variable& variable)
    {
        declare(variable.name);
        if (variable.initializer != nullptr)
        {
            resolve(*variable.initializer);
        }
        define(variable.name);
    }

    void Resolver::assignmentExpression(Expressions::Assignment& assignment,
                                        Expression& expression)
    {
        resolve(*assignment.value);
        resolveLocal(expression, assignment.name);
    }

    void Resolver::binaryExpression(Expressions::Binary& binary)
    {
        resolve(*binary.left);
        resolve(*binary.right);
    }

    void Resolver::callExpression(Expressions::Call& call)
    {
        resolve(*call.callee);
        for (auto& argument : call.arguments)
        {
            resolve(*argument);
        }
    }

    void Resolver::getExpression(Expressions::Get& get)
    {
        resolve(*get.object);
    }

    void Resolver::groupingExpression(Expressions::Grouping& grouping)
    {
        resolve(*grouping.expression);
    }

    void Resolver::literalExpression(Expressions::Literal& literal) {}

    void Resolver::logicalExpression(Expressions::Logical& logical)
    {
        resolve(*logical.left);
        resolve(*logical.right);
    }

    void Resolver::setExpression(Expressions::Set& set)
    {
        resolve(*set.value);
        resolve(*set.object);
    }

    void Resolver::thisExpression(Expressions::This& thisExpression,
                                  Expression& expression)
    {
        if (_currentClass == ClassType::eNone)
        {
            throw RuntimeError(thisExpression.keyword,
                               "Cannot use 'this' outside of a class.");
        }

        resolveLocal(expression, thisExpression.keyword);
    }

    void Resolver::unaryExpression(Expressions::Unary& unary)
    {
        resolve(*unary.right);
    }

    void Resolver::variableExpression(Expressions::Variable& variable,
                                      Expression& expression)
    {
        if (!_scopes.empty()
            && _scopes.top().find(variable.name.lexeme) != _scopes.top().end()
            && !_scopes.top()[variable.name.lexeme])
        {
            throw RuntimeError(
                variable.name,
                "Cannot read local variable in its own initializer.");
        }

        resolveLocal(expression, variable.name);
    }

    void Resolver::beginScope()
    {
        _scopes.push({});
    }

    void Resolver::endScope()
    {
        _scopes.pop();
    }

    void Resolver::declare(const Token& name)
    {
        if (_scopes.empty())
        {
            return;
        }
        auto& scope = _scopes.top();
        if (scope.find(name.lexeme) != scope.end())
        {
            throw RuntimeError(
                name,
                fmt::format(
                    "Variable with name '{}' already declared in this scope.",
                    name.lexeme));
        }
        scope.insert({name.lexeme, false});
    }

    void Resolver::define(const Token& name)
    {
        if (_scopes.empty())
        {
            return;
        }
        _scopes.top()[name.lexeme] = true;
    }

    void Resolver::resolveLocal(Expression& expression, const Token& name)
    {
        for (size_t i = 0; i < _scopes.size(); ++i)
        {
            if (_scopes.top().find(name.lexeme) != _scopes.top().end())
            {
                _interpreter.resolve(expression, i);
                return;
            }
        }
    }

    void Resolver::resolveFunction(Statements::Function& function,
                                   FunctionType type)
    {
        FunctionType enclosingFunction = _currentFunction;
        _currentFunction = type;

        beginScope();
        for (Token& param : function.parameters)
        {
            declare(param);
            define(param);
        }
        resolve(function.body);
        endScope();

        _currentFunction = enclosingFunction;
    }

}  // namespace sail