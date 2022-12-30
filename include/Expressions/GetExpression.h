#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Get
    {
        std::unique_ptr<Expression> object;
        Token name;

        auto operator==(const Get& other) const -> bool;
    };
}  // namespace sail::Expressions
