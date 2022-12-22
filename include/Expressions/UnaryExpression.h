#pragma once

#include "Expression.h"

namespace sail::Expressions
{
    struct Unary
    {
        std::unique_ptr<Expression> right;
        Token oper;
    };
}  // namespace sail::Expressions
