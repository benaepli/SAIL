#include <fstream>
#include <iostream>
#include <memory>

#include "Interpreter/Interpreter.h"

#include "Parser/Parser.h"
#include "Scanner/Scanner.h"

namespace sail
{
    namespace
    {
        void run(const std::string& source)
        {
            std::vector<Token> tokens;
            Scanner scanner {source, tokens};
            scanner.scanTokens();

            Parser parser {tokens};
            std::unique_ptr<Expression> expression = parser.parse();

            printExpression(*expression);
        }
    }  // namespace

    void Interpreter::runFile(const std::string& path)
    {
        std::ifstream stream(path);
        std::string source;

        if (stream.is_open())
        {
            source = std::string((std::istreambuf_iterator<char>(stream)),
                                 std::istreambuf_iterator<char>());
            run(source);
        }
        else
        {
            throw std::runtime_error("Could not open file " + path);
        }
    }

    void Interpreter::runPrompt()
    {
        while (true)
        {
            std::cout << "> ";
            std::string source;
            std::getline(std::cin, source);

            if (source == "exit")
            {
                break;
            }

            try
            {
                run(source);
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
}  // namespace sail
