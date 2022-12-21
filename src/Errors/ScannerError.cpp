#include "Errors/ScannerError.h"

#include "fmt/format.h"

namespace sail
{
    ScannerError::ScannerError(const std::string& message, size_t line)
    {
        _message = fmt::format("Scan error at line {}: {}", line, message);
    }

    auto ScannerError::what() const noexcept -> const char*
    {
        return _message.c_str();
    }

}  // namespace sail