#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Variable
    {
        Token name;

        auto operator==(const Variable& other) const -> bool
        {
            return name == other.name;
        }
    };
}  // namespace sail::Expressions
