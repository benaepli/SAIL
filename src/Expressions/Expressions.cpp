#include <Expressions/Expressions.h>

#include "../utils/Overload.h"

namespace sail
{

    void printExpression(const Expression& expression)
    {
        auto visitor = Overload {
            [](const Expressions::Assignment& assign)
            {
                std::cout << "Assign expression with name "
                          << assign.name.lexeme << std::endl;
                printExpression(*assign.value);
            },
            [](const Expressions::Binary& binary)
            {
                std::cout << "Binary expression with operator "
                          << binary.oper.lexeme << std::endl;
                printExpression(*binary.left);
                printExpression(*binary.right);
            },
            [](const Expressions::Grouping& grouping)
            {
                std::cout << "Grouping expression" << std::endl;
                printExpression(*grouping.expression);
            },
            [](const Expressions::Literal& literal)
            {
                std::cout << "Literal expression with value " << literal.literal
                          << std::endl;
            },
            [](const Expressions::Logical& logical)
            {
                std::cout << "Logical expression with operator "
                          << logical.oper.lexeme << std::endl;
                printExpression(*logical.left);
                printExpression(*logical.right);
            },
            [](const Expressions::Unary& unary)
            {
                std::cout << "Unary expression with operator "
                          << unary.oper.lexeme << std::endl;
                printExpression(*unary.right);
            },
            [](const Expressions::Variable& variable)
            {
                std::cout << "Variable expression with name "
                          << variable.name.lexeme << std::endl;
            },
        };

        std::visit(visitor, expression);
    }
}  // namespace sail