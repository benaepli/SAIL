#pragma once

#include <iostream>
#include <string>
#include <variant>

#include "Token/LiteralType.h"

namespace sail
{

    enum class TokenType
    {
        // Single-character tokens.
        eLeftParen,
        eRightParen,
        eLeftBrace,
        eRightBrace,
        eComma,
        eDot,
        eMinus,
        ePlus,
        eSemicolon,
        eSlash,
        eStar,

        // One or two character tokens.
        eBang,
        eBangEqual,
        eEqual,
        eEqualEqual,
        eGreater,
        eGreaterEqual,
        eLess,
        eLessEqual,

        // Literals.
        eIdentifier,
        eString,
        eNumber,

        // Keywords.
        eAnd,
        eClass,
        eElse,
        eFalse,
        eFun,
        eFor,
        eIf,
        eNull,
        eOr,
        ePrint,
        eReturn,
        eSuper,
        eThis,
        eTrue,
        eLet,
        eWhile,

        eBitwiseOr,
        eBitwiseAnd,
        eBitwiseXor,
        eBitwiseNot,

        eEndOfFile
    };

    struct Token
    {
        TokenType type;
        std::string lexeme;
        LiteralType literal = "";
        size_t line;

        auto hasLiteral() const -> bool;

        friend auto operator<<(std::ostream& os, const Token& token)
            -> std::ostream&;
    };
}  // namespace sail
