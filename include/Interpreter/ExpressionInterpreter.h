#pragma once

#include "Expressions/Expressions.h"

namespace sail::Interpreter
{
    auto evaluate(Expression& expression) -> LiteralType;
}  // namespace sail::Interpreter
