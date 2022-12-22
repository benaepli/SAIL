#include "Errors/ParserError.h"

#include "fmt/format.h"

namespace sail
{
    ParserError::ParserError(const Token& token, const std::string& message)
    {
        _message =
            fmt::format("Parse error at line {}: {}", token.line, message);
    }

    auto ParserError::what() const noexcept -> const char*
    {
        return _message.c_str();
    }
}  // namespace sail