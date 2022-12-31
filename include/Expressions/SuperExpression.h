#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Super
    {
        Token keyword;
        Token method;

        auto operator==(const Super& other) const -> bool
        {
            return keyword == other.keyword && method == other.method;
        }
    };
}  // namespace sail::Expressions
