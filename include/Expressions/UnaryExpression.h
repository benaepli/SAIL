#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Unary
    {
        Expression right;
        Token oper;

        auto operator==(const Unary& other) const -> bool;
    };
}  // namespace sail::Expressions
