#pragma once

#include <memory>
#include <vector>

#include "Expression.h"

namespace sail::Expressions
{
    struct Call
    {
        Expression callee;
        std::vector<Expression> arguments;
        Token paren;

        auto operator==(const Call& other) const -> bool;
    };
}  // namespace sail::Expressions
