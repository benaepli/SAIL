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
        , _localEnvironment(std::make_shared<Environment>(_closure))
    {
    }

    auto Function::call(Interpreter& interpreter, std::vector<Value>& arguments) -> Value
    {
        auto& environment = _localEnvironment;
        environment->reset();

        return process(interpreter, arguments);
    }

    auto Function::call(Interpreter& interpreter,
                        std::vector<Value>& arguments,
                        std::shared_ptr<Instance> instance) -> Value
    {
        auto& environment = _localEnvironment;
        environment->reset();
        environment->define("this", instance);

        return process(interpreter, arguments);
    }

    auto Function::arity() const -> size_t
    {
        return _body->parameters.size();
    }

    auto Function::name() const -> std::string const&
    {
        return _body->name.lexeme;
    }

    auto Function::process(Interpreter& interpreter, std::vector<Value>& arguments) -> Value
    {
        for (size_t i = 0; i < _body->parameters.size(); i++)
        {
            _localEnvironment->define(_body->parameters[i].lexeme, arguments[i]);
        }

        try
        {
            interpreter.executeBlock(_body->body, _localEnvironment);
        }
        catch (Return& returnValue)
        {
            if (_isInitializer)
            {
                return _localEnvironment->getAt(0, "this");
            }

            return returnValue.value();
        }

        return Types::Null {};
    }

}  // namespace sail::Types