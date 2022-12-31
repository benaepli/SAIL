#pragma once

#include "AssignmentExpression.h"
#include "BinaryExpression.h"
#include "CallExpression.h"
#include "GetExpression.h"
#include "GroupingExpression.h"
#include "LiteralExpression.h"
#include "LogicalExpression.h"
#include "SetExpression.h"
#include "SuperExpression.h"
#include "ThisExpression.h"
#include "UnaryExpression.h"
#include "VariableExpression.h"
#include "utils/Overload.h"
#include "utils/hash.h"

// SAIL_HASH_DECLARATION(sail::Expressions::Assignment)
// SAIL_HASH_DECLARATION(sail::Expressions::Binary)
// SAIL_HASH_DECLARATION(sail::Expressions::Call)
// SAIL_HASH_DECLARATION(sail::Expressions::Get)
// SAIL_HASH_DECLARATION(sail::Expressions::Grouping)
// SAIL_HASH_DECLARATION(sail::Expressions::Literal)
// SAIL_HASH_DECLARATION(sail::Expressions::Logical)
// SAIL_HASH_DECLARATION(sail::Expressions::Set)
// SAIL_HASH_DECLARATION(sail::Expressions::Super)
// SAIL_HASH_DECLARATION(sail::Expressions::This)
// SAIL_HASH_DECLARATION(sail::Expressions::Unary)
// SAIL_HASH_DECLARATION(sail::Expressions::Variable)
// SAIL_HASH_DECLARATION(sail::Expression)

namespace sail
{
    inline auto expressionIsNullptr(const Expression& expression) -> bool
    {
        return std::visit([](auto& expression) { return expression == nullptr; }, expression);
    }
}  // namespace sail