#pragma once

#include <optional>
#include <ostream>
#include <string>
#include <variant>

#include "Types/NullType.h"

namespace sail
{
    class LiteralType
        : public std::variant<std::string, double, bool, Types::Null>
    {
      public:
        using std::variant<std::string, double, bool, Types::Null>::variant;
        using std::variant<std::string, double, bool, Types::Null>::operator=;

        auto operator==(const LiteralType& other) const -> bool;

        friend auto operator<<(std::ostream& os, const LiteralType& literal)
            -> std::ostream&;
    };
}  // namespace sail