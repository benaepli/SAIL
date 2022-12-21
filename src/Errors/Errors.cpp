#include "Errors/Errors.h"

#include "Token/Token.h"
#include "fmt/format.h"

namespace sail
{
    SailException::SailException(const std::string& message, size_t line)
        : _message(fmt::format("Error at line {}: {}", line, message))
    {
    }

    SailException::SailException(const Token& token, const std::string& message)
    {
        if (token.type == TokenType::eEndOfFile)
        {
            _message = fmt::format("Error at end: {}", message);
        }
        else
        {
            _message = fmt::format("Error at line {}: {}", token.line, message);
        }
    }

    auto SailException::what() const noexcept -> const char*
    {
        return _message.c_str();
    }
}  // namespace sail
