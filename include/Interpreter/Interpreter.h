#pragma once

#include "Expressions/Expressions.h"

namespace sail::Interpreter
{
    auto evaluate(Expression& expression) -> LiteralType;
    void interpret(Expression& expression);
}  // namespace sail::Interpreter
