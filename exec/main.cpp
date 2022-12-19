#include <iostream>
#include <string>

#include <Interpreter/Interpreter.h>

int main(const int argc, const char* argv[])
{
    sail::Interpreter interpreter{};

    if (argc > 2) {
        std::cout << "Usage: sail [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        interpreter.runFile(argv[1]);
    } else {
        interpreter.runPrompt();
    }

    return 0;
}
