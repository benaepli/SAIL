#include <algorithm>
#include <any>
#include <variant>

#include "Interpreter/Interpreter.h"

#include "../utils/Overload.h"
#include "Errors/RuntimeError.h"
#include "Expressions/Expressions.h"
#include "Interpreter/ExpressionInterpreter.h"
#include "Statements/Statements.h"
#include "Token/Token.h"
#include "fmt/format.h"

namespace sail::Interpreter
{
    namespace
    {
        void visitExpressionStatement(Statements::Expression& statement)
        {
            evaluate(*statement.expression);
        }

        void visitVariableStatement(Statements::Variable& statement) {}

    }  // namespace
    void execute(Statement& statement)
    {
        std::visit(
            Overload {
                [](Statements::Expression& statement) -> void
                { visitExpressionStatement(statement); },
                [](Statements::Variable& statement) -> void
                { visitVariableStatement(statement); },
            },
            statement);
    }

    void interpret(std::vector<Statement>& statements)
    {
        auto each = [&](auto& statement) -> void { execute(statement); };

        std::ranges::for_each(statements, each);
    }

}  // namespace sail::Interpreter
