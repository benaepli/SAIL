#pragma once

#include <memory>
#include <vector>

#include "CallableType.h"
#include "ankerl/unordered_dense.h"

namespace sail::Types
{
    class Function;

    class Class
        : public Callable
        , public std::enable_shared_from_this<Class>
    {
      public:
        explicit Class(
            std::string name,
            ankerl::unordered_dense::map<std::string, std::shared_ptr<Function>>
                methods);

        auto call(Interpreter& interpreter, std::vector<Value>& arguments)
            -> Value override;
        auto arity() const -> size_t override;

        auto name() const -> std::string const& override;

        auto findMemberFunction(const std::string& name) const
            -> std::shared_ptr<Function>;

      private:
        std::string _name;
        ankerl::unordered_dense::map<std::string, std::shared_ptr<Function>>
            _methods;
    };
}  // namespace sail::Types