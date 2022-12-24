#pragma once

#include <memory>
#include <vector>

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Block
    {
        std::vector<std::unique_ptr<sail::Statement>> statements;
    };
}  // namespace sail::Statements
