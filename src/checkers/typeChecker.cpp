#include <vector>
#include <map>
#include <print>
#include <unordered_set>
#include <variant>

#include "typeChecker.hpp"
#include "symbolTable.hpp"
#include "ast.hpp"

template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };

static SymbolTable symbols;

bool TypeChecker::fitsInType(int64_t value, const std::string& type)
{
    if (type == "int8")   return value >= INT8_MIN  && value <= INT8_MAX;
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
                // checkVariableDeclaration(print);
            },
            [this](auto&)
            {
                errorBuffer.push_back("Only function declarations are allowed in top level code");
            },
        }, node);
    }

    symbols.popScope();
}

void TypeChecker::checkVariableDeclaration(VariableDeclarationNode& varDecl)
{
    if (symbols.existsInCurrentScope(varDecl.name))
    {
        errorBuffer.push_back(std::format("Variable '{}' is already defined in the current scope!", varDecl.name));
        return;
    }

    // Resolve the type of RHS, 
    std::string resolvedType = resolveLiteralType(varDecl.value);
    std::string declaredType = varDecl.type.name;

    //Check if its decl type is auto and if yes patch it
    if (declaredType == "auto")
    {
        if (resolvedType == "untypedInt")   varDecl.type.name = "int32";  // default
        if (resolvedType == "untypedFloat") varDecl.type.name = "float64"; // default
        if (resolvedType == "string")        varDecl.type.name = "string";
        if (resolvedType == "bool")          varDecl.type.name = "bool";
        symbols.declare(varDecl.name, varDecl.type.name, varDecl.isMutable);
        return;
    }

    // fit-check for UntypedInt
    if (resolvedType == "untypedInt")
    {
        int64_t raw = std::get<UntypedInt>(varDecl.value.value).value;
        if (varDecl.value.isNegative) raw *= -1;
        std::println("[Debug] Value of {} is {}", varDecl.name, raw);
        if (!fitsInType(raw, declaredType))
        {
            errorBuffer.push_back(std::format(
                "Value '{}' does not fit in type '{}'", raw, declaredType
            ));
            return;
        }
        symbols.declare(varDecl.name, declaredType, varDecl.isMutable);
        return;
    }

    if (resolvedType == "untypedFloat")
    {
        static const std::unordered_set<std::string> floatTypes = { "float32", "float64" };
        // if (varDecl.value.isNegative) *= -1;
        // std::println("[Debug] Value of {} is {}", varDecl.name, raw);
        if (!floatTypes.contains(declaredType))
        {
            errorBuffer.push_back(std::format(
                "Cannot assign float literal to '{}'", declaredType
            ));
            return;
        }
        symbols.declare(varDecl.name, declaredType, varDecl.isMutable);
        return;
    }

    if (varDecl.type.name != resolvedType)
    {
        errorBuffer.push_back(std::format(
            "Type mismatch: identifier '{}' declared as '{}' but resolved type is '{}'",
            varDecl.name, varDecl.type.name, resolvedType
        ));
        return;
    }

    std::println(
        "[Debug]: Identifier: '{}', Declared Type: '{}', Resolved Type: '{}'",
        varDecl.name, varDecl.type.name, resolvedType
    );
    
    symbols.declare(varDecl.name, varDecl.type.name, varDecl.isMutable);
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
        [](UntypedInt&)   { return "untypedInt"; },
        [](UntypedFloat&) { return "untypedFloat"; },
        [](std::string&)  { return "string"; },
        [](bool)          { return "bool"; },
    }, node.value);
}
