#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Binary
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        Token oper;

        auto operator==(const Binary& other) const -> bool;
    };
}  // namespace sail::Expressions
