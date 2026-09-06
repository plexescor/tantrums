#pragma once

#include <string>
#include <variant>
#include <cstdint>

// forward declarations
struct VariableDeclarationNode;
struct PrintNode;
struct FunctionDeclarationNode;

// ASTNode defined early so structs can use it
using ASTNode = std::variant<VariableDeclarationNode, PrintNode, FunctionDeclarationNode>;


struct LiteralNode
{
	std::variant<
		int8_t,
		int16_t,
		int32_t,
		int64_t,
		uint8_t,
		uint16_t,
		uint32_t,
		uint64_t,
		float,
		double,
		std::string,
		bool
	> value;
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
