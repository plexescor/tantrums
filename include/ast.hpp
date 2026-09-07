#pragma once

#include <string>
#include <variant>
#include <cstdint>

struct UntypedInt  
{ 
	int64_t value; 
};  

struct UntypedFloat 
{ 
	double value; 
};

// forward declarations
struct VariableDeclarationNode;
struct PrintNode;
struct FunctionDeclarationNode;

// ASTNode defined early so structs can use it
using ASTNode = std::variant<VariableDeclarationNode, PrintNode, FunctionDeclarationNode>;


struct LiteralNode
{
	std::variant<
		UntypedInt,
		UntypedFloat,
		std::string,
		bool
	> value;
	bool isNegative = false;
};

struct TypeNode 
{
	std::string name;
	bool isNullable = false; // the ? op
};

struct FunctionDeclarationNode
{
	TypeNode type;
	std::string name;

	bool returnsNull = false;
	bool isHeap   = false;
	bool isIo	 = false;
	bool isThrows = false;
	bool isPure   = false;
	bool isMut	= false;
	bool isAuto   = false;

	std::vector<ASTNode> body;
};

struct VariableDeclarationNode
{
	bool isMutable;
	TypeNode type;
	bool isAuto;
	std::string name;

	LiteralNode value;
};

// For printing/debugging until stdlib is implemnetd
struct PrintNode 
{
	LiteralNode value;
};
