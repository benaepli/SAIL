#include "Errors/Errors.h"
#include "fmt/format.h"

namespace sail
{
    SailException::SailException(const std::string& message, size_t line)
        : message(fmt::format("Error at line {}: {}", line, message)) {}

    const char* SailException::what() const noexcept
    {
        return message.c_str();
    }
}
