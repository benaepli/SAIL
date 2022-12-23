#pragma once

#include "Token/LiteralType.h"
#include "Token/Token.h"
#include "ankerl/unordered_dense.h"

namespace sail
{
    class Environment
    {
      public:
        Environment() = default;
        explicit Environment(std::shared_ptr<Environment> enclosing);

        auto get(const Token& name) -> LiteralType&;
        void define(const std::string& name, const LiteralType& value);
        void define(const Token& name, const LiteralType& value);
        void assign(const Token& name, const LiteralType& value);

      private:
        ankerl::unordered_dense::map<std::string, LiteralType> _values {};
        std::shared_ptr<Environment> _enclosing {};
    };
}  // namespace sail
