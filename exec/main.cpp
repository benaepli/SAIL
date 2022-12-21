#include <iostream>
#include <string>

#include "EntryPoints/EntryPoints.h"

auto main(const int argc, const char* argv[]) -> int
{

    if (argc > 2)
    {
        std::cout << "Usage: sail [script]" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        sail::EntryPoints::runFile(argv[1]);
    }
    else
    {
        sail::EntryPoints::runPrompt();
    }

    return EXIT_SUCCESS;
}
