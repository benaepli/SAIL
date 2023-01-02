#pragma once

#include <memory>

#include "Token/Token.h"
#include "utils/classes.h"

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

    class Expression;

    class ExpressionVisitor
    {
      public:
        ExpressionVisitor() = default;
        virtual ~ExpressionVisitor() = default;

        SAIL_DEFAULT_COPY_MOVE(ExpressionVisitor);

        virtual void visitAssignmentExpression(Expressions::Assignment& expression,
                                               std::shared_ptr<Expression>& shared) = 0;
        virtual void visitBinaryExpression(Expressions::Binary& expression,
                                           std::shared_ptr<Expression>& shared) = 0;
        virtual void visitCallExpression(Expressions::Call& expression,
                                         std::shared_ptr<Expression>& shared) = 0;
        virtual void visitGetExpression(Expressions::Get& expression,
                                        std::shared_ptr<Expression>& shared) = 0;
        virtual void visitGroupingExpression(Expressions::Grouping& expression,
                                             std::shared_ptr<Expression>& shared) = 0;
        virtual void visitLiteralExpression(Expressions::Literal& expression,
                                            std::shared_ptr<Expression>& shared) = 0;
        virtual void visitLogicalExpression(Expressions::Logical& expression,
                                            std::shared_ptr<Expression>& shared) = 0;
        virtual void visitSetExpression(Expressions::Set& expression,
                                        std::shared_ptr<Expression>& shared) = 0;
        virtual void visitSuperExpression(Expressions::Super& expression,
                                          std::shared_ptr<Expression>& shared) = 0;
        virtual void visitThisExpression(Expressions::This& expression,
                                         std::shared_ptr<Expression>& shared) = 0;
        virtual void visitUnaryExpression(Expressions::Unary& expression,
                                          std::shared_ptr<Expression>& shared) = 0;
        virtual void visitVariableExpression(Expressions::Variable& expression,
                                             std::shared_ptr<Expression>& shared) = 0;
    };

    class Expression
    {
      public:
        Expression() = default;
        virtual ~Expression() = default;

        SAIL_DEFAULT_COPY_MOVE(Expression);

        virtual void accept(ExpressionVisitor& visitor, std::shared_ptr<Expression>& shared) = 0;
    };

}  // namespace sail