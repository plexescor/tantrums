#include <vector>
#include <map>
#include <print>
#include <variant>

#include "typeChecker.hpp"
#include "symbolTable.hpp"
#include "ast.hpp"

template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };

static SymbolTable symbols;

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
    return false;
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

    //Check if its decl type is auto and if yes patch it
    if (varDecl.type.name == "auto")
    {
        varDecl.type.name = resolvedType;
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
    return std::visit(Overloaded
    {
        [](int8_t)    { return "int8"; },
        [](int16_t)   { return "int16"; },
        [](int32_t)   { return "int32"; },
        [](int64_t)   { return "int64"; },

        [](uint8_t)   { return "uint8"; },
        [](uint16_t)  { return "uint16"; },
        [](uint32_t)  { return "uint32"; },
        [](uint64_t)  { return "uint64"; },

        [](float)     { return "float"; },
        [](double)    { return "double"; },

        [](const std::string&) { return "string"; },
        [](bool)      { return "bool"; }

    }, node.value);
}
