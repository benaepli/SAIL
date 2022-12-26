#pragma once

#include <memory>
#include <vector>

#include "Environment/Environment.h"
#include "Statements/FunctionStatement.h"
#include "Statements/Statement.h"
#include "Types/Value.h"

namespace sail
{
    class Interpreter;
    class Environment;
}  // namespace sail

namespace sail::Types
{
    class Callable
    {
      public:
        Callable() = default;
        virtual ~Callable() = default;

        Callable(const Callable& other) = default;
        auto operator=(const Callable& other) -> Callable& = default;
        Callable(Callable&& other) = default;
        auto operator=(Callable&& other) -> Callable& = default;

        virtual auto call(Interpreter& interpreter,
                          std::vector<Value>& arguments) -> Value = 0;
        virtual auto arity() const -> size_t = 0;

        virtual auto name() const -> std::string const& = 0;
    };

}  // namespace sail::Types