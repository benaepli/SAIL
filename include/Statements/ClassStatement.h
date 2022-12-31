#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct Class
    {
        Token name;
        std::shared_ptr<Expressions::Variable> superclass;
        std::vector<std::shared_ptr<Statements::Function>> methods;
    };
}  // namespace sail::Statements
