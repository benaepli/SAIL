#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct This
    {
        Token keyword;

        auto operator==(const This& other) const -> bool
        {
            return keyword == other.keyword;
        }
    };
}  // namespace sail::Expressions
