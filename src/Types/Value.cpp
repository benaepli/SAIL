#include "../utils/Overload.h"
#include "Types/Types.h"

namespace sail
{

    auto Value::isTruthy() const -> bool
    {
        return std::visit(
            Overload {[](const std::string& str) -> bool
                      { return !str.empty(); },
                      [](const double& num) -> bool { return num != 0; },
                      [](const bool& val) -> bool { return val; },
                      [](const FunctionPointer& function) -> bool
                      { return function != nullptr; },
                      [](const Types::Null&) -> bool { return false; }},
            *this);
    }

    auto Value::isNumeric() const -> bool
    {
        return std::holds_alternative<double>(*this)
            || std::holds_alternative<bool>(*this);
    }

    auto Value::isString() const -> bool
    {
        return std::holds_alternative<std::string>(*this);
    }

    auto Value::isNull() const -> bool
    {
        return std::holds_alternative<Types::Null>(*this);
    }

    auto Value::asNumber() const -> std::optional<double>
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

    auto Value::operator==(const Value& other) const -> bool
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
                      [&](const bool& val) {
                          return std::holds_alternative<bool>(other)
                              && val == std::get<bool>(other);
                      },
                      [&](const FunctionPointer& function)
                      {
                          return std::holds_alternative<FunctionPointer>(other)
                              && function == std::get<FunctionPointer>(other);
                      },
                      [&](const Types::Null&)
                      { return std::holds_alternative<Types::Null>(other); }},
            *this);
    }

    auto operator<<(std::ostream& os, const Value& value) -> std::ostream&
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
                             [&](const FunctionPointer& function)
                             {
                                 os << "function ";
                                 // TODO: Print function name
                             },
                             [&](const Types::Null& null) { os << "null"; }},
                   value);

        return os;
    }
}  // namespace sail