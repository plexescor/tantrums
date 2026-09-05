#pragma once
#include <vector>

#include "token.hpp"

class Parser
{
    public:
        Parser(const std::vector<Token>& tokens);
        ~Parser() = default;
        void parse();
    
    private:
        std::vector<Token> tokens;
};