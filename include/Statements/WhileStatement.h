#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"

namespace sail::Statements
{
    struct While
    {
        sail::Expression condition;
        Statement body;
    };

}  // namespace sail::Statements
