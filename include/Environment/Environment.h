#pragma once

#include "Token/Token.h"
#include "Types/Value.h"
#include "ankerl/unordered_dense.h"

namespace sail
{
    class Environment
    {
      public:
        Environment() = default;
        explicit Environment(std::shared_ptr<Environment> enclosing);

        auto get(const std::string& name) -> Value&;
        auto get(const Token& name) -> Value&;
        auto getAt(size_t distance, const std::string& name) -> Value&;
        auto getAt(size_t distance, const Token& name) -> Value&;
        void define(const std::string& name, const Value& value);
        void define(const Token& name, const Value& value);
        void assign(const Token& name, const Value& value);
        void assignAt(size_t distance, const Token& name, const Value& value);

        auto enclosing() const -> std::shared_ptr<Environment> const& { return _enclosing; }

        void reset();

      private:
        auto ancestor(size_t distance) -> Environment*;

        ankerl::unordered_dense::map<std::string, Value> _values {};
        std::shared_ptr<Environment> _enclosing {};
    };
}  // namespace sail
