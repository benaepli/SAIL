#include <memory>

#include "Types/MethodType.h"

#include "Interpreter/Interpreter.h"
#include "Types/NullType.h"

namespace sail::Types
{
    Method::Method(std::shared_ptr<Instance> instance,
                   std::shared_ptr<Function> method)
        : _instance(std::move(instance))
        , _function(std::move(method))
    {
    }

    auto Method::call(Interpreter& interpreter, std::vector<Value>& arguments)
        -> Value
    {
        return _function->call(interpreter, arguments, _instance);
    }

    auto Method::arity() const -> size_t
    {
        return _function->arity();
    }

    auto Method::name() const -> std::string const&
    {
        return _function->name();
    }

}  // namespace sail::Types