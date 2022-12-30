#include <variant>

#include "Token/Token.h"

#include <magic_enum.hpp>

#include "Token/LiteralType.h"
#include "utils/hash.h"

namespace sail
{

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

    auto Token::operator==(const Token& other) const -> bool
    {
        return type == other.type && lexeme == other.lexeme
            && literal == other.literal && line == other.line;
    }

}  // namespace sail

SAIL_MAKE_HASHABLE_DEFINITION(
    sail::Token, val.type, val.lexeme, val.literal, val.line)
