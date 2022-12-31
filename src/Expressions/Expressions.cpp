#include "Expressions/Expressions.h"

#include "Expressions/Expression.h"
#include "utils/hash.h"

namespace
{
    using namespace sail;
    using namespace sail::Expressions;

    auto hashFunction(const Expressions::Assignment& assignment) -> size_t;
    auto hashFunction(const Expressions::Binary& binary) -> size_t;
    auto hashFunction(const Expressions::Call& call) -> size_t;
    auto hashFunction(const Expressions::Get& get) -> size_t;
    auto hashFunction(const Expressions::Grouping& grouping) -> size_t;
    auto hashFunction(const Expressions::Literal& literal) -> size_t;
    auto hashFunction(const Expressions::Logical& logical) -> size_t;
    auto hashFunction(const Expressions::Set& set) -> size_t;
    auto hashFunction(const Expressions::Super& super) -> size_t;
    auto hashFunction(const Expressions::This& thisExpr) -> size_t;
    auto hashFunction(const Expressions::Unary& unary) -> size_t;
    auto hashFunction(const Expressions::Variable& variable) -> size_t;

    auto hashFunction(const Expression& expression) -> size_t
    {
        return std::visit([](auto&& arg) { return hashFunction(*arg); }, expression);
    }

    auto hashFunction(const Expressions::Assignment& assignment) -> size_t
    {
        size_t seed = 0;
        if (!expressionIsNullptr(assignment.value))
        {
            utils::hashCombine(seed, hashFunction(assignment.value));
        }
        utils::hashCombine(seed, assignment.name);
        return seed;
    }

    auto hashFunction(const Expressions::Binary& binary) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(
            seed, hashFunction(binary.left), hashFunction(binary.right), binary.oper);
        return seed;
    }

    auto hashFunction(const Expressions::Call& call) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(call.callee));
        for (const auto& argument : call.arguments)
        {
            utils::hashCombine(seed, hashFunction(argument));
        }
        return seed;
    }

    auto hashFunction(const Expressions::Get& get) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(get.object), get.name);
        return seed;
    }

    auto hashFunction(const Expressions::Grouping& grouping) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(grouping.expression));
        return seed;
    }

    auto hashFunction(const Expressions::Literal& literal) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, literal.literal);
        return seed;
    }

    auto hashFunction(const Expressions::Logical& logical) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(
            seed, hashFunction(logical.left), hashFunction(logical.right), logical.oper);
        return seed;
    }

    auto hashFunction(const Expressions::Set& set) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(set.object), set.name, hashFunction(set.value));
        return seed;
    }

    auto hashFunction(const Expressions::Super& super) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, super.keyword, super.method);
        return seed;
    }

    auto hashFunction(const Expressions::This& thisExpr) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, thisExpr.keyword);
        return seed;
    }

    auto hashFunction(const Expressions::Unary& unary) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(unary.right), unary.oper);
        return seed;
    }

    auto hashFunction(const Expressions::Variable& variable) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, variable.name);
        return seed;
    }

}  // namespace

// SAIL_HASH_DEFINITION(sail::Expressions::Assignment, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Binary, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Call, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Get, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Grouping, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Literal, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Logical, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Set, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Super, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::This, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Unary, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expressions::Variable, hashFunction)
// SAIL_HASH_DEFINITION(sail::Expression, hashFunction)

namespace sail::Expressions
{
    auto Assignment::operator==(const Assignment& other) const -> bool
    {
        return (value == other.value) && (name == other.name);
    }

    auto Binary::operator==(const Binary& other) const -> bool
    {
        return (left == other.left) && (right == other.right) && (oper == other.oper);
    }

    auto Call::operator==(const Call& other) const -> bool
    {
        if (arguments.size() != other.arguments.size())
        {
            return false;
        }
        for (size_t i = 0; i < arguments.size(); ++i)
        {
            if (arguments[i] != other.arguments[i])
            {
                return false;
            }
        }
        return callee == other.callee;
    }

    auto Get::operator==(const Get& other) const -> bool
    {
        return (object == other.object) && (name == other.name);
    }

    auto Grouping::operator==(const Grouping& other) const -> bool
    {
        return expression == other.expression;
    }

    auto Logical::operator==(const Logical& other) const -> bool
    {
        return (left == other.left) && (right == other.right) && (oper == other.oper);
    }

    auto Set::operator==(const Set& other) const -> bool
    {
        return (object == other.object) && (name == other.name) && (value == other.value);
    }

    auto Unary::operator==(const Unary& other) const -> bool
    {
        return (right == other.right) && (oper == other.oper);
    }
}  // namespace sail::Expressions