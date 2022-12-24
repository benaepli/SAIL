#include <utility>

#include "Environment/Environment.h"

#include "Errors/RuntimeError.h"
#include "fmt/format.h"

namespace sail
{
    Environment::Environment(std::shared_ptr<Environment> enclosing)
        : _enclosing(std::move(enclosing))
    {
    }

    auto Environment::get(const Token& name) -> LiteralType&
    {
        if (name.type != TokenType::eIdentifier)
        {
            throw RuntimeError(name, "Attempted to get value of non-variable");
        }

        if (_values.contains(name.lexeme))
        {
            return _values[name.lexeme];
        }

        if (_enclosing != nullptr)
        {
            return _enclosing->get(name);
        }

        throw RuntimeError(
            name,
            fmt::format("Attempted to get undefined variable '{}'",
                        name.lexeme));
    }

    void Environment::define(const std::string& name, const LiteralType& value)
    {
        _values[name] = value;
    }

    void Environment::define(const Token& name, const LiteralType& value)
    {
        if (name.type != TokenType::eIdentifier)
        {
            throw RuntimeError(name, "Attempted to define non-variable");
        }

        _values[name.lexeme] = value;
    }

    void Environment::assign(const Token& name, const LiteralType& value)
    {
        if (name.type != TokenType::eIdentifier)
        {
            throw RuntimeError(name, "Attempted to assign non-variable");
        }

        if (_values.contains(name.lexeme))
        {
            _values[name.lexeme] = value;
            return;
        }

        if (_enclosing != nullptr)
        {
            _enclosing->assign(name, value);
            return;
        }

        throw RuntimeError(
            name,
            fmt::format("Attempted to assign undefined variable '{}'",
                        name.lexeme));
    }
}  // namespace sail
