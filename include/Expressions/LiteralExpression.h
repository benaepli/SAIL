#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Literal
    {
        LiteralType literal;

        auto operator==(const Literal& other) const -> bool
        {
            return literal == other.literal;
        }
    };
}  // namespace sail::Expressions
