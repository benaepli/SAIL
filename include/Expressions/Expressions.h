#pragma once

#include "AssignmentExpression.h"
#include "BinaryExpression.h"
#include "CallExpression.h"
#include "GetExpression.h"
#include "GroupingExpression.h"
#include "LiteralExpression.h"
#include "LogicalExpression.h"
#include "SetExpression.h"
#include "ThisExpression.h"
#include "UnaryExpression.h"
#include "VariableExpression.h"
#include "utils/hash.h"

namespace std
{
    template<>
    struct hash<sail ::Expression>
    {
        auto operator()(const sail ::Expression& val) const -> size_t;
    };
}  // namespace std