#pragma once

#include "Types/CallableType.h"
#include "Types/Value.h"

namespace sail::Native::Functions
{
    class Millis : public Types::Callable
    {
      public:
        Millis() = default;

        auto call(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value override;
        auto arity() const -> size_t override;

        auto name() const -> std::string const& override;

      private:
        const std::string _name = "millis";
    };

    class Seconds : public Types::Callable
    {
      public:
        Seconds() = default;

        auto call(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value override;
        auto arity() const -> size_t override;

        auto name() const -> std::string const& override;

      private:
        const std::string _name = "seconds";
    };
}  // namespace sail::Native::Functions