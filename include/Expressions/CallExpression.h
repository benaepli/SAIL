#pragma once

#include <memory>
#include <vector>

#include "Expression.h"

namespace sail::Expressions
{
    struct Call
    {
        std::unique_ptr<Expression> callee;
        std::vector<std::unique_ptr<Expression>> arguments;
        Token paren;

        auto operator==(const Call& other) const -> bool;
    };
}  // namespace sail::Expressions
