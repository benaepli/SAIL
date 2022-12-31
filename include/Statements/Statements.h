#pragma once

#include "BlockStatement.h"
#include "ClassStatement.h"
#include "ExpressionStatement.h"
#include "FunctionStatement.h"
#include "IfStatement.h"
#include "ReturnStatement.h"
#include "VariableStatement.h"
#include "WhileStatement.h"

namespace sail
{
    inline auto statementIsNullptr(Statement& statement) -> bool
    {
        return std::visit([](auto& statement) { return statement == nullptr; }, statement);
    }
}  // namespace sail