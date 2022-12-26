#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "Expressions/Expressions.h"
#include "Statements/Statements.h"
#include "Token/Token.h"

namespace sail
{
    class Parser
    {
      public:
        explicit Parser(std::vector<Token>& tokens);

        auto parse() -> std::vector<std::unique_ptr<Statement>>;

      private:
        auto statement() -> std::unique_ptr<Statement>;
        auto declaration() -> std::unique_ptr<Statement>;
        auto varDeclaration() -> std::unique_ptr<Statement>;
        auto blockStatement() -> std::unique_ptr<Statement>;
        auto expressionStatement() -> std::unique_ptr<Statement>;
        auto functionStatement() -> std::unique_ptr<Statement>;
        auto whileStatement() -> std::unique_ptr<Statement>;
        auto ifStatement() -> std::unique_ptr<Statement>;
        auto forStatement() -> std::unique_ptr<Statement>;
        auto returnStatement() -> std::unique_ptr<Statement>;

        auto expression() -> std::unique_ptr<Expression>;
        auto assignment() -> std::unique_ptr<Expression>;
        auto orExpression() -> std::unique_ptr<Expression>;
        auto andExpression() -> std::unique_ptr<Expression>;
        auto equality() -> std::unique_ptr<Expression>;
        auto comparison() -> std::unique_ptr<Expression>;
        auto term() -> std::unique_ptr<Expression>;
        auto factor() -> std::unique_ptr<Expression>;
        auto unary() -> std::unique_ptr<Expression>;
        auto call() -> std::unique_ptr<Expression>;
        auto primary() -> std::unique_ptr<Expression>;

        inline auto block() -> std::vector<std::unique_ptr<Statement>>;
        inline auto finishCall(std::unique_ptr<Expression> callee)
            -> std::unique_ptr<Expression>;

        auto match(const std::vector<TokenType>& tokenTypes) -> bool;
        auto check(TokenType tokenType) -> bool;
        auto advance() -> Token&;
        auto previous() -> Token&;

        auto isAtEnd() -> bool;
        auto peek() -> Token&;

        auto consume(TokenType tokenType) -> Token*;
        inline auto consume(TokenType tokenType, const std::string& message)
            -> Token&;

        void synchronize();

        std::vector<Token>& _tokens;
        size_t _current = 0;
    };

}  // namespace sail
