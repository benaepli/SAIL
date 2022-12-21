#pragma once

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace sail
{
    enum class TokenType;

    struct LiteralNull
    {
    };

    class LiteralType
        : public std::variant<std::string, double, bool, LiteralNull>
    {
      public:
        auto isTruthy() const -> bool;

        auto isNumeric() const -> bool;
        auto isString() const -> bool;
        auto isNull() const -> bool;

        auto asNumber() const -> std::optional<double>;

        using std::variant<std::string, double, bool, LiteralNull>::variant;
        using std::variant<std::string, double, bool, LiteralNull>::operator=;

        auto operator==(const LiteralType& other) const -> bool;

        friend auto operator<<(std::ostream& os, const LiteralType& literal)
            -> std::ostream&;
    };
}  // namespace sail