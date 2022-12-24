#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct While
    {
        std::unique_ptr<sail::Expression> condition;
        std::unique_ptr<Statement> body;
    };

}  // namespace sail::Statements
