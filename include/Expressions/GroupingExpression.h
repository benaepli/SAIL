#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Grouping
    {
        std::unique_ptr<Expression> expression;

        auto operator==(const Grouping& other) const -> bool;
    };
}  // namespace sail::Expressions
