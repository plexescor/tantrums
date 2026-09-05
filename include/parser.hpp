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
    
    private:
        Token& expect(std::vector<TokenType> expectedTypes);
        Token& expect(TokenType type);
        Token& advance();
        Token& peek();
        Token& current();

        ASTNode parseStatement();
        ASTNode parsePrint();

        std::vector<TokenType> getPossibleTokens_Print();
    private:
        std::vector<ASTNode> ast_Vector;
        std::vector<Token> tokens;
        size_t currentPosition = 0;
};