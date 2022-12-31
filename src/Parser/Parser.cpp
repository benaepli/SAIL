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
#include "Types/NullType.h"
#include "Types/Types.h"
#include "fmt/format.h"

namespace sail
{

    Parser::Parser(std::vector<Token>& tokens)
        : _tokens(tokens)
    {
    }

    auto Parser::parse() -> std::vector<Statement>
    {
        std::vector<Statement> statements {};

        while (!isAtEnd())
        {
            Statement newStatement = statement();
            statements.push_back(newStatement);
        }

        return statements;
    }

    auto Parser::statement() -> Statement
    {
        return declaration();
    }

    auto Parser::declaration() -> Statement
    {
        if (match({TokenType::eClass}))
        {
            return classDeclaration();
        }

        if (match({TokenType::eLet}))
        {
            return varDeclaration();
        }

        if (match({TokenType::eFn}))
        {
            return functionStatement();
        }

        if (match({TokenType::eLeftBrace}))
        {
            return blockStatement();
        }

        if (match({TokenType::eReturn}))
        {
            return returnStatement();
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

    auto Parser::classDeclaration() -> Statement
    {
        Token name = consume(TokenType::eIdentifier, "Expect class name.");
        consume(TokenType::eLeftBrace, "Expect '{' before class body.");

        std::vector<std::shared_ptr<Statements::Function>> methods {};
        while (!check(TokenType::eRightBrace) && !isAtEnd())
        {
            methods.push_back(functionStatement());
        }

        consume(TokenType::eRightBrace, "Expect '}' after class body.");
        return std::make_shared<Statements::Class>(name, methods);
    }

    auto Parser::varDeclaration() -> Statement
    {
        Token& name = consume(TokenType::eIdentifier, "Expected identifier after 'let'");

        Expression initializer {};
        if (match({TokenType::eEqual}))
        {
            initializer = expression();
        }

        Token& semicolon =
            consume(TokenType::eSemicolon, "Expected semicolon after variable declaration");

        return std::make_shared<Statements::Variable>(name, initializer);
    }

    auto Parser::blockStatement() -> Statement
    {
        std::vector<Statement> statements = block();
        return std::make_shared<Statements::Block>(statements);
    }

    auto Parser::block() -> std::vector<Statement>
    {
        std::vector<Statement> statements {};

        while (!check(TokenType::eRightBrace) && !isAtEnd())
        {
            statements.push_back(declaration());
        }

        consume(TokenType::eRightBrace, "Expected '}' after block");
        return statements;
    }

    auto Parser::expressionStatement() -> Statement
    {
        Expression newExpression = expression();
        consume(TokenType::eSemicolon, "Expected semicolon after value");
        return std::make_shared<Statements::Expression>(newExpression);
    }

    auto Parser::functionStatement() -> std::shared_ptr<Statements::Function>
    {
        Token& name = consume(TokenType::eIdentifier, "Expected identifier after 'fun'");
        consume(TokenType::eLeftParen, "Expected '(' after function name");
        std::vector<Token> parameters {};
        if (!check(TokenType::eRightParen))
        {
            do
            {
                if (parameters.size() >= 255)
                {
                    throw ParserError(peek(), "Cannot have more than 255 parameters");
                }
                parameters.push_back(consume(TokenType::eIdentifier, "Expected parameter name"));
            } while (match({TokenType::eComma}));
        }
        consume(TokenType::eRightParen, "Expected ')' after parameters");
        consume(TokenType::eLeftBrace, "Expected '{' before function body");
        std::vector<Statement> body = block();
        return std::make_shared<Statements::Function>(name, parameters, body);
    }

    auto Parser::returnStatement() -> Statement
    {
        Token& keyword = previous();
        Expression value {};
        if (!check(TokenType::eSemicolon))
        {
            value = expression();
        }

        consume(TokenType::eSemicolon, "Expected semicolon after return value");
        return std::make_shared<Statements::Return>(value, keyword);
    }

    auto Parser::ifStatement() -> Statement
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'if'");
        Expression condition = expression();
        consume(TokenType::eRightParen, "Expected ')' after if condition");

        Statement thenBranch = statement();
        Statement elseBranch {};
        if (match({TokenType::eElse}))
        {
            elseBranch = statement();
        }

        return std::make_shared<Statements::If>(condition, thenBranch, elseBranch);
    }

    auto Parser::whileStatement() -> Statement
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'while'");
        Expression condition = expression();
        consume(TokenType::eRightParen, "Expected ')' after while condition");
        Statement body = statement();

        return std::make_shared<Statements::While>(condition, body);
    }

    auto Parser::forStatement() -> Statement
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'for'");

        Statement initializer = std::shared_ptr<Statements::Block> {nullptr};
        if (match({TokenType::eSemicolon}))
        {
            initializer = {};
        }
        else if (match({TokenType::eLet}))
        {
            initializer = varDeclaration();
        }
        else
        {
            initializer = expressionStatement();
        }

        Expression condition {};
        if (!check(TokenType::eSemicolon))
        {
            condition = expression();
        }
        consume(TokenType::eSemicolon, "Expected ';' after loop condition");

        Expression increment {};
        if (!check(TokenType::eRightParen))
        {
            increment = expression();
        }
        consume(TokenType::eRightParen, "Expected ')' after for clauses");

        Statement body = statement();

        if (!expressionIsNullptr(increment))
        {
            std::vector<Statement> statements {};
            statements.push_back(body);
            statements.emplace_back(std::make_shared<Statements::Expression>(increment));
            body = std::make_shared<Statements::Block>(statements);
        }

