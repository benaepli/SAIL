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

        auto parse() -> std::vector<Statement>;

      private:
        auto statement() -> Statement;
        auto declaration() -> Statement;
        auto classDeclaration() -> Statement;
        auto varDeclaration() -> Statement;
        auto blockStatement() -> Statement;
        auto expressionStatement() -> Statement;
        auto functionStatement() -> std::shared_ptr<Statements::Function>;
        auto whileStatement() -> Statement;
        auto ifStatement() -> Statement;
        auto forStatement() -> Statement;
        auto returnStatement() -> Statement;

        auto expression() -> Expression;
        auto assignment() -> Expression;
        auto orExpression() -> Expression;
        auto andExpression() -> Expression;
        auto equality() -> Expression;
        auto comparison() -> Expression;
        auto term() -> Expression;
        auto factor() -> Expression;
        auto unary() -> Expression;
        auto call() -> Expression;
        auto primary() -> Expression;

        inline auto block() -> std::vector<Statement>;
        inline auto finishCall(Expression callee) -> Expression;

        auto match(const std::vector<TokenType>& tokenTypes) -> bool;
        auto check(TokenType tokenType) -> bool;
        auto advance() -> Token&;
        auto previous() -> Token&;

        auto isAtEnd() -> bool;
        auto peek() -> Token&;

        auto consume(TokenType tokenType) -> Token*;
        inline auto consume(TokenType tokenType, const std::string& message) -> Token&;

        void synchronize();

        std::vector<Token>& _tokens;
        size_t _current = 0;
    };

}  // namespace sail
