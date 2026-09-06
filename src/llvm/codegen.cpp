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
	// llvm::FunctionType* mainType = llvm::FunctionType::get
	// (
	//	 builder.getInt32Ty(),
	//	 {},
	//	 false
	// );

	// llvm::Function* MainFunc = llvm::Function::Create
	// (
	//	 mainType, 
	//	 llvm::Function::ExternalLinkage, 
	//	 "main", 
	//	 module.get()
	// );

	// llvm::BasicBlock* EntryBB = llvm::BasicBlock::Create(context, "entry", MainFunc);
	// builder.SetInsertPoint(EntryBB);

	// Initialize printf as declar for debugging purposes
	// We will add dynamic extern in tanstrums also <insert_peek_emoji>
	// llvm::FunctionType *printfType = llvm::FunctionType::get
	// (
	//	 builder.getInt32Ty(),			  // Return type: i32
	//	 {builder.getPtrTy()},			  // First arg: i8* 
	//	 true							   // Is variadic: true
	// );

	// printfFunc = 
	//	 builder.GetInsertBlock()->getModule()->getOrInsertFunction("printf", printfType);

	for (currentNode = 0; currentNode < nodes.size(); currentNode++)
	{
		// std::println("Node index: {}", nodes[currentNode].index());
		std::visit(Overloaded 
		{
			[this](const FunctionDeclarationNode& fnDecl)
			{
				generateFunction(fnDecl);   
			},
			[this](const auto&) {},
		}, nodes[currentNode]);
	}

	// builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));

	if (emitIr) module->print(llvm::outs(), nullptr);
}

void CodeGenerator::generateFunction(const FunctionDeclarationNode& functionDeclNode)
{
	std::string returnType = functionDeclNode.type.name;
	std::string name = functionDeclNode.name;
	const std::vector<ASTNode>& body = functionDeclNode.body;

	// just capture them for now
	// i dont think we will need this here at all after TypeChecker i implemented
	bool isHeap   = functionDeclNode.isHeap;
	bool isIo	 = functionDeclNode.isIo;
	bool isThrows = functionDeclNode.isThrows;
	bool isPure   = functionDeclNode.isPure;
	bool isMut	= functionDeclNode.isMut;
	bool isAuto   = functionDeclNode.isAuto;

   llvm::Type* result;

	if (returnType == "int8")
		result = builder.getInt8Ty();
	else if (returnType == "int16")
		result = builder.getInt16Ty();
	else if (returnType == "int32")
		result = builder.getInt32Ty();
	else if (returnType == "int64")
		result = builder.getInt64Ty();
	else if (returnType == "uint8")
		result = builder.getInt8Ty();
	else if (returnType == "uint16")
		result = builder.getInt16Ty();
	else if (returnType == "uint32")
		result = builder.getInt32Ty();
	else if (returnType == "uint64")
		result = builder.getInt64Ty();
	else if (returnType == "bool")
		result = builder.getInt1Ty();
	else if (returnType == "void")
		result = builder.getVoidTy();
	else assert (false && "[Debug] Invalid type");

	// No arg and variadic arg support for now
	llvm::FunctionType* functionType = llvm::FunctionType::get
	(
		result,
		{},
		false
	);

	llvm::Function* function = llvm::Function::Create
	(
		functionType, 
		llvm::Function::ExternalLinkage, 
		name, 
		module.get()
	);

	llvm::BasicBlock* EntryBB = llvm::BasicBlock::Create(context, "entry", function);
	builder.SetInsertPoint(EntryBB);

	// init printf for this function's use
	llvm::FunctionType* printfType = llvm::FunctionType::get(
		builder.getInt32Ty(), {builder.getPtrTy()}, true
	);
	printfFunc = module->getOrInsertFunction("printf", printfType);

	// emit body
	for (const ASTNode& node : body)
	{
		std::visit(Overloaded
		{
			[this](const PrintNode& print)
			{ 
				generatePrint(print); 
			},
			[](const VariableDeclarationNode&)
			{},

			// }, //Functinos inside functinos! Subject unexplained removal
			// currently disabled
			[this](const FunctionDeclarationNode& fn) 
			{ 
				// generateFunction(fn); 
			}, 
		}, node);
	}

	// return
	if (returnType == "void")
		builder.CreateRetVoid();
	else
		builder.CreateRet(llvm::ConstantInt::get(result, 0));
}

void CodeGenerator::generatePrint(const PrintNode& printNode)
{
	std::visit([this](const auto& literalVal)
	{
		using T = std::decay_t<decltype(literalVal)>;

		if constexpr (std::is_same_v<T, std::string>)
		{
			llvm::Value* str = builder.CreateGlobalString(literalVal);
			builder.CreateCall(printfFunc, {str});
		}
		else if constexpr (std::is_same_v<T, int8_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%hhd\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt8Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, int16_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%hd\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt16Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, int32_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%d\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt32Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, int64_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%lld\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt64Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, uint8_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%hhu\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt8Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, uint16_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%hu\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt16Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, uint32_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%u\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt32Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, uint64_t>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%llu\n");
			llvm::Value* val = llvm::ConstantInt::get(builder.getInt64Ty(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%f\n");
			llvm::Value* val = llvm::ConstantFP::get(builder.getDoubleTy(), literalVal);
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			llvm::Value* fmt = builder.CreateGlobalString("%f\n");
			llvm::Value* val = llvm::ConstantFP::get(builder.getDoubleTy(), static_cast<double>(literalVal));
			builder.CreateCall(printfFunc, {fmt, val});
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			llvm::Value* str = builder.CreateGlobalString(literalVal ? "true\n" : "false\n");
			builder.CreateCall(printfFunc, {str});
		}
	}, printNode.value.value);
}