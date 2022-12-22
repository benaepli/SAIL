#pragma once

#include <memory>

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Expression
    {
        std::unique_ptr<sail::Expression> expression;
    };
}  // namespace sail::Statements
