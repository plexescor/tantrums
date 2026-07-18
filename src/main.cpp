#include "lexer.hpp"
#include <print>

int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        std::println("Usage: tantrums <fileName.42AHH>");
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        Lexer lexer;
        lexer.lexize(argv[i]);
    }
    
    return 0;
}