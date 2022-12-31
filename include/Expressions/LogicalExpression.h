#pragma once

#include "Expression.h"
#include "Token/Token.h"

namespace sail::Expressions
{
    struct Logical
    {
        Expression left;
        Expression right;
        Token oper;

        auto operator==(const Logical& other) const -> bool;
    };
}  // namespace sail::Expressions
