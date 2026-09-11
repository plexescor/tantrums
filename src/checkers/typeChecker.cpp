#include <vector>
#include <map>
#include <print>
#include <unordered_set>
#include <variant>
#include <string>

#include "typeChecker.hpp"
#include "symbolTable.hpp"
#include "ast.hpp"

template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };

static SymbolTable symbols;

bool TypeChecker::fitsInType(int64_t value, const std::string& type)
{
	 if (type == "int8")	return value >= INT8_MIN  && value <= INT8_MAX;
	 if (type == "int16")  return value >= INT16_MIN && value <= INT16_MAX;
	 if (type == "int32")  return value >= INT32_MIN && value <= INT32_MAX;
	 if (type == "int64")  return true; // int64_t can hold anything being parsed
	 if (type == "uint8")  return value >= 0 && value <= UINT8_MAX;
	 if (type == "uint16") return value >= 0 && value <= UINT16_MAX;
	 if (type == "uint32") return value >= 0 && value <= UINT32_MAX;
	 if (type == "uint64") return value >= 0; // can't represent > INT64_MAX via stoll anyway
	 return false; // int literal into other type is denied
}

TypeChecker::TypeChecker(std::vector<ASTNode>& astNodes)
	 : astNodes(astNodes)
{
	 // Initialize valid int and float types set
	 validIntTypes = 
	 { 
		  "int8",
		  "int16",
		  "int32",
		  "int64",
		  "uint8",
		  "uint16",
		  "uint32",
		  "uint64",
		  "untypedInt"
	 };

	 validFloatTypes = 
	 {
		  "float32",
		  "float64",
		  "untypedFloat"
	 };
}

bool TypeChecker::check()
{
	 for (ASTNode& node : astNodes)
	 {
		  std::visit(Overloaded
		  {
				[this](FunctionDeclarationNode& fnDecl)
				{
					 checkFunctionDeclaration(fnDecl);
				},
				[this](auto&)
				{
					 errorBuffer.push_back("Only function declarations are allowed in top level code");
				},
		  }, node);
	 }
	 flushErrorBuffer();
	 return errorBuffer.empty();
}

std::string TypeChecker::resolveExprType(const ExprNode& node)
{
	 return std::visit(Overloaded
	 {
		  [](const LiteralNode& literal) -> std::string
		  {
				return std::visit(Overloaded
				{
					 [](const UntypedInt&)	{ return std::string("untypedInt"); },
					 [](const UntypedFloat&) { return std::string("untypedFloat"); },
					 [](const std::string&)  { return std::string("string"); },
					 [](bool)					 { return std::string("bool"); }
				}, literal.value);
		  },

		  [this](const IdentifierNode& identifierNode) -> std::string
		  {
				return resolveIdentifier(identifierNode);
		  },

		  [this](const BinaryExprNode& binaryNode) -> std::string
		  {
				// Resolve the left and right side nodes
				std::string left = resolveExprType(*binaryNode.leftNode);
				std::string right = resolveExprType(*binaryNode.rightNode);

				// Check if any error occured, return immed if yes
				if (left == "error" || right == "error") return "error";

				if (validIntTypes.contains(left) && validIntTypes.contains(right))
				{
					 // Return the type which is more concrete
					 if (left != "untypedInt") return left;
					 else if (right != "untypedInt") return right;

					 // return this if both are not concrete
					 return "untypedInt";
				}
				
				if (validFloatTypes.contains(left) && validFloatTypes.contains(right))
				{
					 // Return the type which is more concrete
					 if (left != "untypedFloat") return left;
					 else if (right != "untypedFloat") return right;

					 // return this if both are not concrete
					 return "untypedFloat";
				}

				errorBuffer.push_back(
					 std::format("Type mismatch in binary expression, "
					 "left side deduced as '{}', right side deduced as '{}'",
					 left, right
				));
				return "error";
		  },

		  [this](const UnaryExprNode& unaryNode) -> std::string
		  {
				std::string operandType = resolveExprType(*unaryNode.operand);
				if (operandType == "error") return "error";

				if (validIntTypes.contains(operandType) || validFloatTypes.contains(operandType))
				{
					 return operandType;
				}
				else // if (operandType == "string" || operandType == "bool")
				{
					 errorBuffer.push_back(std::format("Cannot perform a unary operation on a {}", operandType));
					 return "error";
				}
		  }
	 }, node);
}

std::string TypeChecker::resolveIdentifier(const IdentifierNode& node)
{
	 std::string name = node.name;

	 std::optional<std::pair<std::string, bool>> type = symbols.lookup(name);
	 std::string type_S = "";
	 if (!type.has_value())
	 {
		  errorBuffer.push_back(std::format("Identifier {} does not exist in the current scope!", name));
		  return "error";
		  type_S = type.value().first;
	 }
	 std::println("Symbol: {}", type.value().first);
	 type_S = type.value().first;
	 return type_S;
}

