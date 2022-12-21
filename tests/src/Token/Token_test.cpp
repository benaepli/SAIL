#include <sstream>

#include "Token/Token.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Token ostream", "[Token]")
{
    using namespace sail;

    Token const token {TokenType::eLeftParen, "(", "", 1};
    std::ostringstream os1;
    os1 << token;
    REQUIRE(os1.str() == "Token type eLeftParen with lexeme ( on line 1");
}

TEST_CASE("Literal ostream", "[Token]")
{
    using namespace sail;

    LiteralType const literal {std::string {"hello"}};
    std::ostringstream os1;
    os1 << literal;
    REQUIRE(os1.str() == "hello");
}