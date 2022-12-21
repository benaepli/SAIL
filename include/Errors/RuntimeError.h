#pragma once

#include "Token/Token.h"

namespace sail
{
    class RuntimeError : public std::exception
    {
      public:
        RuntimeError(Token token, const std::string& message);

        auto what() const noexcept -> const char* override;

      private:
        std::string _message;
    };
}  // namespace sail