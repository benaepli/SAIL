#pragma once

#include <memory>

#include "Expressions/Expressions.h"
#include "Statement.h"
#include "Token/Token.h"

namespace sail::Statements
{
    struct Return
    {
        std::unique_ptr<sail::Expression> value;
        Token keyword;
    };

}  // namespace sail::Statements
