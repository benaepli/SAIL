#include <unordered_map>

#include "Scanner/Scanner.h"

#include <fmt/format.h>

#include "Errors/ScannerError.h"

namespace sail
{
    // keywords map
    static const std::unordered_map<std::string_view, TokenType> KEYWORDS {
        {"class", TokenType::eClass},
        {"else", TokenType::eElse},
        {"false", TokenType::eFalse},
        {"for", TokenType::eFor},
        {"fun", TokenType::eFun},
        {"if", TokenType::eIf},
        {"let", TokenType::eLet},
        {"null", TokenType::eNull},
        {"return", TokenType::eReturn},
        {"super", TokenType::eSuper},
        {"this", TokenType::eThis},
        {"true", TokenType::eTrue},
        {"while", TokenType::eWhile},
    };

    Scanner::Scanner(std::string source, std::vector<Token>& tokens)
        : _source(std::move(source))
        , _tokens(tokens)
    {
    }

    auto Scanner::scanTokens() -> std::vector<Token>
    {
        while (!isAtEnd())
        {
            _start = _current;
            scanToken();
        }

        Token token {
            .type = TokenType::eEndOfFile,
            .lexeme = "",
            .literal = "",
            .line = _line,
        };
        _tokens.push_back(token);
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
        if (isAtEnd())
        {
            return false;
        }
        if (_source[_current] != expected)
        {
            return false;
        }

        _current++;
        return true;
    }

    auto Scanner::peek() const -> char
    {
        if (isAtEnd())
        {
            return '\0';
        }
        return _source[_current];
    }

    auto Scanner::peekNext() -> char
    {
        if (_current + 1 >= _source.length())
        {
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
        Token token {
            .type = type,
            .lexeme = text,
            .literal = literal,
            .line = _line,
        };
        _tokens.push_back(token);
    }

    void Scanner::addToken(TokenType type, double literal)
    {
        std::string const text = _source.substr(_start, _current - _start);
        Token token {
            .type = type,
            .lexeme = text,
            .literal = literal,
            .line = _line,
        };
        _tokens.push_back(token);
    }

    void Scanner::scanToken()
    {
        char c = advance();
        switch (c)
        {
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
                addToken(TokenType::eRightBrace);
                break;
            case ',':
                addToken(TokenType::eComma);
                break;
            case '.':
                addToken(TokenType::eDot);
                break;
            case '-':
                addToken(TokenType::eMinus);
                break;
            case '+':
                addToken(TokenType::ePlus);
                break;
            case ';':
                addToken(TokenType::eSemicolon);
                break;
            case '*':
                addToken(TokenType::eStar);
                break;
            case '!':
                addToken(match('=') ? TokenType::eBangEqual : TokenType::eBang);
                break;
            case '=':
                addToken(match('=') ? TokenType::eEqualEqual
                                    : TokenType::eEqual);
                break;
            case '<':
                addToken(match('=') ? TokenType::eLessEqual : TokenType::eLess);
                break;
            case '>':
                addToken(match('=') ? TokenType::eGreaterEqual
                                    : TokenType::eGreater);
                break;
            case '/':
                if (match('/'))
                {
                    while (peek() != '\n' && !isAtEnd())
                    {
                        advance();
                    }
                }
                else
                {
                    addToken(TokenType::eSlash);
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
                if (match('|'))
                {
                    addToken(TokenType::eOr);
                }
                else
                {
                    addToken(TokenType::eBitwiseOr);
                }
                break;
            case '&':
                if (match('&'))
                {
                    addToken(TokenType::eAnd);
                }
                else
                {
                    addToken(TokenType::eBitwiseAnd);
                }
                break;
            case '^':
                addToken(TokenType::eBitwiseXor);
                break;
            case '~':
                addToken(TokenType::eBitwiseNot);
                break;
            default:
                if (isDigit(c))
                {
                    number();
                }
                else if (isAlpha(c))
                {
                    identifier();
                }
                else
                {
                    throw ScannerError("Unexpected character", _line);
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
        while (peek() != '"' && !isAtEnd())
        {
            if (peek() == '\n')
            {
                _line++;
            }
            advance();
        }

        if (isAtEnd())
        {
            throw ScannerError("Unterminated string", _line);
        }

        advance();

        const std::string value =
            _source.substr(_start + 1, _current - _start - 2);
        addToken(TokenType::eString, value);
    }

    void Scanner::number()
    {
        while (isDigit(peek()))
        {
            advance();
        }

        if (peek() == '.' && isDigit(peekNext()))
        {
            advance();

            while (isDigit(peek()))
            {
                advance();
            }
        }
        const double value =
            std::stod(_source.substr(_start, _current - _start));
        addToken(TokenType::eNumber, value);
    }

    void Scanner::identifier()
    {
        while (isAlphaNumeric(peek()))
        {
            advance();
        }

        std::string const text = _source.substr(_start, _current - _start);

        auto it = KEYWORDS.find(text);
        if (it != KEYWORDS.end())
        {
            addToken(it->second);
        }
        else
        {
            addToken(TokenType::eIdentifier);
        }
    }
}  // namespace sail
