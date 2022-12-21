#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "Errors/Errors.h"
#include "Expressions/Expressions.h"
#include "Token/Token.h"

namespace sail
{

    class Parser
    {
      public:
        explicit Parser(std::vector<Token>& tokens);

        auto parse() -> std::unique_ptr<Expression>;

      private:
        auto expression() -> std::unique_ptr<Expression>;
        auto equality() -> std::unique_ptr<Expression>;
        auto comparison() -> std::unique_ptr<Expression>;
        auto term() -> std::unique_ptr<Expression>;
        auto factor() -> std::unique_ptr<Expression>;
        auto unary() -> std::unique_ptr<Expression>;
        auto primary() -> std::unique_ptr<Expression>;

        auto match(const std::vector<TokenType>& tokenTypes) -> bool;
        auto check(TokenType tokenType) -> bool;
        auto advance() -> Token&;
        auto previous() -> Token&;

        auto isAtEnd() -> bool;
        auto peek() -> Token&;

        auto consume(TokenType tokenType, const std::string& message) -> Token;

        void synchronize();

        std::vector<Token>& _tokens;
        size_t _current = 0;
    };

}  // namespace sail
