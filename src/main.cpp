#include <print>
#include <thread>
#include <chrono>
#include <vector>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "ast.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "compiler.hpp"

int main(int argc, char* argv[])
{   
    bool emitIR = false;
    std::filesystem::path outputPath;
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "--emit-llvm-ir")
        {
            emitIR = true;
        }
        // else if (std::string(argv[i]) == "-o")
        // {
        //     // I know its dangerous
        //     outputPath = argv[i + 1];
        // }
    }
    if (argc < 2) 
    {
        // std::println("Usage: tantrums <fileName.tnt>");
        return 1;
    }
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    std::vector<std::thread> workers;

    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "--emit-llvm-ir") continue;
        std::thread worker([&argv, i, emitIR]() 
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
            // std::println("AST size: {}", astNode.size());

            std::unique_ptr<CodeGenerator> codegen = std::make_unique<CodeGenerator>(astNode);
            codegen->generate(emitIR);

            // Todo: make using cli args
            Compiler compiler(codegen.get());
            compiler.compile("output.o");
            compiler.link("a");
            // std::println("Finished processing file: {} ~ Worker: {}", argv[i], i);
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