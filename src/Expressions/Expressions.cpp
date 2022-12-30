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
    auto hashFunction(const Expressions::This& thisExpr) -> size_t;
    auto hashFunction(const Expressions::Unary& unary) -> size_t;
    auto hashFunction(const Expressions::Variable& variable) -> size_t;

    auto hashFunction(const Expression& expression) -> size_t
    {
        return std::visit([](auto&& arg) { return hashFunction(arg); },
                          expression);
    }

    auto hashFunction(const Expressions::Assignment& assignment) -> size_t
    {
        size_t seed = 0;
        if (assignment.value)
        {
            utils::hashCombine(seed, hashFunction(*assignment.value));
        }
        utils::hashCombine(seed, assignment.name);
        return seed;
    }

    auto hashFunction(const Expressions::Binary& binary) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed,
                           hashFunction(*binary.left),
                           hashFunction(*binary.right),
                           binary.oper);
        return seed;
    }

    auto hashFunction(const Expressions::Call& call) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(*call.callee));
        for (const auto& argument : call.arguments)
        {
            utils::hashCombine(seed, hashFunction(*argument));
        }
        return seed;
    }

    auto hashFunction(const Expressions::Get& get) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(*get.object), get.name);
        return seed;
    }

    auto hashFunction(const Expressions::Grouping& grouping) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, hashFunction(*grouping.expression));
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
        utils::hashCombine(seed,
                           hashFunction(*logical.left),
                           hashFunction(*logical.right),
                           logical.oper);
        return seed;
    }

    auto hashFunction(const Expressions::Set& set) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed,
                           hashFunction(*set.object),
                           set.name,
                           hashFunction(*set.value));
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
        utils::hashCombine(seed, hashFunction(*unary.right), unary.oper);
        return seed;
    }

    auto hashFunction(const Expressions::Variable& variable) -> size_t
    {
        size_t seed = 0;
        utils::hashCombine(seed, variable.name);
        return seed;
    }

}  // namespace

SAIL_HASH_DEFINITION(sail::Expression, hashFunction)