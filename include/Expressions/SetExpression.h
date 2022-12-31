#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Set
    {
        Expression object;
        Token name;
        Expression value;

        auto operator==(const Set& other) const -> bool;
    };
}  // namespace sail::Expressions
