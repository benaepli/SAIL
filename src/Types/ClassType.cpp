#include "Types/ClassType.h"

#include "Types/InstanceType.h"
#include "Types/Value.h"

namespace sail::Types
{
    Class::Class(
        std::string name,
        ankerl::unordered_dense::map<std::string, std::shared_ptr<Function>>
            methods)
        : _name(std::move(name))
        , _methods(std::move(methods))
    {
    }

    auto Class::call(Interpreter& interpreter, std::vector<Value>& arguments)
        -> Value
    {
        auto instance = std::make_shared<Instance>(shared_from_this());
        return {instance};
    }

    auto Class::arity() const -> size_t
    {
        return 0;
    }

    auto Class::name() const -> std::string const&
    {
        return _name;
    }

    auto Class::findMemberFunction(const std::string& name) const
        -> std::shared_ptr<Function>
    {
        if (_methods.contains(name))
        {
            return _methods.at(name);
        }

        return nullptr;
    }
}  // namespace sail::Types