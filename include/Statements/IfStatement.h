#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct If
    {
        std::unique_ptr<sail::Expression> condition;
        std::unique_ptr<Statement> thenBranch;
        std::unique_ptr<Statement> elseBranch;
    };
}  // namespace sail::Statements
