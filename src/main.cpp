#include <print>
#include <thread>
#include <chrono>
#include <vector>

#include "ast.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2) 
    {
        std::println("Usage: tantrums <fileName.tnt>");
        return 1;
    }
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    std::vector<std::thread> workers;

    for (int i = 1; i < argc; i++)
    {
        std::thread worker([&argv, i]() 
        {
            std::vector<Token> tokens;
            std::vector<ASTNode> astNode;
            Lexer lexer;
            if (!lexer.lexize(argv[i]))
            {
                return;
            }
            tokens = lexer.getTokens();
            Parser parser(tokens);
            astNode = parser.parse();
            std::println("Finished processing file: {} ~ Worker: {}", argv[i], i);
        });
        workers.push_back(std::move(worker));
    }

    for (auto& worker : workers)
    {
        worker.join();
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::println("Compilation time: {} ms", duration.count());


    return 0;
}