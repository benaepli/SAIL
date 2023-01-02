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

    auto Environment::get(const std::string& name) -> Value&
    {
        auto it = _values.find(name);
        if (it != _values.end())
        {
            return it->second;
        }

        if (_enclosing != nullptr)
        {
            return _enclosing->get(name);
        }

        throw std::runtime_error(fmt::format("Attempted to get undefined variable '{}'", name));
    }

    auto Environment::get(const Token& name) -> Value&
    {
        auto it = _values.find(name.lexeme);
        if (it != _values.end())
        {
            return it->second;
        }

        if (_enclosing != nullptr)
        {
            return _enclosing->get(name);
        }

        throw RuntimeError(name,
                           fmt::format("Attempted to get undefined variable '{}'", name.lexeme));
    }

    auto Environment::getAt(size_t distance, const std::string& name) -> Value&
    {
        return ancestor(distance)->_values[name];
    }

    auto Environment::getAt(size_t distance, const Token& name) -> Value&
    {
        return ancestor(distance)->_values[name.lexeme];
    }

    void Environment::define(const std::string& name, const Value& value)
    {
        _values[name] = value;
    }

    void Environment::define(const Token& name, const Value& value)
    {
        _values[name.lexeme] = value;
    }

    void Environment::assign(const Token& name, const Value& value)
    {
        auto it = _values.find(name.lexeme);
        if (it != _values.end())
        {
            it->second = value;
            return;
        }

        if (_enclosing != nullptr)
        {
            _enclosing->assign(name, value);
            return;
        }

        throw RuntimeError(name,
                           fmt::format("Attempted to assign undefined variable '{}'", name.lexeme));
    }

    void Environment::assignAt(size_t distance, const Token& name, const Value& value)
    {
        ancestor(distance)->assign(name, value);
    }

    auto Environment::ancestor(size_t distance) -> Environment*
    {
        auto* environment = this;
        for (size_t i = 0; i < distance; i++)
        {
            environment = environment->_enclosing.get();
        }

        return environment;
    }

    void Environment::reset()
    {
        _values.clear();
    }
}  // namespace sail
