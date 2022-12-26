#include "../utils/Overload.h"
#include "Token/Token.h"

namespace sail
{
    auto LiteralType::operator==(const LiteralType& other) const -> bool
    {
        return std::visit(
            Overload {
                [&](const std::string& str)
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
                [&](const Types::Null&)
                { return std::holds_alternative<Types::Null>(other); },
            },
            *this);
    }

    auto operator<<(std::ostream& os, const LiteralType& literal)
        -> std::ostream&
    {
        std::visit(
            Overload {
                [&](const std::string& str)
                {
                    if (str.empty())
                    {
                        return;
                    }
                    os << str;
                },
                [&](const double& num) { os << num; },
                [&](const bool& b) { os << b; },
                [&](const Types::Null&) { os << "null"; },
            },
            literal);

        return os;
    }
}  // namespace sail