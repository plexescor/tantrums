#include <vector>
#include <variant>
#include <string>
#include <print>

#pragma warning(push, 0)
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/TargetSelect.h>
#pragma warning(pop)

#include "typeChecker.hpp"
#include "codegen.hpp"
#include "ast.hpp"

// WTf is this even
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };

CodeGenerator::CodeGenerator(std::vector<ASTNode>& nodes)
	: builder(context),
	 module(std::make_unique<llvm::Module>("tantrums", context))
{
	this->nodes = std::move(nodes);
}

void CodeGenerator::setTypeChecker(TypeChecker* checker)
{
	typeChecker = checker;
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

llvm::Type* CodeGenerator::getLlvmType(std::string& returnType)
{
	if (returnType == "int8") return builder.getInt8Ty();
	else if (returnType == "int16") return builder.getInt16Ty();
	else if (returnType == "int32") return builder.getInt32Ty();
	else if (returnType == "int64") return builder.getInt64Ty();
	else if (returnType == "uint8") return builder.getInt8Ty();
	else if (returnType == "uint16") return builder.getInt16Ty();
	else if (returnType == "uint32") return builder.getInt32Ty();
	else if (returnType == "uint64") return builder.getInt64Ty();
	else if (returnType == "float32") return builder.getFloatTy();
	else if (returnType == "float64") return builder.getDoubleTy();
	else if (returnType == "string")  return builder.getPtrTy();
	else if (returnType == "bool") return builder.getInt1Ty();
	else if (returnType == "void") return builder.getVoidTy();

	// treate untyped int as int32 by default, untypedFloat as float32
	else if (returnType == "untypedInt") return builder.getInt32Ty();
	else if (returnType == "untypedFloat") return builder.getFloatTy();

	std::println(stderr, "[Codegen] Unknown type: '{}'", returnType);
	assert(false && "[Codegen] Unknown type");
	return nullptr;
}

llvm::Value* CodeGenerator::generateExpr(const ExprNode& exprNode, const std::string& resolvedType)
{
	return std::visit(Overloaded
	{
		[this, resolvedType](const LiteralNode& litNode) -> llvm::Value*
		{
			return std::visit(Overloaded
			{
				[this, resolvedType](const UntypedInt& untypedInt) -> llvm::Value*
				{
					std::string resolved = resolvedType;
					return llvm::ConstantInt::get(getLlvmType(resolved), untypedInt.value);
				},

				[this, resolvedType](const UntypedFloat& untypedFloat) -> llvm::Value*
				{
					std::string resolved = resolvedType;
					return llvm::ConstantFP::get(getLlvmType(resolved), untypedFloat.value);
				},

				[this](const std::string& string) -> llvm::Value*
				{
					return builder.CreateGlobalString(string);
				},

				[this, resolvedType](const bool& boolean) -> llvm::Value*
				{
					std::string resolved = resolvedType;
					return llvm::ConstantInt::get(getLlvmType(resolved), boolean ? 1 : 0);
				}
			}, litNode.value);
		},

		[this, resolvedType](const IdentifierNode& identifierNode) -> llvm::Value*
		{
			std::string resolved = resolvedType;
			auto it = namedValues_Variables.find(identifierNode.name);
			if (it == namedValues_Variables.end() || it->second == nullptr)
			{
				std::println(stderr, "[Codegen error] Variable '{}' not found or not allocated!", identifierNode.name);
				return nullptr;
			}
			return builder.CreateLoad(getLlvmType(resolved), it->second);
		},

		[this, resolvedType](const BinaryExprNode& binaryNode) -> llvm::Value*
		{
			std::string resolved = resolvedType;

			llvm::Value* left = generateExpr(*binaryNode.leftNode, resolved);
			llvm::Value* right = generateExpr(*binaryNode.rightNode, resolved);

			TokenType opToken = binaryNode.operation;

			bool isFloat = false;
			if (resolvedType == "float32" || resolvedType == "float64" || resolvedType == "untypedFloat")
				isFloat = true;
				
			switch (opToken)
			{
				case TokenType::TOKEN_PLUS_OPERATOR:
					return isFloat ? builder.CreateFAdd(left, right)
									: builder.CreateAdd(left, right);
									break;

				case TokenType::TOKEN_MINUS_OPERATOR:
					return isFloat ? builder.CreateFSub(left, right)
									: builder.CreateSub(left, right);
									break;

				case TokenType::TOKEN_STAR_OPERATOR:
					return isFloat ? builder.CreateFMul(left, right)
									: builder.CreateMul(left, right);
									break;
				
				case TokenType::TOKEN_DIVISION_OPERATOR:
					return isFloat ? builder.CreateFDiv(left, right)
									: builder.CreateSDiv(left, right);
									break;
				
				default:
					return nullptr;
			}
		},

		[this, resolvedType](const UnaryExprNode& unaryNode) -> llvm::Value*
		{
			std::string resolved = resolvedType;
			llvm::Value* operand = generateExpr(*unaryNode.operand, resolved);

			if (resolvedType == "float32" || resolvedType == "float64")
			{
				return builder.CreateFNeg(operand);
			}
			else
			{
				return builder.CreateNeg(operand);
			}
		}
	}, exprNode);
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
	//	builder.getInt32Ty(),
	//	{},
	//	false
	// );

	// llvm::Function* MainFunc = llvm::Function::Create
	// (
	//	mainType, 
	//	llvm::Function::ExternalLinkage, 
	//	"main", 
	//	module.get()
	// );

	// llvm::BasicBlock* EntryBB = llvm::BasicBlock::Create(context, "entry", MainFunc);
	// builder.SetInsertPoint(EntryBB);

	// Initialize printf as declar for debugging purposes
	// We will add dynamic extern in tanstrums also <insert_peek_emoji>
	// llvm::FunctionType *printfType = llvm::FunctionType::get
	// (
	//	builder.getInt32Ty(),			 // Return type: i32
	//	{builder.getPtrTy()},			 // First arg: i8* 
	//	true								// Is variadic: true
	// );

	// printfFunc = 
	//	builder.GetInsertBlock()->getModule()->getOrInsertFunction("printf", printfType);

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
	bool isHeap	= functionDeclNode.isHeap;
	bool isIo	= functionDeclNode.isIo;
	bool isThrows = functionDeclNode.isThrows;
	bool isPure	= functionDeclNode.isPure;
	bool isMut	= functionDeclNode.isMut;
	bool isAuto	= functionDeclNode.isAuto;

	llvm::Type* result = getLlvmType(returnType);
//	std::println(stderr, "[Debug] returnType='{}' -> result={}", returnType, (void*)result);

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
			// Single threaded so fine, though my reasoning can be wrong
			[this, &function](const VariableDeclarationNode& varDecl)
			{
				generateVariable(varDecl, function);
			},

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

void CodeGenerator::generateVariable(const VariableDeclarationNode& varDeclNode
									, llvm::Function* function)
{
	std::string type_Str = varDeclNode.type.name;
	std::string name = varDeclNode.name;
	llvm::Type* type = getLlvmType(type_Str);

	llvm::IRBuilder<> tempBuilder(&function->getEntryBlock(), function->getEntryBlock().begin());
	llvm::AllocaInst* alloca = tempBuilder.CreateAlloca(type, nullptr, name);

	llvm::Value* initialValue = generateExpr(varDeclNode.value, type_Str);

	if (!initialValue)
	{
		std::println("Error");
		return;
	}
	builder.CreateStore(initialValue, alloca);
	namedValues_Variables[name] = alloca;
	
}
void CodeGenerator::generatePrint(const PrintNode& printNode)
{
	if (!typeChecker) 
	{
		std::println(stderr, "[CodeGen error] Type Checker pointer is null!");
		return;
	}
	std::string resolvedType = printNode.resolvedType;
	llvm::Value* value = generateExpr(printNode.value, resolvedType);
	if (!value || !printfFunc)
		return;

	llvm::Value* format;
	llvm::Value* doubleValue = nullptr;

	// Treate booleans similar as num literals
	if (typeChecker->validIntTypes.contains(resolvedType) || resolvedType == "bool")
	{
		format = builder.CreateGlobalString("%d\n", "print.format");
	}
	else if (typeChecker->validFloatTypes.contains(resolvedType))
	{
		if (value->getType()->isFloatTy()) 
		{
			doubleValue = builder.CreateFPExt(value, builder.getDoubleTy(), "promotedDouble");
		}
		format = builder.CreateGlobalString("%f\n", "print.format");
	}
	else if (resolvedType == "string")
	{
		format = builder.CreateGlobalString("%s\n", "print.format");
	}
	
	if (doubleValue)
		builder.CreateCall(printfFunc, { format, doubleValue });
	else
		builder.CreateCall(printfFunc, { format, value} );
}