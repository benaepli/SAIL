#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Grouping
    {
        Expression expression;

        auto operator==(const Grouping& other) const -> bool;
    };
}  // namespace sail::Expressions
