#pragma once

#include <exception>
#include <string>

namespace sail
{
    class ScannerError : public std::exception
    {
      public:
        ScannerError(const std::string& message, size_t line);

        auto what() const noexcept -> const char* override;

      private:
        std::string _message;
    };
}  // namespace sail