#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Grouping
    {
        std::unique_ptr<Expression> expression;
    };
}  // namespace sail::Expressions