void TypeChecker::checkFunctionDeclaration(FunctionDeclarationNode& fnDecl)
{
	 symbols.pushScope();

	 for (ASTNode& node : fnDecl.body)
	 {
		  std::visit(Overloaded
		  {
				[this](FunctionDeclarationNode& fnDecl)
				{
					 // checkFunctionDeclaration(fnDecl);
				},
				[this](VariableDeclarationNode& varDecl)
				{
					 checkVariableDeclaration(varDecl);
				},
				[this](PrintNode& print)
				{
					 checkPrint(print);
				},
				[this](auto&)
				{
					 errorBuffer.push_back("Only function declarations are allowed in top level code");
				},
		  }, node);
	 }

	 symbols.popScope();
}	
void TypeChecker::checkPrint(PrintNode& printNode)
{
	 /*
		  if the final type of an expression comes out to be a string
		  (after conversion) and the original one was validIntTypes or validFloatTypes
		  or bool, then its convertable
	 */
	std::string resolvedType = resolveExprType(printNode.value);
	if (resolvedType == "error")
	{
		std::println(stderr, "Cannot resolve the final type of the expression!");
		return;
	}
	printNode.resolvedType = resolvedType;

	// If its not an int or a bool or a string
	// then its unprintable
	if (!validIntTypes.contains(resolvedType) 
		&& !validFloatTypes.contains(resolvedType)
		&& resolvedType != "bool"
		&& resolvedType != "string")
	{
		std::println(stderr, "The resolved type '{}'of expression is not printable!", resolvedType);
		return;
	}
}

void TypeChecker::checkVariableDeclaration(VariableDeclarationNode& varDecl)
{
	 if (symbols.existsInCurrentScope(varDecl.name))
	 {
		  errorBuffer.push_back(std::format("Variable '{}' is already defined in the current scope!", varDecl.name));
		  return;
	 }

	 // Resolve the type of RHS, 
	//  std::println("[Debug] Entering checkVarDecl for '{}'", varDecl.name);
    
    std::string resolvedType = resolveExprType(varDecl.value);
    // std::println("[Debug] '{}' resolved to '{}'", varDecl.name, resolvedType);
    
    std::string declaredType = varDecl.type.name;

	 //Check if its decl type is auto and if yes patch it
	 //this doesnt work now ever since the expression parsing thing
	 if (declaredType == "auto")
	 {
		  if (resolvedType == "untypedInt")	varDecl.type.name = "int32";  // default
		  if (resolvedType == "untypedFloat") varDecl.type.name = "float64"; // default
		  if (resolvedType == "string")		  varDecl.type.name = "string";
		  if (resolvedType == "bool")			 varDecl.type.name = "bool";
		  symbols.declare(varDecl.name, varDecl.type.name, varDecl.isMutable);
		  return;
	 }

	 // just call it quits if something *deeper* failed
	 if (resolvedType == "error") return;

	 if (resolvedType == "untypedInt")
	 {
		  if (!validIntTypes.contains(declaredType))
		  {
				errorBuffer.push_back(
					 std::format("Type mismatch between declared '{}' and resolved '{}' variable types",
						  declaredType, resolvedType
				));
				return;
		  }

		  symbols.declare(varDecl.name, varDecl.type.name, varDecl.isMutable);
	 }

	 else if (resolvedType == "untypedFloat")
	 {
		  if (!validFloatTypes.contains(declaredType))
		  {
				errorBuffer.push_back(
					 std::format("Type mismatch between declared '{}' and resolved '{}' variable types",
						  declaredType, resolvedType
				));
				return;
		  }

		  symbols.declare(varDecl.name, varDecl.type.name, varDecl.isMutable);
	 }

	 // covers string and bools
	 else if (declaredType == resolvedType)
	 {
		  symbols.declare(varDecl.name, varDecl.type.name, varDecl.isMutable);
	 }

	 else
	 {
		  errorBuffer.push_back(
				std::format(
					 "Type mismatch between declared '{}' and resolved '{}' variable types",
					 declaredType,
					 resolvedType
				)
		  );
		  return;
	 }
}

void TypeChecker::flushErrorBuffer()
{
	 for (const auto& error : errorBuffer)
	 {
		  std::println("[TypeChecker Error]: {}", error);
	 }
}

std::string TypeChecker::resolveLiteralType(LiteralNode &node)
{
	 return std::visit(Overloaded {
		  [](UntypedInt&)	{ return "untypedInt"; },
		  [](UntypedFloat&) { return "untypedFloat"; },
		  [](std::string&)  { return "string"; },
		  [](bool)			 { return "bool"; },
	 }, node.value);
}
