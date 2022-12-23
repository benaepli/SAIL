#pragma once

#include <memory>

#include "Expressions/Expression.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Print  // will be removed later, only for debugging
    {
        std::unique_ptr<sail::Expression> expression;
    };
}  // namespace sail::Statements
