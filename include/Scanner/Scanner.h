#pragma once

#include <vector>
#include <string>

#include "Token.h"

namespace sail
{
    class Scanner
    {
    public:
        explicit Scanner(std::string source);
        std::vector<Token> scanTokens();

    private:
        inline bool isAtEnd() const;
        char advance();
        bool match(char expected);
        char peek() const;
        char peekNext();

        void addToken(TokenType type);
        void addToken(TokenType type, const std::string& literal);

        void scanToken();

        static bool isDigit(char c);
        static bool isAlpha(char c);
        static bool isAlphaNumeric(char c);

        void string();
        void number();
        void identifier();

        const std::string _source;
        std::vector<Token> _tokens;
        size_t _start = 0;
        size_t _current = 0;
        size_t _line = 1;
    };
}
