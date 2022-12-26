#pragma once

#include "Types/CallableType.h"
#include "Types/Value.h"

namespace sail::Native::Functions
{
    class Print : public Types::Callable
    {
      public:
        Print() = default;

        auto call(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value override;
        auto arity() const -> size_t override;

        auto name() const -> std::string const& override;

      private:
        const std::string _name = "print";
    };
}  // namespace sail::Native::Functions