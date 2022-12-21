#include "../utils/Overload.h"
#include "Token/Token.h"

namespace sail
{

    auto LiteralType::isTruthy() const -> bool
    {
        return std::visit(
            Overload {[&](const std::string& str) { return !str.empty(); },
                      [&](const double& num) { return num != 0; },
                      [&](const bool& b) { return b; },
                      [&](const LiteralNull& null) { return false; }},
            *this);
    }

    auto LiteralType::isNumeric() const -> bool
    {
        return std::holds_alternative<double>(*this)
            || std::holds_alternative<bool>(*this);
    }

    auto LiteralType::isString() const -> bool
    {
        return std::holds_alternative<std::string>(*this);
    }

    auto LiteralType::isNull() const -> bool
    {
        return std::holds_alternative<LiteralNull>(*this);
    }

    auto LiteralType::asNumber() const -> std::optional<double>
    {
        if (std::holds_alternative<double>(*this))
        {
            return std::get<double>(*this);
        }
        if (std::holds_alternative<bool>(*this))
        {
            return std::get<bool>(*this) ? 1.0 : 0.0;
        }

        return std::nullopt;
    }

    auto LiteralType::operator==(const LiteralType& other) const -> bool
    {
        return std::visit(
            Overload {[&](const std::string& str)
                      {
                          return std::holds_alternative<std::string>(other)
                              && str == std::get<std::string>(other);
                      },
                      [&](const double& num)
                      {
                          return std::holds_alternative<double>(other)
                              && num == std::get<double>(other);
                      },
                      [&](const bool& b) {
                          return std::holds_alternative<bool>(other)
                              && b == std::get<bool>(other);
                      },
                      [&](const LiteralNull& null)
                      { return std::holds_alternative<LiteralNull>(other); }},
            *this);
    }

    auto operator<<(std::ostream& os, const LiteralType& literal)
        -> std::ostream&
    {
        std::visit(Overload {[&](const std::string& str)
                             {
                                 if (str.empty())
                                 {
                                     return;
                                 }
                                 os << str;
                             },
                             [&](const double& num) { os << num; },
                             [&](const bool& b) { os << b; },
                             [&](const LiteralNull& null) { os << "null"; }},
                   literal);

        return os;
    }
}  // namespace sail