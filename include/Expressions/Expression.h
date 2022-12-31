#pragma once

#include <memory>

#include "Token/Token.h"

namespace sail
{
    namespace Expressions
    {
        struct Assignment;
        struct Binary;
        struct Call;
        struct Get;
        struct Grouping;
        struct Literal;
        struct Logical;
        struct Set;
        struct Super;
        struct This;
        struct Unary;
        struct Variable;
    }  // namespace Expressions

    using Expression = std::variant<std::shared_ptr<Expressions::Assignment>,
                                    std::shared_ptr<Expressions::Binary>,
                                    std::shared_ptr<Expressions::Call>,
                                    std::shared_ptr<Expressions::Get>,
                                    std::shared_ptr<Expressions::Grouping>,
                                    std::shared_ptr<Expressions::Literal>,
                                    std::shared_ptr<Expressions::Logical>,
                                    std::shared_ptr<Expressions::Set>,
                                    std::shared_ptr<Expressions::Super>,
                                    std::shared_ptr<Expressions::This>,
                                    std::shared_ptr<Expressions::Unary>,
                                    std::shared_ptr<Expressions::Variable>>;
}  // namespace sail