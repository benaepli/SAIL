#include <any>
#include <variant>

#include "Interpreter/Interpreter.h"

#include "../utils/Overload.h"
#include "Errors/RuntimeError.h"
#include "Expressions/Expressions.h"
#include "Token/Token.h"
#include "fmt/format.h"

namespace sail::Intepreter
{
    namespace
    {
        auto literalExpression(Expressions::Literal& expression) -> LiteralType
        {
            return expression.literal;
        }

        auto groupingExpression(Expressions::Grouping& expression)
            -> LiteralType
        {
            return Interpreter::evaluate(*expression.expression);
        }

        auto unaryExpression(Expressions::Unary& expression) -> LiteralType
        {
            LiteralType right = Interpreter::evaluate(*expression.right);
            switch (expression.oper.type)
            {
                case TokenType::eMinus:
                {
                    std::optional<double> number = right.asNumber();
                    if (number.has_value())
                    {
                        return -(*number);
                    }
                    throw RuntimeError(expression.oper,
                                       "Cannot negate a non-number");
                }
                case TokenType::eBang:
                    return !right.isTruthy();
                default:;
            }

            return {};
        }

        auto binaryExpression(Expressions::Binary& expression) -> LiteralType
        {
            LiteralType left = Interpreter::evaluate(*expression.left);
            LiteralType right = Interpreter::evaluate(*expression.right);

            if (expression.oper.type == TokenType::ePlus)
            {
                if (left.isString() && right.isString())
                {
                    return std::get<std::string>(left)
                        + std::get<std::string>(right);
                }
            }

            if (expression.oper.type == TokenType::eBangEqual)
            {
                return left != right;
            }

            if (expression.oper.type == TokenType::eEqualEqual)
            {
                return left == right;
            }

            std::optional<double> leftNumber = left.asNumber();
            std::optional<double> rightNumber = right.asNumber();

            if (!leftNumber.has_value() || !rightNumber.has_value())
            {
                throw RuntimeError(expression.oper,
                                   "Cannot perform arithmetic on non-numbers");
            }

            double leftValue = *leftNumber;
            double rightValue = *rightNumber;

            switch (expression.oper.type)
            {
                case TokenType::eMinus:
                    return leftValue - rightValue;
                case TokenType::eSlash:
                    return leftValue / rightValue;
                case TokenType::eStar:
                    return leftValue * rightValue;
                case TokenType::ePlus:
                    return leftValue + rightValue;
                case TokenType::eGreater:
                    return leftValue > rightValue;
                case TokenType::eGreaterEqual:
                    return leftValue >= rightValue;
                case TokenType::eLess:
                    return leftValue < rightValue;
                case TokenType::eLessEqual:
                    return leftValue <= rightValue;
                default:;
            }

            throw RuntimeError(expression.oper, "Unknown operator");
        }

    }  // namespace

    auto evaluate(Expression& expression) -> LiteralType
    {
        return std::visit(
            Overload {[](Expressions::Literal& expression) -> LiteralType
                      { return literalExpression(expression); },
                      [](Expressions::Grouping& expression) -> LiteralType
                      { return groupingExpression(expression); },
                      [](Expressions::Unary& expression) -> LiteralType
                      { return unaryExpression(expression); },
                      [](Expressions::Binary& expression) -> LiteralType
                      { return binaryExpression(expression); }},
            expression);
    }

    void interpret(Expression& expression)
    {
        LiteralType value = evaluate(expression);
        std::cout << value << std::endl;
    }

}  // namespace sail::Intepreter