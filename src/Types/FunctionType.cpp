#include <memory>

#include "Types/FunctionType.h"

#include "Interpreter/Interpreter.h"
#include "Types/NullType.h"

namespace sail::Types
{
    Function::Function(std::shared_ptr<Statements::Function> body,
                       std::shared_ptr<Environment> closure,
                       bool isInitializer)
        : _body(std::move(body))
        , _closure(std::move(closure))
        , _isInitializer(isInitializer)
    {
    }

    auto Function::call(Interpreter& interpreter, std::vector<Value>& arguments) -> Value
    {
        auto environment = std::make_shared<Environment>(_closure);

        return process(interpreter, arguments, environment);
    }

    auto Function::call(Interpreter& interpreter,
                        std::vector<Value>& arguments,
                        std::shared_ptr<Instance> instance) -> Value
    {
        auto environment = std::make_shared<Environment>(_closure);
        environment->define("this", instance);

        return process(interpreter, arguments, environment);
    }

    auto Function::arity() const -> size_t
    {
        return _body->parameters.size();
    }

    auto Function::name() const -> std::string const&
    {
        return _body->name.lexeme;
    }

    auto Function::process(Interpreter& interpreter,
                           std::vector<Value>& arguments,
                           std::shared_ptr<Environment>& environment) -> Value
    {
        for (size_t i = 0; i < _body->parameters.size(); i++)
        {
            environment->define(_body->parameters[i].lexeme, arguments[i]);
        }

        try
        {
            interpreter.executeBlock(_body->body, environment);
        }
        catch (Return& returnValue)
        {
            if (_isInitializer)
            {
                return environment->getAt(0, "this");
            }

            return returnValue.value();
        }

        return Types::Null {};
    }

}  // namespace sail::Types