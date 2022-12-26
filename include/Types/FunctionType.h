#pragma once

#include <memory>
#include <vector>

#include "CallableType.h"
#include "Interpreter/Interpreter.h"

namespace sail::Types
{
    class Function : public Callable
    {
      public:
        explicit Function(Statements::Function body,
                          std::shared_ptr<Environment> closure);

        auto call(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value override;
        auto arity() const -> size_t override;

        auto name() const -> std::string const& override;

      private:
        Statements::Function _body;
        std::shared_ptr<Environment> _closure;
    };
}  // namespace sail::Types