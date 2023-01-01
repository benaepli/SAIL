#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "Expressions/Expressions.h"
#include "Statements/Statement.h"
#include "Statements/Statements.h"
#include "Token/Token.h"

namespace sail
{
    class Parser
    {
      public:
        explicit Parser(std::vector<Token>& tokens);

        auto parse() -> std::vector<std::shared_ptr<Statement>>;  // Unperformant, but a prototype

      private:
        auto statement() -> std::shared_ptr<Statement>;
        auto declaration() -> std::shared_ptr<Statement>;
        auto classDeclaration() -> std::shared_ptr<Statement>;
        auto varDeclaration() -> std::shared_ptr<Statement>;
        auto blockStatement() -> std::shared_ptr<Statement>;
        auto expressionStatement() -> std::shared_ptr<Statement>;
        auto functionStatement() -> std::shared_ptr<Statements::Function>;
        auto whileStatement() -> std::shared_ptr<Statement>;
        auto ifStatement() -> std::shared_ptr<Statement>;
        auto forStatement() -> std::shared_ptr<Statement>;
        auto returnStatement() -> std::shared_ptr<Statement>;

        auto expression() -> std::shared_ptr<Expression>;
        auto assignment() -> std::shared_ptr<Expression>;
        auto orExpression() -> std::shared_ptr<Expression>;
        auto andExpression() -> std::shared_ptr<Expression>;
        auto equality() -> std::shared_ptr<Expression>;
        auto comparison() -> std::shared_ptr<Expression>;
        auto term() -> std::shared_ptr<Expression>;
        auto factor() -> std::shared_ptr<Expression>;
        auto unary() -> std::shared_ptr<Expression>;
        auto call() -> std::shared_ptr<Expression>;
        auto primary() -> std::shared_ptr<Expression>;

        inline auto block() -> std::vector<std::shared_ptr<Statement>>;
        inline auto finishCall(std::shared_ptr<Expression>& callee) -> std::shared_ptr<Expression>;

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
