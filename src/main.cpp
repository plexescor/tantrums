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
        std::println("Argument {}: {}", i, argv[i]);
    }
    // std::println("File name: {}", argv[1]);
    return 0;
}