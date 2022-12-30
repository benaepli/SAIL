#pragma once

#include <memory>
#include <vector>

#include "ClassType.h"
#include "ankerl/unordered_dense.h"

namespace sail::Types
{
    class Instance : public std::enable_shared_from_this<Instance>
    {
      public:
        explicit Instance(std::shared_ptr<Class> klass);

        auto get(const Token& name) -> Value;
        auto set(const Token& name, Value value) -> void;

        auto toString() const -> std::string;

      private:
        std::shared_ptr<Class> _klass;
        ankerl::unordered_dense::map<std::string, Value> _fields;
    };
}  // namespace sail::Types