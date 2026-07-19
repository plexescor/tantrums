#pragma once
#include <filesystem>
#include <string>
#include <vector>

enum class TOKEN_TYPE {
    TOKEN_IDENTIFIER,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING_LITERAL,
    TOKEN_DOUBLE_QUOTE,
    TOKEN_SINGLE_QUOTE,
    TOKEN_LEFT_PARANTHESIS,
    TOKEN_RIGHT_PARANTHESIS,
    TOKEN_LEFT_CURLY_BRACES,
    TOKEN_RIGHT_CURLY_BRACES,
    TOKEN_TANTRUMS,
    TOKEN_VOID,
    TOKEN_SEMI_COLON,
    TOKEN_RETURN,
    TOKEN_PRINT,
    TOKEN_UNKNOWN,
    TOKEN_END_OF_FILE
};

struct Token {
    TOKEN_TYPE type;
    std::string value;
    uint64_t lineNumber;
};

class Lexer
{
    public:
        Lexer();
        ~Lexer();

    public:
        void lexize(const std::filesystem::path& filePath); //or lexise?
        std::vector<Token> getTokens();
    
    private:
        // Token getNextToken(std::string& line);
        TOKEN_TYPE getTokenType(std::string& word);

    private:
        bool isQuoteOpen = false;
        char lastChar;
        uint64_t currentLine = 1;
        std::vector<Token> tokens;
};