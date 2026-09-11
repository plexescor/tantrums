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

    private:    
		std::string resolveExprType(const ExprNode& node);
        std::string resolveIdentifier(const IdentifierNode& node);

        bool fitsInType(int64_t value, const std::string& type);
        void flushErrorBuffer();
        std::string resolveLiteralType(LiteralNode& node);
    private:
        std::vector<std::string> errorBuffer;
        std::vector<ASTNode>& astNodes;
        std::unordered_set<std::string> validIntTypes;
        std::unordered_set<std::string> validFloatTypes;
};