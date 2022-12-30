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
        struct This;
        struct Unary;
        struct Variable;
    }  // namespace Expressions

    using Expression = std::variant<Expressions::Assignment,
                                    Expressions::Binary,
                                    Expressions::Call,
                                    Expressions::Get,
                                    Expressions::Grouping,
                                    Expressions::Literal,
                                    Expressions::Logical,
                                    Expressions::Set,
                                    Expressions::This,
                                    Expressions::Unary,
                                    Expressions::Variable>;
}  // namespace sail