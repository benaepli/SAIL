#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>

#include "Parser/Parser.h"

#include "Errors/ParserError.h"
#include "Expressions/Expressions.h"
#include "Expressions/VariableExpression.h"
#include "Statements/Statements.h"
#include "Token/Token.h"
#include "fmt/format.h"

namespace sail
{

    Parser::Parser(std::vector<Token>& tokens)
        : _tokens(tokens)
    {
    }

    auto Parser::parse() -> std::vector<std::unique_ptr<Statement>>
    {
        std::vector<std::unique_ptr<Statement>> statements {};

        while (!isAtEnd())
        {
            std::unique_ptr<Statement> newStatement = statement();
            statements.push_back(std::move(newStatement));
        }

        return statements;
    }

    auto Parser::statement() -> std::unique_ptr<Statement>
    {
        return declaration();
    }

    auto Parser::declaration() -> std::unique_ptr<Statement>
    {
        if (match({TokenType::eLet}))
        {
            return varDeclaration();
        }

        if (match({TokenType::eLeftBrace}))
        {
            return blockStatement();
        }

        if (match({TokenType::ePrint}))
        {
            return printStatement();
        }

        if (match({TokenType::eIf}))
        {
            return ifStatement();
        }

        if (match({TokenType::eWhile}))
        {
            return whileStatement();
        }

        if (match({TokenType::eFor}))
        {
            return forStatement();
        }

        return expressionStatement();
    };

    auto Parser::varDeclaration() -> std::unique_ptr<Statement>
    {
        Token& name =
            consume(TokenType::eIdentifier, "Expected identifier after 'let'");

        std::unique_ptr<Expression> initializer {};
        if (match({TokenType::eEqual}))
        {
            initializer = std::move(expression());
        }

        Token& semicolon =
            consume(TokenType::eSemicolon,
                    "Expected semicolon after variable declaration");

        return std::make_unique<Statement>(
            Statements::Variable {name, std::move(initializer)});
    }

    auto Parser::blockStatement() -> std::unique_ptr<Statement>
    {
        std::vector<std::unique_ptr<Statement>> statements {};

        while (!check(TokenType::eRightBrace) && !isAtEnd())
        {
            statements.push_back(std::move(statement()));
        }

        consume(TokenType::eRightBrace, "Expected '}' after block");
        return std::make_unique<Statement>(
            Statements::Block {std::move(statements)});
    }

    auto Parser::expressionStatement() -> std::unique_ptr<Statement>
    {
        std::unique_ptr<Expression> newExpression = expression();
        consume(TokenType::eSemicolon, "Expected semicolon after value");
        return std::make_unique<Statement>(
            Statements::Expression {std::move(newExpression)});
    }

