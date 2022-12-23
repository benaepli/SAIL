#include <utility>

#include "Environment/Environment.h"
#include "Errors/RuntimeError.h"

namespace sail
{
    Environment::Environment(std::shared_ptr<Environment> enclosing)
        : _enclosing(std::move(enclosing))
    {
    }

    auto Environment::get(const Token& name) -> LiteralType&
    {
        if (name.type != TokenType::eIdentifier || !name.literal.isString())
        {
            throw RuntimeError(name, "Attempted to get value of non-variable");
        }

        const auto& nameString = std::get<std::string>(name.literal);
        if (_values.contains(nameString))
        {
            return _values[nameString];
        }

        if (_enclosing != nullptr)
        {
            return _enclosing->get(name);
        }

        throw RuntimeError(name, "Undefined variable '" + nameString + "'");
    }

    void Environment::define(const std::string& name, const LiteralType& value)
    {
        _values[name] = value;
    }

    void Environment::define(const Token& name, const LiteralType& value)
    {
        if (name.type != TokenType::eIdentifier || !name.literal.isString())
        {
            throw RuntimeError(name, "Attempted to define non-variable");
        }

        const auto& nameString = std::get<std::string>(name.literal);
        _values[nameString] = value;
    }

    void Environment::assign(const Token& name, const LiteralType& value)
    {
        if (name.type != TokenType::eIdentifier || !name.literal.isString())
        {
            throw RuntimeError(name, "Attempted to assign non-variable");
        }

        const auto& nameString = std::get<std::string>(name.literal);

        if (_values.contains(nameString))
        {
            _values[nameString] = value;
            return;
        }

        if (_enclosing != nullptr)
        {
            _enclosing->assign(name, value);
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + nameString + "'");
    }
}  // namespace sail
