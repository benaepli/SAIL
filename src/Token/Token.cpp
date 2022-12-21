#include <variant>

#include "Token/Token.h"

#include <magic_enum.hpp>

#include "../utils/Overload.h"

namespace sail
{

    LiteralNull::LiteralNull(TokenType null)
    {
        if (null != TokenType::eNull)
        {
            throw std::invalid_argument(
                "LiteralNull must be initialized with TokenType::eNull");
        }
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

    auto Token::hasLiteral() const -> bool
    {
        return type == TokenType::eString || type == TokenType::eNumber
            || type == TokenType::eTrue || type == TokenType::eFalse
            || type == TokenType::eNull;
    }

    auto operator<<(std::ostream& os, const Token& token) -> std::ostream&
    {
        os << "Token type " << magic_enum::enum_name(token.type)
           << " with lexeme " << token.lexeme;

        if (token.hasLiteral())
        {
            os << " and literal " << token.literal;
        }

        os << " on line " << token.line;

        return os;
    }
}  // namespace sail
