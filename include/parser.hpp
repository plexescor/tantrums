#pragma once
#include <vector>
#include <cstdint>
#include <optional>

#include "token.hpp"

class Parser
{
    public:
        Parser(const std::vector<Token>& tokens);
        ~Parser() = default;
        void parse();
    
    private:
        Token& expect(std::vector<TokenType> expectedTypes);
        Token& expect(TokenType type);
        Token& advance();
        Token& peek();
        Token& current();

        std::vector<TokenType> getPossibleTokens_Print();
    private:
        std::vector<Token> tokens;
        size_t currentPosition = 0;
};