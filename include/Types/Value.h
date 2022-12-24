#pragma once

#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <variant>

#include "NullType.h"

namespace sail
{

    namespace Types
    {
        struct Function;
    }  // namespace Types

    using FunctionPointer = std::shared_ptr<Types::Function>;

    using ValueVariantType =
        std::variant<std::string, double, bool, FunctionPointer, Types::Null>;

    struct Value : public ValueVariantType
    {
        using ValueVariantType::ValueVariantType;
        using ValueVariantType::operator=;

        auto isTruthy() const -> bool;
        auto isNumeric() const -> bool;
        auto isString() const -> bool;
        auto isNull() const -> bool;

        // Implicitly converts the value to a number if possible.
        auto asNumber() const -> std::optional<double>;

        auto operator==(const Value& other) const -> bool;
        friend auto operator<<(std::ostream& ostr, const Value& value)
            -> std::ostream&;
    };
}  // namespace sail