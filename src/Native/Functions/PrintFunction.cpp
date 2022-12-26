#include "Native/Functions/PrintFunction.h"

#include "Types/NullType.h"

namespace sail::Native::Functions
{

    auto Print::call(Interpreter& /*interpreter*/,
                     std::vector<Value>& arguments) -> Value
    {
        for (const auto& argument : arguments)
        {
            std::cout << argument << std::endl;
        }
        return Types::Null {};
    }

    auto Print::arity() const -> size_t
    {
        return std::numeric_limits<size_t>::max();
    }

    auto Print::name() const -> std::string const&
    {
        return _name;
    }
}  // namespace sail::Native::Functions