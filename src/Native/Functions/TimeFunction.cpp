#include "Native/Functions/TimeFunction.h"

#include "Types/NullType.h"

namespace sail::Native::Functions
{

    auto Millis::call(Interpreter& /*interpreter*/,
                      std::vector<Value>& arguments) -> Value
    {
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();

        return static_cast<double>(time);
    }

    auto Millis::arity() const -> size_t
    {
        return 0;
    }

    auto Millis::name() const -> std::string const&
    {
        return _name;
    }

    auto Seconds::call(Interpreter& /*interpreter*/,
                       std::vector<Value>& arguments) -> Value
    {
        auto time = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();

        return static_cast<double>(time);
    }

    auto Seconds::arity() const -> size_t
    {
        return 0;
    }

    auto Seconds::name() const -> std::string const&
    {
        return _name;
    }
}  // namespace sail::Native::Functions