#pragma once
#include <filesystem>
#include <vector>
#include <cstdint>
#include "token.hpp"

enum class LexerState
{
    LEXER_STATE_DEFAULT,
    LEXER_STATE_WORD,
    LEXER_STATE_NUMBER,
    LEXER_STATE_STRING,
    LEXER_STATE_COMMENT,
    LEXER_STATE_OPERATOR,
    LEXER_STATE_OTHER,
};

class Lexer
{
    public:
        bool lexize(const std::filesystem::path& filePath);
        // Tantrums future: non mut
        std::vector<Token> getTokens() const;

    private:
        std::vector<Token> tokens;
        uint64_t currentLine = 1;
        uint64_t currentColumn = 1;
        uint64_t totalLengthRead = 0;
        size_t fileLength = 0;
        LexerState currentState = LexerState::LEXER_STATE_DEFAULT;

};