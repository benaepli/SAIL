#pragma once

#include <memory>
#include <vector>

#include "Statements/FunctionStatement.h"
#include "Statements/Statement.h"

namespace sail
{
    class Interpreter;
    class Environment;
    class Value;
}  // namespace sail

namespace sail::Types
{
    class Callable
    {
      public:
        Callable() = default;
        virtual ~Callable() = default;

        virtual auto call(Interpreter& interpreter,
                          std::vector<Value>& arguments) -> Value = 0;
        virtual auto arity() const -> size_t = 0;

        virtual auto name() const -> std::string const& = 0;
    };

}  // namespace sail::Types