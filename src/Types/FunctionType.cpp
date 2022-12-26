#include <memory>

#include "Types/FunctionType.h"

#include "Interpreter/Interpreter.h"
#include "Types/NullType.h"

namespace sail::Types
{
    Function::Function(Statements::Function body,
                       std::shared_ptr<Environment> closure)
        : _body(std::move(body))
        , _closure(std::move(closure))
    {
    }

    auto Function::call(Interpreter& interpreter, std::vector<Value>& arguments)
        -> Value
    {
        auto environment = std::make_shared<Environment>(_closure);

        for (int i = 0; i < _body.parameters.size(); i++)
        {
            environment->define(_body.parameters[i].lexeme, arguments[i]);
        }

        try
        {
            interpreter.executeBlock(_body.body, environment);
        }
        catch (Return& returnValue)
        {
            return returnValue.value();
        }

        return Types::Null {};
    }

    auto Function::arity() const -> size_t
    {
        return _body.parameters.size();
    }

    auto Function::name() const -> std::string const&
    {
        return _body.name.lexeme;
    }
}  // namespace sail::Types