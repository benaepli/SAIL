#pragma once

#include "Expressions/Expression.h"

namespace sail::Statements
{
    struct Variable
    {
        Token name;
        sail::Expression initializer;
    };
}  // namespace sail::Statements