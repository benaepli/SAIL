#pragma once

#include <memory>

#include "Statement.h"
#include "Token/Token.h"

namespace sail::Statements
{
    struct Function
    {
        Token name;
        std::vector<Token> parameters;
        std::vector<Statement> body;
    };
}  // namespace sail::Statements
