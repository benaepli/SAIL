#include <catch2/catch_test_macros.hpp>

#include "Scanner/Token.h"

TEST_CASE("Token ostream", "[Token]")
{
    using namespace sail;

    Token token{ TokenType::eLeftParen, "(", 1 };
    std::ostringstream os1;
    os1 << token;
    REQUIRE(os1.str() == "eLeftParen ( ");
}