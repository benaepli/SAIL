#pragma once

#include <memory>
#include <string>

namespace sail
{
    class Interpreter;

    class Instance
    {
      public:
        Instance();
        ~Instance();

        void runFile(const std::string& path);
        void runPrompt();

      private:
        void run(const std::string& source);

        Interpreter* _interpreter;
    };
}  // namespace sail