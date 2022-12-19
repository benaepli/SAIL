#pragma once

#include <string>
#include <exception>
#include <optional>

namespace sail
{
    class SAIL_EXPORT SailException final : public std::exception
    {
    public:
        SailException(const std::string& message, size_t line);

        const char* what() const noexcept override;

    private:
        std::string message;
    };
}
