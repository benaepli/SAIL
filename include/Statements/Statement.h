#pragma once

#include <memory>
#include <variant>

namespace sail
{
    namespace Statements
    {
        struct Block;
        struct Class;
        struct Expression;
        struct Function;
        struct If;
        struct Return;
        struct Variable;
        struct While;
    }  // namespace Statements

    using Statement = std::variant<std::shared_ptr<Statements::Block>,
                                   std::shared_ptr<Statements::Class>,
                                   std::shared_ptr<Statements::Expression>,
                                   std::shared_ptr<Statements::Function>,
                                   std::shared_ptr<Statements::If>,
                                   std::shared_ptr<Statements::Return>,
                                   std::shared_ptr<Statements::Variable>,
                                   std::shared_ptr<Statements::While>>;
}  // namespace sail