    auto Parser::printStatement() -> std::unique_ptr<Statement>
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'print'");
        std::unique_ptr<Expression> newExpression = expression();
        consume(TokenType::eRightParen, "Expected ')' after print expression");
        consume(TokenType::eSemicolon, "Expected semicolon after value");
        return std::make_unique<Statement>(

            Statements::Print {std::move(newExpression)});
    }

    auto Parser::ifStatement() -> std::unique_ptr<Statement>
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'if'");
        std::unique_ptr<Expression> condition = expression();
        consume(TokenType::eRightParen, "Expected ')' after if condition");

        std::unique_ptr<Statement> thenBranch = statement();
        std::unique_ptr<Statement> elseBranch {};
        if (match({TokenType::eElse}))
        {
            elseBranch = statement();
        }

        return std::make_unique<Statement>(
            Statements::If {std::move(condition),
                            std::move(thenBranch),
                            std::move(elseBranch)});
    }

    auto Parser::whileStatement() -> std::unique_ptr<Statement>
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'if'");
        std::unique_ptr<Expression> condition = expression();
        consume(TokenType::eLeftParen, "Expected ')' after while condition");
        std::unique_ptr<Statement> body = statement();

        return std::make_unique<Statement>(
            Statements::While {std::move(condition), std::move(body)});
    }

    auto Parser::forStatement() -> std::unique_ptr<Statement>
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'for'");

        std::unique_ptr<Statement> initializer {};
        if (match({TokenType::eSemicolon}))
        {
            initializer = nullptr;
        }
        else if (match({TokenType::eLet}))
        {
            initializer = varDeclaration();
        }
        else
        {
            initializer = expressionStatement();
        }

        std::unique_ptr<Expression> condition {};
        if (!check(TokenType::eSemicolon))
        {
            condition = expression();
        }
        consume(TokenType::eSemicolon, "Expected ';' after loop condition");

        std::unique_ptr<Expression> increment {};
        if (!check(TokenType::eRightParen))
        {
            increment = expression();
        }
        consume(TokenType::eRightParen, "Expected ')' after for clauses");

        std::unique_ptr<Statement> body = statement();

        if (increment != nullptr)
        {
            std::vector<std::unique_ptr<Statement>> statements {};
            statements.push_back(std::move(body));
            statements.push_back(std::make_unique<Statement>(
                Statements::Expression {std::move(increment)}));
            body = std::make_unique<Statement>(
                Statements::Block {std::move(statements)});
        }

        if (condition == nullptr)
        {
            condition = std::make_unique<Expression>(
                Expressions::Literal {LiteralType {true}});
        }

        body = std::make_unique<Statement>(
            Statements::While {std::move(condition), std::move(body)});

        if (initializer != nullptr)
        {
            std::vector<std::unique_ptr<Statement>> statements {};
            statements.push_back(std::move(initializer));
            statements.push_back(std::move(body));
            body = std::make_unique<Statement>(
                Statements::Block {std::move(statements)});
        }

        return body;
    }

    auto Parser::expression() -> std::unique_ptr<Expression>
    {
        return assignment();
    }

    auto Parser::assignment() -> std::unique_ptr<Expression>
    {
        std::unique_ptr<Expression> expr = orExpression();

        if (match({TokenType::eEqual}))
        {
            Token equals = previous();
            std::unique_ptr<Expression> value = assignment();

            if (auto* variable = std::get_if<Expressions::Variable>(&*expr))
            {
                Token& name = variable->name;
                return std::make_unique<Expression>(
                    Expressions::Assignment {std::move(value), name});
            }
            throw ParserError(equals, "Invalid assignment target");
        }

        return expr;
    }

    auto Parser::orExpression() -> std::unique_ptr<Expression>
    {
        std::unique_ptr<Expression> expr = andExpression();

        while (match({TokenType::eOr}))
        {
            Token oper = previous();
            std::unique_ptr<Expression> right = andExpression();
            expr = std::make_unique<Expression>(
                Expressions::Logical {std::move(expr), std::move(right), oper});
        }

        return expr;
    }

    auto Parser::andExpression() -> std::unique_ptr<Expression>
    {
        std::unique_ptr<Expression> expr = equality();

        while (match({TokenType::eAnd}))
        {
            Token oper = previous();
            std::unique_ptr<Expression> right = equality();
            expr = std::make_unique<Expression>(
                Expressions::Logical {std::move(expr), std::move(right), oper});
        }

        return expr;
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

        if (match({TokenType::eIdentifier}))
        {
            return std::make_unique<Expression>(
                Expressions::Variable {previous()});
        }

        if (match({TokenType::eLeftParen}))
        {
            std::unique_ptr<Expression> expr = expression();
            consume(TokenType::eRightParen, "Expected ')' after expression");
            return std::make_unique<Expression>(
                Expressions::Grouping {std::move(expr)});
        }
        throw ParserError(peek(), "Expected expression");
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

    auto Parser::consume(TokenType tokenType) -> Token*
    {
        if ((check(tokenType)))
        {
            return &advance();
        };
        return nullptr;
    }

    auto Parser::consume(TokenType tokenType, const std::string& message)
        -> Token&
    {
        if (check(tokenType))
        {
            return advance();
        }
        throw ParserError(peek(), message);
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
