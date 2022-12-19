#include "Scanner/Scanner.h"

#include "Errors/Errors.h"
#include <fmt/format.h>
#include <unordered_map>

namespace sail
{
    // keywords map
    static const std::unordered_map<std::string_view, TokenType> KEYWORDS{
        {"class", TokenType::CLASS},
        {"else", TokenType::ELSE},
        {"false", TokenType::FALSE},
        {"for", TokenType::FOR},
        {"fun", TokenType::FUN},
        {"if", TokenType::IF},
        {"let", TokenType::LET},
        {"null", TokenType::eNull},
        {"return", TokenType::RETURN},
        {"super", TokenType::SUPER},
        {"this", TokenType::THIS},
        {"true", TokenType::TRUE},
        {"while", TokenType::WHILE},
    };

    Scanner::Scanner(std::string source)
        : _source(std::move(source)) { }

    auto Scanner::scanTokens() -> std::vector<Token>
    {
        while (!isAtEnd()) {
            _start = _current;
            scanToken();
        }

        _tokens.emplace_back(TokenType::END_OF_FILE, "", "", _line);
        return _tokens;
    }

    auto Scanner::isAtEnd() const -> bool
    {
        return _current >= _source.length();
    }

    auto Scanner::advance() -> char
    {
        return _source[_current++];
    }

    auto Scanner::match(char expected) -> bool
    {
        if (isAtEnd()) {
            return false;
}
        if (_source[_current] != expected) {
            return false;
}

        _current++;
        return true;
    }

    auto Scanner::peek() const -> char
    {
        if (isAtEnd()) {
            return '\0';
}
        return _source[_current];
    }

    auto Scanner::peekNext() -> char
    {
        if (_current + 1 >= _source.length()) {
            return '\0';
}
        return _source[_current + 1];
    }

    void Scanner::addToken(TokenType type)
    {
        addToken(type, "");
    }

    void Scanner::addToken(TokenType type, const std::string& literal)
    {
        std::string const text = _source.substr(_start, _current - _start);
        _tokens.emplace_back(type, text, literal, _line);
    }

    void Scanner::scanToken()
    {
        char c = advance();
        switch (c) {
            case '(':
                addToken(TokenType::eLeftParen);
                break;
            case ')':
                addToken(TokenType::eRightParen);
                break;
            case '{':
                addToken(TokenType::eLeftBrace);
                break;
            case '}':
                addToken(TokenType::RIGHT_BRACE);
                break;
            case ',':
                addToken(TokenType::COMMA);
                break;
            case '.':
                addToken(TokenType::DOT);
                break;
            case '-':
                addToken(TokenType::MINUS);
                break;
            case '+':
                addToken(TokenType::PLUS);
                break;
            case ';':
                addToken(TokenType::SEMICOLON);
                break;
            case '*':
                addToken(TokenType::STAR);
                break;
            case '!':
                addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '=':
                addToken(match('=')
                    ? TokenType::EQUAL_EQUAL
                    : TokenType::EQUAL);
                break;
            case '<':
                addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                break;
            case '>':
                addToken(match('=')
                    ? TokenType::GREATER_EQUAL
                    : TokenType::GREATER);
                break;
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
}
                } else {
                    addToken(TokenType::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                _line++;
                break;
            case '"':
                string();
                break;
            case '|':
                if (match('|')) {
                    addToken(TokenType::OR);
                } else {
                    addToken(TokenType::BITWISE_OR);
                }
                break;
            case '&':
                if (match('&')) {
                    addToken(TokenType::AND);
                } else {
                    addToken(TokenType::BITWISE_AND);
                }
                break;
            case '^':
                addToken(TokenType::BITWISE_XOR);
                break;
            case '~':
                addToken(TokenType::BITWISE_NOT);
                break;
            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                } else {
                    throw SailException(
                        fmt::format("Unexpected character: {}.", c),
                        _line);
                }

        }
    }

    auto Scanner::isDigit(char c) -> bool
    {
        return c >= '0' && c <= '9';
    }

    auto Scanner::isAlpha(char c) -> bool
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    auto Scanner::isAlphaNumeric(char c) -> bool
    {
        return isAlpha(c) || isDigit(c);
    }

    void Scanner::string()
    {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') {
                _line++;
}
            advance();
        }

        if (isAtEnd()) {
            throw SailException("Unterminated string.", _line);
        }

        advance();

        const std::string value = _source.substr(_start + 1, _current - _start - 2);
        addToken(TokenType::STRING, value);
    }

    void Scanner::number()
    {
        while (isDigit(peek())) {
            advance();
}

        if (peek() == '.' && isDigit(peekNext())) {
            advance();

            while (isDigit(peek())) {
                advance();
}
        }

        addToken(TokenType::NUMBER, _source.substr(_start, _current - _start));
    }

    void Scanner::identifier()
    {
        while (isAlphaNumeric(peek())) {
            advance();
}

        std::string const text = _source.substr(_start, _current - _start);

        auto it = KEYWORDS.find(text);
        if (it != KEYWORDS.end()) {
            addToken(it->second);
        } else {
            addToken(TokenType::IDENTIFIER);
        }
    }
}
