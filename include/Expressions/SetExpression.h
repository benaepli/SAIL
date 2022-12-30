#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Set
    {
        std::unique_ptr<Expression> object;
        Token name;
        std::unique_ptr<Expression> value;

        auto operator==(const Set& other) const -> bool;
    };
}  // namespace sail::Expressions
