#pragma once

#include <iostream>
#include <string>

namespace sail
{
    enum class TokenType
    {
        // Single-character tokens.
        eLeftParen,
        eRightParen,
        eLeftBrace,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,

        // One or two character tokens.
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // Literals.
        IDENTIFIER,
        STRING,
        NUMBER,

        // Keywords.
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        eNull,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        LET,
        WHILE,

        BITWISE_OR,
        BITWISE_AND,
        BITWISE_XOR,
        BITWISE_NOT,

        END_OF_FILE
    };

    class Token
    {
      public:
        Token(TokenType type,
              std::string lexeme,
              std::string literal,
              int line);
        Token(TokenType type, std::string lexeme, int line);

        // ostream operator overload
        friend std::ostream& operator<<(std::ostream& os, const Token& token);

        TokenType type;
        std::string lexeme;
        std::string literal;
        int line;
    };
}  // namespace sail
