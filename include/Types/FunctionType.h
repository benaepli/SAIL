#pragma once

#include <memory>
#include <vector>

#include "Types/Value.h"

namespace sail::Types
{
    struct Function
    {
        std::vector<Value> parameters;
        Value returnValue;
    };
}  // namespace sail::Types