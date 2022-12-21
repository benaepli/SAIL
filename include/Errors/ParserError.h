#pragma once

#include "Token/Token.h"

namespace sail
{
    class ParserError : public std::exception
    {
      public:
        ParserError(Token token, const std::string& message);

        auto what() const noexcept -> const char* override;

      private:
        std::string _message;
    };
}  // namespace sail