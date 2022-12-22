#pragma once

#include <vector>

#include "Expressions/Expressions.h"
#include "Statements/Statements.h"

namespace sail::Interpreter
{
    void execute(Statement& statement);
    void interpret(std::vector<Statement>& statements);
}  // namespace sail::Interpreter
