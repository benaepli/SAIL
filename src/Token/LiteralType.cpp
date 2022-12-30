#include "Token/LiteralType.h"

#include <vcruntime.h>

#include "Token/Token.h"
#include "ankerl/unordered_dense.h"
#include "utils/Overload.h"
#include "utils/hash.h"

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

    static auto hash(const std::string& str) -> size_t;
    static auto hash(double num) -> size_t;
    static auto hash(bool b) -> size_t;
    static auto hash(const Types::Null& n) -> size_t;

    static auto hash(const LiteralType& literal) -> size_t
    {
        return std::visit(
            Overload {
                [&](const std::string& str) { return hash(str); },
                [&](const double& num) { return hash(num); },
                [&](const bool& b) { return hash(b); },
                [&](const Types::Null& n) { return hash(n); },
            },
            literal);
    }

    static auto hash(const std::string& str) -> size_t
    {
        return ankerl::unordered_dense::hash<std::string> {}(str);
    }

    static auto hash(double num) -> size_t
    {
        return ankerl::unordered_dense::hash<double> {}(num);
    }

    static auto hash(bool b) -> size_t
    {
        return ankerl::unordered_dense::hash<bool> {}(b);
    }

    static auto hash(const Types::Null& /*unused*/) -> size_t
    {
        return 0;
    }
}  // namespace sail

SAIL_HASH_DEFINITION(sail::LiteralType, sail::hash)