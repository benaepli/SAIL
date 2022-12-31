#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Get
    {
        Expression object;
        Token name;

        auto operator==(const Get& other) const -> bool;
    };
}  // namespace sail::Expressions
