#pragma once

#include <exception>
#include <optional>
#include <string>

namespace sail
{
    class Token;

    class SAIL_EXPORT SailException : public std::exception
    {
      public:
        SailException(const std::string& message, size_t line);
        SailException(const Token& token, const std::string& message);

        auto what() const noexcept -> const char* override;

      private:
        std::string _message;
    };
}  // namespace sail
