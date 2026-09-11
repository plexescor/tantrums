#pragma once
#include <vector>
#include <map>
#include <string>
#include <unordered_set>

#include "symbolTable.hpp"
#include "ast.hpp"

class TypeChecker
{
	 public: 
		  TypeChecker(std::vector<ASTNode>& astNodes);
		  ~TypeChecker() = default;

		  bool check();
		  void checkFunctionDeclaration(FunctionDeclarationNode& fnDecl);
		  void checkVariableDeclaration(VariableDeclarationNode& varDecl);
		  void checkPrint(PrintNode& printNode);

	 public:	 
		  std::string resolveExprType(const ExprNode& node);
		  std::string resolveIdentifier(const IdentifierNode& node);
		  std::unordered_set<std::string> validIntTypes;
		  std::unordered_set<std::string> validFloatTypes;
	 private:
		  bool fitsInType(int64_t value, const std::string& type);
		  void flushErrorBuffer();
		  std::string resolveLiteralType(LiteralNode& node);
	 private:
		  std::vector<std::string> errorBuffer;
		  std::vector<ASTNode>& astNodes;
		  
};