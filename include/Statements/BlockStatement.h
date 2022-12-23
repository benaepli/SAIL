#pragma once

#include <memory>
#include <vector>

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Block
    {
        std::vector<Statement> statements;
    };
}  // namespace sail::Statements
