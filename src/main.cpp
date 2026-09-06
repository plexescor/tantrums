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
        std::thread worker([&argv, &start, i, emitIR]() 
        {
            // Sorry for my bad time accumulation
            std::vector<Token> tokens;
            std::vector<ASTNode> astNode;

            Lexer lexer;
            std::println("Lexing file: {} : Progress: {}%", argv[i], 0);
            if (!lexer.lexize(argv[i]))
            {
                return;
            }
            tokens = lexer.getTokens();

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::println("Lexing took: {} ms", duration.count());

            std::println("Parsing file: {} : Progress: {}%", argv[i], 20);
            Parser parser(tokens);
            astNode = parser.parse();
            // std::println("AST size: {}", astNode.size());

            std::chrono::steady_clock::time_point end_ = std::chrono::steady_clock::now();
            auto duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - end);
            std::println("Parsing took: {} ms", duration_.count());

            std::println("Generating IR for: {} : Progress: {}%", argv[i], 40);
            std::unique_ptr<CodeGenerator> codegen = std::make_unique<CodeGenerator>(astNode);
            codegen->generate(emitIR);

            std::chrono::steady_clock::time_point end__ = std::chrono::steady_clock::now();
            auto duration__ = std::chrono::duration_cast<std::chrono::milliseconds>(end__ - end_);
            std::println("IR generation took: {} ms", duration__.count());

            // Todo: make using cli args
            Compiler compiler(codegen.get());
            std::println("Compiling file: {} : Progress: {}%", argv[i], 60);
            compiler.compile("output.o");

            std::chrono::steady_clock::time_point end___ = std::chrono::steady_clock::now();
            auto duration___ = std::chrono::duration_cast<std::chrono::milliseconds>(end___ - end__);
            std::println("Compiling took: {} ms", duration___.count());

            std::println("Linking file: {} : Progress: {}%", argv[i], 80);
            compiler.link("a");
            std::println("Linked executable: {} : Progress: {}%", argv[i], 100);

            std::chrono::steady_clock::time_point end____ = std::chrono::steady_clock::now();
            auto duration____ = std::chrono::duration_cast<std::chrono::milliseconds>(end____ - end___);
            std::println("Linking took: {} ms", duration____.count());
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