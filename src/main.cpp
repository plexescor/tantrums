#include "lexer.hpp"
#include <print>
#include <chrono>

int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        std::println("Usage: tantrums <fileName.tnt>");
        return 1;
    }
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    Lexer lexer;
    for (int i = 1; i < argc; i++)
    {
        if (!lexer.lexize(argv[i]))
        {
            return 1;
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("Lexing time: {} ms", duration.count());

    return 0;
}