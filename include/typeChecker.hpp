#pragma once
#include <vector>
#include <map>

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

    private:    
        bool fitsInType(int64_t value, const std::string& type);
        void flushErrorBuffer();
        std::string resolveLiteralType(LiteralNode& node);
    private:
        std::vector<std::string> errorBuffer;
        std::vector<ASTNode>& astNodes;
};