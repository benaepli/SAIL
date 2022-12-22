#pragma once

#include "Expressions/Expression.h"

namespace sail::Statements
{
    struct Variable
    {
        Token name;
        std::unique_ptr<sail::Expression> initializer;
    };
}  // namespace sail::Statements