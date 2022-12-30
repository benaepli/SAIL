#pragma once

#include <memory>
#include <vector>

#include "CallableType.h"
#include "Interpreter/Interpreter.h"
#include "Types/Value.h"

namespace sail::Types
{
    class Method : public Callable
    {
      public:
        Method(std::shared_ptr<Instance> instance,
               std::shared_ptr<Function> method);

        auto call(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value override;
        auto arity() const -> size_t override;

        auto name() const -> std::string const& override;

      private:
        std::shared_ptr<Instance> _instance;
        std::shared_ptr<Function> _function;
    };
}  // namespace sail::Types