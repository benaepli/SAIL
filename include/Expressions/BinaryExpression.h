#pragma once

#include <memory>

#include "Expression.h"

namespace sail::Expressions
{
    struct Binary
    {
        Expression left;
        Expression right;
        Token oper;

        auto operator==(const Binary& other) const -> bool;
    };
}  // namespace sail::Expressions
