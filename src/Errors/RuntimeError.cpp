#include "Errors/RuntimeError.h"

#include "fmt/format.h"

namespace sail
{
    RuntimeError::RuntimeError(Token token, const std::string& message)
    {
        _message =
            fmt::format("Runtime error at line {}: {}", token.line, message);
    }

    auto RuntimeError::what() const noexcept -> const char*
    {
        return _message.c_str();
    }
}  // namespace sail