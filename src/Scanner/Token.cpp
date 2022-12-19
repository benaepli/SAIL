#include "Scanner/Token.h"

#include <magic_enum.hpp>

namespace sail
{
    Token::Token(TokenType type,
                 std::string lexeme,
                 std::string literal,
                 int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)),
          line(line) { }

    Token::Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(std::move(lexeme)), line(line) { }

    std::ostream& operator<<(std::ostream& os, const Token& token)
    {
        os << magic_enum::enum_name(token.type) << " " << token.lexeme << " " <<
            token
            .literal;
        return os;
    }
}
