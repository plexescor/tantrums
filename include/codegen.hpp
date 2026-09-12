#pragma once

#include <vector>
#include <cstdint>
#include <map>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include "ast.hpp"
#include "typeChecker.hpp"

class CodeGenerator
{
	public:
		CodeGenerator(std::vector<ASTNode>& nodes);
		void generate(bool emitIr);
		llvm::LLVMContext* getLlvmContext();
		llvm::IRBuilder<>* getLlvmBuilder();
		llvm::Module* getLlvmModule();
		llvm::TargetMachine* getTargetMachine();

	private:
		llvm::Type* getLlvmType(std::string& returnType);
		llvm::Value* generateExpr(const ExprNode& exprNode, const std::string& resolvedType);
		void generatePrint(const PrintNode& printNode);
		void generateFunction(const FunctionDeclarationNode& functionDeclNode);
		void generateFunctionCall(const FunctionCallNode& fnCall);
		void generateVariable(const VariableDeclarationNode& varDeclNode, llvm::Function* function);

	public:
		void setTypeChecker(TypeChecker* checker);
		
	private:
		std::map<std::string, llvm::AllocaInst*> namedValues_Variables;
		std::map<std::string, std::pair<llvm::Function*, llvm::FunctionType*>> namedValues_Functions;

		TypeChecker* typeChecker = nullptr;	

		llvm::FunctionCallee printfFunc;
		llvm::LLVMContext context;
		llvm::IRBuilder<> builder;
		llvm::TargetMachine* targetMachine;
		std::unique_ptr<llvm::Module> module;

		std::vector<ASTNode> nodes;
		size_t currentNode = 0;
};