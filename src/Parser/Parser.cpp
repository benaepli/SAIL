#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>

#include "Parser/Parser.h"

#include "Errors/ParserError.h"
#include "Expressions/Expressions.h"
#include "Token/Token.h"
#include "fmt/format.h"

namespace sail
{

    Parser::Parser(std::vector<Token>& tokens)
        : _tokens(tokens)
    {
    }

    auto Parser::parse() -> std::unique_ptr<Expression>
    {
        return expression();
    }

    auto Parser::expression() -> std::unique_ptr<Expression>
    {
        return equality();
    }

    auto Parser::equality() -> std::unique_ptr<Expression>
    {
        std::unique_ptr<Expression> expr = comparison();

        while (match({TokenType::eBangEqual, TokenType::eEqualEqual}))
        {
            Token oper = previous();
            std::unique_ptr<Expression> right = comparison();
            expr = std::make_unique<Expression>(
                Expressions::Binary {std::move(expr), std::move(right), oper});
        }

        return expr;
    }

    auto Parser::comparison() -> std::unique_ptr<Expression>
    {
        std::unique_ptr<Expression> expr = term();

        while (match({TokenType::eGreater,
                      TokenType::eGreaterEqual,
                      TokenType::eLess,
                      TokenType::eLessEqual}))
        {
            Token oper = previous();
            std::unique_ptr<Expression> right = term();
            expr = std::make_unique<Expression>(
                Expressions::Binary {std::move(expr), std::move(right), oper});
        }

        return expr;
    }

    auto Parser::term() -> std::unique_ptr<Expression>
    {
        std::unique_ptr<Expression> expr = factor();

        while (match({TokenType::eMinus, TokenType::ePlus}))
        {
            Token oper = previous();
            std::unique_ptr<Expression> right = factor();
            expr = std::make_unique<Expression>(
                Expressions::Binary {std::move(expr), std::move(right), oper});
        }

        return expr;
    }

    auto Parser::factor() -> std::unique_ptr<Expression>
    {
        std::unique_ptr<Expression> expr = unary();

        while (match({TokenType::eSlash, TokenType::eStar}))
        {
            Token oper = previous();
            std::unique_ptr<Expression> right = unary();
            expr = std::make_unique<Expression>(
                Expressions::Binary {std::move(expr), std::move(right), oper});
        }

        return expr;
    }

    auto Parser::unary() -> std::unique_ptr<Expression>
    {
        if (match({TokenType::eBang, TokenType::eMinus}))
        {
            Token oper = previous();
            std::unique_ptr<Expression> right = unary();
            return std::make_unique<Expression>(
                Expressions::Unary {std::move(right), oper});
        }

        return primary();
    }

    auto Parser::primary() -> std::unique_ptr<Expression>
    {
        if (match({TokenType::eFalse}))
        {
            return std::make_unique<Expression>(Expressions::Literal {false});
        }
        if (match({TokenType::eTrue}))
        {
            return std::make_unique<Expression>(Expressions::Literal {true});
        }
        if (match({TokenType::eNull}))
        {
            return std::make_unique<Expression>(
                Expressions::Literal {LiteralNull {}});
        }

        if (match({TokenType::eNumber, TokenType::eString}))
        {
            return std::make_unique<Expression>(
                Expressions::Literal {previous().literal});
        }
        if (match({TokenType::eLeftParen}))
        {
            std::unique_ptr<Expression> expr = expression();
            consume(TokenType::eRightParen, "Expected ')' after expression.");
            return std::make_unique<Expression>(
                Expressions::Grouping {std::move(expr)});
        }
        throw std::runtime_error("Expected expression.");
    }

    auto Parser::match(const std::vector<TokenType>& tokenTypes) -> bool
    {
        auto each = [&](TokenType tokenType)
        {
            if (check(tokenType))
            {
                advance();
                return true;
            }
            return false;
        };

        return std::ranges::any_of(tokenTypes, each);
    }

    auto Parser::check(TokenType tokenType) -> bool
    {
        if (isAtEnd())
        {
            return false;
        }
        return peek().type == tokenType;
    }

    auto Parser::advance() -> Token&
    {
        if (!isAtEnd())
        {
            _current++;
        }
        return previous();
    }

    auto Parser::previous() -> Token&
    {
        return _tokens[_current - 1];
    }

    auto Parser::isAtEnd() -> bool
    {
        return peek().type == TokenType::eEndOfFile;
    }

    auto Parser::peek() -> Token&
    {
        return _tokens[_current];
    }

    auto Parser::consume(TokenType tokenType, const std::string& message)
        -> Token
    {
        if (check(tokenType))
        {
            return advance();
        }

        throw ParserError {peek(), message};
    }

    void Parser::synchronize()
    {
        advance();

        while (!isAtEnd())
        {
            if (previous().type == TokenType::eSemicolon)
            {
                return;
            }

            switch (peek().type)
            {
                case TokenType::eClass:
                case TokenType::eFun:
                case TokenType::eLet:
                case TokenType::eFor:
                case TokenType::eIf:
                case TokenType::eWhile:
                case TokenType::ePrint:
                case TokenType::eReturn:
                    return;

                default:;
            }

            advance();
        }
    }

}  // namespace sail
