#pragma once
#include <vector>
#include <cstdint>
#include <optional>

#include "token.hpp"
#include "ast.hpp"

class Parser
{
    public:
        Parser(const std::vector<Token>& tokens);
        ~Parser() = default;
        std::vector<ASTNode> parse();
        bool hasError() const { return hadError; }
    
    private:
        bool isDeclKeyword(TokenType type);
        bool isAnnotationKeyword(TokenType type);
        Token& expect(std::vector<TokenType> expectedTypes);
        Token& expect(TokenType type);
        Token& advance();
        Token& peek();
        Token& current();
        void synchronize();

        std::optional<ASTNode> parseStatement();
        std::optional<ASTNode> parsePrint();
        std::optional<ASTNode> parseVariableDeclaration();
        std::optional<ASTNode> parseFunctionDeclaration();

        std::vector<TokenType> getPossibleTokens_Print();
        std::vector<TokenType> getPossibleTokens_Decl();
    private:
        std::vector<ASTNode> ast_Vector;
        std::vector<Token> tokens;
        size_t currentPosition = 0;
        bool hadError = false;
};