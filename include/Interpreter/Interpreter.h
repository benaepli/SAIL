#pragma once

#include <string>

#include "Scanner/Scanner.h"


namespace sail
{
    class Interpreter
    {
    public:
        Interpreter();

        void runFile(const std::string& path);
        void runPrompt();

    private:
        void run(const std::string& source);
    };
}
