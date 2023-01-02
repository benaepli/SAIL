#include <fstream>
#include <iostream>

#include "Instance/Instance.h"

#include "Interpreter/Interpreter.h"
#include "Parser/Parser.h"
#include "Resolver/Resolver.h"
#include "Scanner/Scanner.h"
#include "mimalloc-new-delete.h"

namespace sail
{
    Instance::Instance()
        : _interpreter(new Interpreter())
    {
    }

    Instance::~Instance()
    {
        delete _interpreter;
    }

    void Instance::runFile(const std::string& path)
    {
        std::ifstream stream(path);
        try
        {
            if (stream.is_open())
            {
                std::string source;
                source = std::string((std::istreambuf_iterator<char>(stream)),
                                     std::istreambuf_iterator<char>());
                run(source);
            }
            else
            {
                std::cout << "Could not open file " << path << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void Instance::runPrompt()
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

            // try
            // {
            run(source);
            // }
            // catch (const std::exception& e)
            // {
            //     std::cout << e.what() << std::endl;
            // }
        }
    }

    void Instance::run(const std::string& source)
    {
        std::vector<Token> tokens;
        Scanner scanner {source, tokens};
        scanner.scanTokens();

        Parser parser {tokens};
        std::vector<std::shared_ptr<Statement>> statements = parser.parse();

        Resolver resolver {*_interpreter};
        resolver.resolve(statements);

        _interpreter->interpret(statements);

        // end here
    }
}  // namespace sail