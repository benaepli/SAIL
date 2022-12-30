#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Assignment
    {
        std::unique_ptr<Expression> value;
        Token name;

        auto operator==(const Assignment& other) const -> bool;
    };
}  // namespace sail::Expressions
