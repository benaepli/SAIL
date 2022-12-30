#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Class
    {
        Token name;
        std::vector<std::unique_ptr<sail::Statement>> methods;
    };
}  // namespace sail::Statements
