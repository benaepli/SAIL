#pragma once

#include <memory>
#include <vector>

#include "CallableType.h"
#include "Environment/Environment.h"
#include "Interpreter/Interpreter.h"
#include "Types/Value.h"

namespace sail::Types
{
    class Function : public Callable
    {
      public:
        Function(Statements::Function body,
                 std::shared_ptr<Environment> closure);

        auto call(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value override;
        auto call(Interpreter& interpreter,
                  std::vector<Value>& arguments,
                  std::shared_ptr<Instance> instance) -> Value;
        auto arity() const -> size_t override;

        auto name() const -> std::string const& override;

      private:
        auto process(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value;

        Statements::Function _body;
        std::shared_ptr<Environment> _closure;
        std::shared_ptr<Environment> _localEnvironment;
    };
}  // namespace sail::Types