#pragma once

#include <string>
#include <vector>

#include "Token/Token.h"

namespace sail
{
    class Scanner
    {
      public:
        explicit Scanner(std::string source, std::vector<Token>& tokens);
        auto scanTokens() -> std::vector<Token>;

      private:
        inline auto isAtEnd() const -> bool;
        auto advance() -> char;
        auto match(char expected) -> bool;
        auto peek() const -> char;
        auto peekNext() -> char;

        void addToken(TokenType type);
        void addToken(TokenType type, const std::string& literal);
        void addToken(TokenType type, double literal);

        void scanToken();

        static auto isDigit(char c) -> bool;
        static auto isAlpha(char c) -> bool;
        static auto isAlphaNumeric(char c) -> bool;

        void string();
        void number();
        void identifier();

        const std::string _source;
        std::vector<Token>& _tokens;
        size_t _start = 0;
        size_t _current = 0;
        size_t _line = 1;
    };
}  // namespace sail
