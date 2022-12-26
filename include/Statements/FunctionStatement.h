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
        std::vector<std::unique_ptr<Statement>> body;
    };
}  // namespace sail::Statements
