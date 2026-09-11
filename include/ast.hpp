#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <memory>

#include "token.hpp"

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

// int32 x = y; its a var, function calls handled
struct IdentifierNode;

// just a literal, used to be the only in legacy
struct LiteralNode;

// two terms(each term can consist of more subterms and so on) with an op
// 2 + (2*2)
struct BinaryExprNode;

// for nullable and -ve conversion and logical NOT
struct UnaryExprNode;

// ASTNode defined early so structs can use it
using ASTNode = std::variant<VariableDeclarationNode, PrintNode, FunctionDeclarationNode>;
using ExprNode = std::variant<LiteralNode, IdentifierNode, BinaryExprNode, UnaryExprNode>;

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

struct IdentifierNode
{
	std::string name;
};

struct BinaryExprNode
{
	TokenType operation;

	// as we dont use classes approach, we need to do this shi-
	std::unique_ptr<ExprNode> leftNode;
	std::unique_ptr<ExprNode> rightNode;
};

struct UnaryExprNode
{
	TokenType operation;
	// Operand refers to a ExprNode on which operation is done on such as
	// in case of -x, op is "-" and operand is "x"
	std::unique_ptr<ExprNode> operand;	
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
	bool isHeap	= false;
	bool isIo	 = false;
	bool isThrows = false;
	bool isPure	= false;
	bool isMut	= false;
	bool isAuto	= false;

	std::vector<ASTNode> body;
};

struct VariableDeclarationNode
{
	bool isMutable;
	TypeNode type;
	bool isAuto;
	std::string name;

	ExprNode value;
};

// For printing/debugging until stdlib is implemnetd
struct PrintNode 
{
	ExprNode value;
	std::string resolvedType;
};
