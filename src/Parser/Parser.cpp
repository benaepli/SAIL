#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>

#include "Parser/Parser.h"

#include "Errors/ParserError.h"
#include "Expressions/Expression.h"
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

    auto Parser::parse() -> std::vector<std::shared_ptr<Statement>>
    {
        std::vector<std::shared_ptr<Statement>> statements {};

        while (!isAtEnd())
        {
            std::shared_ptr<Statement> newStatement = statement();
            statements.push_back(newStatement);
        }

        return statements;
    }

    auto Parser::statement() -> std::shared_ptr<Statement>
    {
        return declaration();
    }

    auto Parser::declaration() -> std::shared_ptr<Statement>
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

    auto Parser::classDeclaration() -> std::shared_ptr<Statement>
    {
        Token& name = consume(TokenType::eIdentifier, "Expect class name.");

        std::shared_ptr<Expressions::Variable> superclass {};
        if (match({TokenType::eLess}))
        {
            consume(TokenType::eIdentifier, "Expect superclass name.");
            superclass = std::make_shared<Expressions::Variable>(previous());
        }

        consume(TokenType::eLeftBrace, "Expect '{' before class body.");

        std::vector<std::shared_ptr<Statements::Function>> methods {};
        while (!check(TokenType::eRightBrace) && !isAtEnd())
        {
            methods.push_back(functionStatement());
        }

        consume(TokenType::eRightBrace, "Expect '}' after class body.");
        return std::make_shared<Statements::Class>(name, superclass, methods);
    }

    auto Parser::varDeclaration() -> std::shared_ptr<Statement>
    {
        Token& name = consume(TokenType::eIdentifier, "Expected identifier after 'let'");

        std::shared_ptr<Expression> initializer {};
        if (match({TokenType::eEqual}))
        {
            initializer = expression();
        }

        Token& semicolon =
            consume(TokenType::eSemicolon, "Expected semicolon after variable declaration");

        return std::make_shared<Statements::Variable>(name, initializer);
    }

    auto Parser::blockStatement() -> std::shared_ptr<Statement>
    {
        std::vector<std::shared_ptr<Statement>> statements {};
        return std::make_shared<Statements::Block>(statements);
    }

    auto Parser::block() -> std::vector<std::shared_ptr<Statement>>
    {
        std::vector<std::shared_ptr<Statement>> statements {};

        while (!check(TokenType::eRightBrace) && !isAtEnd())
        {
            statements.push_back(declaration());
        }

        consume(TokenType::eRightBrace, "Expect '}' after block.");
        return statements;
    }

    auto Parser::expressionStatement() -> std::shared_ptr<Statement>
    {
        std::shared_ptr<Expression> newExpression = expression();
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
        std::vector<std::shared_ptr<Statement>> body = block();

        bool possibleInitializer = name.lexeme == "init";

        return std::make_shared<Statements::Function>(name, parameters, body, possibleInitializer);
    }

    auto Parser::returnStatement() -> std::shared_ptr<Statement>
    {
        Token& keyword = previous();
        std::shared_ptr<Expression> value {};
        if (!check(TokenType::eSemicolon))
        {
            value = expression();
        }

        consume(TokenType::eSemicolon, "Expected semicolon after return value");
        return std::make_shared<Statements::Return>(keyword, value);
    }

    auto Parser::ifStatement() -> std::shared_ptr<Statement>
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'if'");
        std::shared_ptr<Expression> condition = expression();
        consume(TokenType::eRightParen, "Expected ')' after if condition");

        std::shared_ptr<Statement> thenBranch = statement();
        std::shared_ptr<Statement> elseBranch {};
        if (match({TokenType::eElse}))
        {
            elseBranch = statement();
        }

        return std::make_shared<Statements::If>(condition, thenBranch, elseBranch);
    }

    auto Parser::whileStatement() -> std::shared_ptr<Statement>
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'while'");
        std::shared_ptr<Expression> condition = expression();
        consume(TokenType::eRightParen, "Expected ')' after while condition");
        std::shared_ptr<Statement> body = statement();

        return std::make_shared<Statements::While>(condition, body);
    }

    auto Parser::forStatement() -> std::shared_ptr<Statement>
    {
        consume(TokenType::eLeftParen, "Expected '(' after 'for'");

        std::shared_ptr<Statement> initializer {};
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

        std::shared_ptr<Expression> condition {};
        if (!check(TokenType::eSemicolon))
        {
            condition = expression();
        }
        consume(TokenType::eSemicolon, "Expected ';' after loop condition");

        std::shared_ptr<Expression> increment {};
        if (!check(TokenType::eRightParen))
        {
            increment = expression();
        }
        consume(TokenType::eRightParen, "Expected ')' after for clauses");

        std::shared_ptr<Statement> body = statement();

        if (increment != nullptr)
        {
            std::vector<std::shared_ptr<Statement>> bodyStatements {};
            bodyStatements.push_back(body);
            bodyStatements.push_back(std::make_shared<Statements::Expression>(increment));
            body = std::make_shared<Statements::Block>(bodyStatements);
        }

        if (condition == nullptr)
        {
            condition = std::make_shared<Expressions::Literal>(LiteralType {true});
        }

        body = std::make_shared<Statements::While>(condition, body);

        if (initializer != nullptr)
        {
            std::vector<std::shared_ptr<Statement>> bodyStatements {};
            bodyStatements.push_back(initializer);
            bodyStatements.push_back(body);
            body = std::make_shared<Statements::Block>(bodyStatements);
        }

        return body;
    }

    auto Parser::expression() -> std::shared_ptr<Expression>
    {
        return assignment();
    }

    auto Parser::assignment() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = orExpression();

        if (match({TokenType::eEqual}))
        {
            Token& equals = previous();
            std::shared_ptr<Expression> value = assignment();

            if (auto* variable = dynamic_cast<Expressions::Variable*>(expression.get()))
            {
                Token& name = variable->name;
                return std::make_shared<Expressions::Assignment>(name, value);
            }
            if (auto* get = dynamic_cast<Expressions::Get*>(expression.get()))
            {
                return std::make_shared<Expressions::Set>(get->object, get->name, value);
            }
            throw ParserError(equals, "Invalid assignment target");
        }

        return expression;
    }

    auto Parser::orExpression() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = andExpression();

        while (match({TokenType::eOr}))
        {
            Token& oper = previous();
            std::shared_ptr<Expression> right = andExpression();
            expression = std::make_shared<Expressions::Logical>(expression, oper, right);
        }

        return expression;
    }

    auto Parser::andExpression() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = equality();

        while (match({TokenType::eAnd}))
        {
            Token& oper = previous();
            std::shared_ptr<Expression> right = equality();
            expression = std::make_shared<Expressions::Logical>(expression, oper, right);
        }

        return expression;
    }

    auto Parser::equality() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = comparison();

        while (match({TokenType::eBangEqual, TokenType::eEqualEqual}))
        {
            Token& oper = previous();
            std::shared_ptr<Expression> right = comparison();
            expression = std::make_shared<Expressions::Binary>(expression, oper, right);
        }

        return expression;
    }

    auto Parser::comparison() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = term();

        while (match({TokenType::eGreater,
                      TokenType::eGreaterEqual,
                      TokenType::eLess,
                      TokenType::eLessEqual}))
        {
            Token& oper = previous();
            std::shared_ptr<Expression> right = term();
            expression = std::make_shared<Expressions::Binary>(expression, oper, right);
        }

        return expression;
    }

    auto Parser::term() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = factor();

        while (match({TokenType::eMinus, TokenType::ePlus}))
        {
            Token& oper = previous();
            std::shared_ptr<Expression> right = factor();
            expression = std::make_shared<Expressions::Binary>(expression, oper, right);
        }

        return expression;
    }

    auto Parser::factor() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = unary();

        while (match({TokenType::eSlash, TokenType::eStar}))
        {
            Token& oper = previous();
            std::shared_ptr<Expression> right = unary();
            expression = std::make_shared<Expressions::Binary>(expression, oper, right);
        }

        return expression;
    }

    auto Parser::unary() -> std::shared_ptr<Expression>
    {
        if (match({TokenType::eBang, TokenType::eMinus}))
        {
            Token& oper = previous();
            std::shared_ptr<Expression> right = unary();
            return std::make_shared<Expressions::Unary>(oper, right);
        }

        return call();
    }

    auto Parser::call() -> std::shared_ptr<Expression>
    {
        std::shared_ptr<Expression> expression = primary();

        while (true)
        {
            if (match({TokenType::eLeftParen}))
            {
                expression = finishCall(expression);
            }
            else if (match({TokenType::eDot}))
            {
                Token& name = consume(TokenType::eIdentifier, "Expect property name after '.'.");
                expression = std::make_shared<Expressions::Get>(expression, name);
            }
            else
            {
                break;
            }
        }

        return expression;
    }

    auto Parser::finishCall(std::shared_ptr<Expression>& callee) -> std::shared_ptr<Expression>
    {
        std::vector<std::shared_ptr<Expression>> arguments {};
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
        Token& paren = consume(TokenType::eRightParen, "Expect ')' after arguments");
        return std::make_shared<Expressions::Call>(callee, paren, arguments);
    }

    auto Parser::primary() -> std::shared_ptr<Expression>
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

        if (match({TokenType::eSuper}))
        {
            Token& keyword = previous();
            consume(TokenType::eDot, "Expected '.' after 'super'");
            Token& method = consume(TokenType::eIdentifier, "Expected superclass method name");
            return std::make_shared<Expressions::Super>(keyword, method);
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
            std::shared_ptr<Expression> expr = expression();
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
