#pragma once

#include "Token/LiteralType.h"
#include "Token/Token.h"
#include "unordered_dense.h"

namespace sail
{
    class Environment
    {
      public:
        auto get(Token name) -> LiteralType&;

      private:
    };
}  // namespace sail
