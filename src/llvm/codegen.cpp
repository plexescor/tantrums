#include <vector>
#include <variant>
#include <string>
#include <print>

#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/TargetSelect.h>

#include "codegen.hpp"
#include "ast.hpp"

// WTf is this even
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };

CodeGenerator::CodeGenerator(std::vector<ASTNode>& nodes)
    : builder(context),
      module(std::make_unique<llvm::Module>("tantrums", context))
{
    this->nodes = nodes;
}

llvm::LLVMContext* CodeGenerator::getLlvmContext()
{
    return &context;
}
llvm::IRBuilder<>* CodeGenerator::getLlvmBuilder()
{
    return &builder;
}

llvm::Module* CodeGenerator::getLlvmModule()
{
    return module.get();
}

llvm::TargetMachine* CodeGenerator::getTargetMachine()
{
    return targetMachine;
}

void CodeGenerator::generate(bool emitIr)
{
    // Boiler plate for llvm
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    llvm::Triple tripleStr(llvm::sys::getDefaultTargetTriple());
    module->setTargetTriple(tripleStr);

    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(tripleStr, error);
    if (!target) 
    {
        return;
    }

    llvm::TargetOptions opt;
    targetMachine = target->createTargetMachine(
        tripleStr, "generic", "", opt, llvm::Reloc::PIC_
    );

    module->setDataLayout(targetMachine->createDataLayout()); 

    // Main function
    // currently no args
    llvm::FunctionType* mainType = llvm::FunctionType::get
    (
        builder.getInt32Ty(),
        {},
        false
    );

    llvm::Function* MainFunc = llvm::Function::Create
    (
        mainType, 
        llvm::Function::ExternalLinkage, 
        "main", 
        module.get()
    );

    llvm::BasicBlock* EntryBB = llvm::BasicBlock::Create(context, "entry", MainFunc);
    builder.SetInsertPoint(EntryBB);

    // Initialize printf as declar for debugging purposes
    // We will add dynamic extern in tanstrums also <insert_peek_emoji>
    llvm::FunctionType *printfType = llvm::FunctionType::get
    (
        builder.getInt32Ty(),              // Return type: i32
        {builder.getPtrTy()},              // First arg: i8* 
        true                               // Is variadic: true
    );

    printfFunc = 
        builder.GetInsertBlock()->getModule()->getOrInsertFunction("printf", printfType);

    for (currentNode = 0; currentNode < nodes.size(); currentNode++)
    {
        // std::println("Node index: {}", nodes[currentNode].index());
        std::visit(Overloaded 
        {
            [this](const PrintNode& print)
            {
                // std::println("We reached print node path!");
                generatePrint(print);
            },
            [](const VariableDeclarationNode& varDecl) {},
            [](const FunctionDeclarationNode fnDecl)
            {

            },
        }, nodes[currentNode]);
    }

    builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));

    if (emitIr) module->print(llvm::outs(), nullptr);
}

void CodeGenerator::generatePrint(const PrintNode& printNode)
{
    std::visit([this](const auto& literalVal)
    {
        using T = std::decay_t<decltype(literalVal)>;

        if constexpr (std::is_same_v<T, std::string>)
        {
            llvm::Value* str = builder.CreateGlobalStringPtr(literalVal);
            builder.CreateCall(printfFunc, {str});
        }
        else if constexpr (std::is_same_v<T, int32_t>)
        {
            llvm::Value* fmt = builder.CreateGlobalStringPtr("%d\n");
            llvm::Value* val = llvm::ConstantInt::get(builder.getInt32Ty(), literalVal);
            builder.CreateCall(printfFunc, {fmt, val});
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            llvm::Value* fmt = builder.CreateGlobalStringPtr("%f\n");
            llvm::Value* val = llvm::ConstantFP::get(builder.getDoubleTy(), literalVal);
            builder.CreateCall(printfFunc, {fmt, val});
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            llvm::Value* fmt = builder.CreateGlobalStringPtr("%f\n");
            llvm::Value* val = llvm::ConstantFP::get(builder.getFloatTy(), literalVal);
            builder.CreateCall(printfFunc, {fmt, val});
        }
    }, printNode.value.value);
}