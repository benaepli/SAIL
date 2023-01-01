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

    class ExpressionVisitor
    {
      public:
        ExpressionVisitor() = default;
        virtual ~ExpressionVisitor() = default;

        SAIL_DEFAULT_COPY_MOVE(ExpressionVisitor);

        virtual void visitAssignmentExpression(
            std::shared_ptr<Expressions::Assignment>& expression) = 0;
        virtual void visitBinaryExpression(std::shared_ptr<Expressions::Binary>& expression) = 0;
        virtual void visitCallExpression(std::shared_ptr<Expressions::Call>& expression) = 0;
        virtual void visitGetExpression(std::shared_ptr<Expressions::Get>& expression) = 0;
        virtual void visitGroupingExpression(
            std::shared_ptr<Expressions::Grouping>& expression) = 0;
        virtual void visitLiteralExpression(std::shared_ptr<Expressions::Literal>& expression) = 0;
        virtual void visitLogicalExpression(std::shared_ptr<Expressions::Logical>& expression) = 0;
        virtual void visitSetExpression(std::shared_ptr<Expressions::Set>& expression) = 0;
        virtual void visitSuperExpression(std::shared_ptr<Expressions::Super>& expression) = 0;
        virtual void visitThisExpression(std::shared_ptr<Expressions::This>& expression) = 0;
        virtual void visitUnaryExpression(std::shared_ptr<Expressions::Unary>& expression) = 0;
        virtual void visitVariableExpression(
            std::shared_ptr<Expressions::Variable>& expression) = 0;
    };

    class Expression
    {
      public:
        Expression() = default;
        virtual ~Expression() = default;

        SAIL_DEFAULT_COPY_MOVE(Expression);

        virtual void accept(ExpressionVisitor& visitor) = 0;
    };

}  // namespace sail