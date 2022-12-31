#pragma once

#include <memory>

#include "Expression.h"

namespace sail::Expressions
{
    struct Assignment
    {
        Expression value;
        Token name;

        auto operator==(const Assignment& other) const -> bool;
    };
}  // namespace sail::Expressions
