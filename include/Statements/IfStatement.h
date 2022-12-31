#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct If
    {
        sail::Expression condition;
        Statement thenBranch;
        Statement elseBranch;
    };
}  // namespace sail::Statements
