#include "Interpreter/Interpreter.h"

#include <fstream>
#include <iostream>

namespace sail
{
    Interpreter::Interpreter() { }

    void Interpreter::runFile(const std::string& path)
    {
        std::ifstream stream(path);
        std::string source;

        if (stream.is_open()) {
            source = std::string((std::istreambuf_iterator<char>(stream)),
                                 std::istreambuf_iterator<char>());
            run(source);
        } else {
            throw std::runtime_error("Could not open file " + path);
        }
    }

    void Interpreter::runPrompt()
    {
        while (true) {
            std::cout << "> ";
            std::string source;
            std::getline(std::cin, source);

            if (source == "exit") {
                break;
            }

            try {
                run(source);
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }

    void Interpreter::run(const std::string& source)
    {
        Scanner scanner {source};
        scanner.scanTokens();

        std::cout << source << std::endl;
    }
}
