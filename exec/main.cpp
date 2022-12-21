#include <iostream>
#include <string>

#include <Interpreter/Interpreter.h>

auto main(const int argc, const char* argv[]) -> int
{

    if (argc > 2)
    {
        std::cout << "Usage: sail [script]" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        sail::Interpreter::runFile(argv[1]);
    }
    else
    {
        sail::Interpreter::runPrompt();
    }

    return EXIT_SUCCESS;
}
