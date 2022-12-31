#include <memory>
#include <utility>

#include "Types/InstanceType.h"

#include "Errors/RuntimeError.h"
#include "Types/ClassType.h"
#include "Types/FunctionType.h"
#include "Types/Value.h"
#include "fmt/format.h"

namespace sail::Types
{
    Instance::Instance(std::shared_ptr<Class> klass)
        : _klass(std::move(klass))
    {
    }

    auto Instance::get(const Token& name) -> Value
    {
        if (_fields.contains(name.lexeme))
        {
            return _fields[name.lexeme];
        }

        std::shared_ptr<Function> function = _klass->findMemberFunction(name.lexeme);
        if (function != nullptr)
        {
            auto method = std::make_shared<Method>(shared_from_this(), function);
            set(name, {method});
            return {method};
        }

        throw RuntimeError(name, fmt::format("Undefined property '{}'.", name.lexeme));
    }

    auto Instance::set(const Token& name, Value value) -> void
    {
        _fields[name.lexeme] = std::move(value);
    }

    auto Instance::toString() const -> std::string
    {
        return _klass->name() + " instance";
    }
}  // namespace sail::Types
