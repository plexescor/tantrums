#include "lexer.hpp"
#include <print>

int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        std::println("Usage: tantrums <fileName.tnt>");
        return 1;
    }
    Lexer lexer;
    for (int i = 1; i < argc; i++)
    {
        if (!lexer.lexize(argv[i]))
        {
            return 1;
        }
    }
    
    return 0;
}