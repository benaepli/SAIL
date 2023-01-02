#include <iostream>
#include <string>

#include "Instance/Instance.h"


auto main(const int argc, const char* argv[]) -> int
{
    sail::Instance instance{};

    if (argc > 2)
    {
        std::cout << "Usage: sail [script]" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        instance.runFile(argv[1]);
    }
    else
    {
        instance.runPrompt();
    }

    return EXIT_SUCCESS;
}
