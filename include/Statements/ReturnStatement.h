#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"
#include "Token/Token.h"

namespace sail::Statements
{
    struct Return
    {
        sail::Expression value;
        Token keyword;
    };

}  // namespace sail::Statements