        if (expressionIsNullptr(condition))
        {
            condition = std::make_shared<Expressions::Literal>(LiteralType {true});
        }

        body = std::make_shared<Statements::While>(condition, body);

        if (!statementIsNullptr(initializer))
        {
            std::vector<Statement> statements {};
            statements.push_back(initializer);
            statements.push_back(body);
            body = std::make_shared<Statements::Block>(statements);
        }

        return body;
    }

    auto Parser::expression() -> Expression
    {
        return assignment();
    }

    auto Parser::assignment() -> Expression
    {
        Expression expr = orExpression();

        if (match({TokenType::eEqual}))
        {
            Token equals = previous();
            Expression value = assignment();

            if (auto* variable = std::get_if<std::shared_ptr<Expressions::Variable>>(&expr))
            {
                Token& name = variable->get()->name;
                return std::make_shared<Expressions::Assignment>(value, name);
            }
            if (auto* get = std::get_if<std::shared_ptr<Expressions::Get>>(&expr))
            {
                auto expr = *get;
                return std::make_shared<Expressions::Set>(expr->object, expr->name, value);
            }
            throw ParserError(equals, "Invalid assignment target");
        }

        return expr;
    }

    auto Parser::orExpression() -> Expression
    {
        Expression expr = andExpression();

        while (match({TokenType::eOr}))
        {
            Token oper = previous();
            Expression right = andExpression();
            expr = std::make_shared<Expressions::Logical>(expr, right, oper);
        }

        return expr;
    }

    auto Parser::andExpression() -> Expression
    {
        Expression expr = equality();

        while (match({TokenType::eAnd}))
        {
            Token oper = previous();
            Expression right = equality();
            expr = std::make_shared<Expressions::Logical>(expr, right, oper);
        }

        return expr;
    }

    auto Parser::equality() -> Expression
    {
        Expression expr = comparison();

        while (match({TokenType::eBangEqual, TokenType::eEqualEqual}))
        {
            Token oper = previous();
            Expression right = comparison();
            expr = std::make_shared<Expressions::Binary>(expr, right, oper);
        }

        return expr;
    }

    auto Parser::comparison() -> Expression
    {
        Expression expr = term();

        while (match({TokenType::eGreater,
                      TokenType::eGreaterEqual,
                      TokenType::eLess,
                      TokenType::eLessEqual}))
        {
            Token oper = previous();
            Expression right = term();
            expr = std::make_shared<Expressions::Binary>(expr, right, oper);
        }

        return expr;
    }

    auto Parser::term() -> Expression
    {
        Expression expr = factor();

        while (match({TokenType::eMinus, TokenType::ePlus}))
        {
            Token oper = previous();
            Expression right = factor();
            expr = std::make_shared<Expressions::Binary>(expr, right, oper);
        }

        return expr;
    }

    auto Parser::factor() -> Expression
    {
        Expression expr = unary();

        while (match({TokenType::eSlash, TokenType::eStar}))
        {
            Token oper = previous();
            Expression right = unary();
            expr = std::make_shared<Expressions::Binary>(expr, right, oper);
        }

        return expr;
    }

    auto Parser::unary() -> Expression
    {
        if (match({TokenType::eBang, TokenType::eMinus}))
        {
            Token oper = previous();
            Expression right = unary();
            return std::make_shared<Expressions::Unary>(right, oper);
        }

        return call();
    }

    auto Parser::call() -> Expression
    {
        Expression expr = primary();

        while (true)
        {
            if (match({TokenType::eLeftParen}))
            {
                expr = finishCall(expr);
            }
            else if (match({TokenType::eDot}))
            {
                Token name = consume(TokenType::eIdentifier, "Expect property name after '.'.");
                expr = std::make_shared<Expressions::Get>(expr, name);
            }
            else
            {
                break;
            }
        }

        return expr;
    }

    auto Parser::finishCall(Expression callee) -> Expression
    {
        std::vector<Expression> arguments {};
        if (!check(TokenType::eRightParen))
        {
            do
            {
                if (arguments.size() >= 255)
                {
                    throw ParserError(peek(), "Can't have more than 255 arguments");
                }
                arguments.push_back(expression());
            } while (match({TokenType::eComma}));
        }
        Token paren = consume(TokenType::eRightParen, "Expect ')' after arguments");
        return std::make_shared<Expressions::Call>(callee, arguments, paren);
    }

    auto Parser::primary() -> Expression
    {
        if (match({TokenType::eFalse}))
        {
            return std::make_shared<Expressions::Literal>(false);
        }
        if (match({TokenType::eTrue}))
        {
            return std::make_shared<Expressions::Literal>(true);
        }
        if (match({TokenType::eNull}))
        {
            return std::make_shared<Expressions::Literal>(Types::Null {});
        }

        if (match({TokenType::eNumber, TokenType::eString}))
        {
            return std::make_shared<Expressions::Literal>(previous().literal);
        }

        if (match({TokenType::eThis}))
        {
            return std::make_shared<Expressions::This>(previous());
        }

        if (match({TokenType::eIdentifier}))
        {
            return std::make_shared<Expressions::Variable>(previous());
        }

        if (match({TokenType::eLeftParen}))
        {
            Expression expr = expression();
            consume(TokenType::eRightParen, "Expected ')' after expression");
            return std::make_shared<Expressions::Grouping>(expr);
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

    auto Parser::consume(TokenType tokenType, const std::string& message) -> Token&
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
                case TokenType::eFn:
                case TokenType::eLet:
                case TokenType::eFor:
                case TokenType::eIf:
                case TokenType::eWhile:
                case TokenType::eReturn:
                    return;

                default:;
            }

            advance();
        }
    }

}  // namespace sail
