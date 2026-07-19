#include <print>
#include "lexer.hpp"
#include <fstream>
#include <vector>
#include <cctype>
#include <sstream>

Lexer::Lexer()
{
    //idk
}

Lexer::~Lexer()
{
    //idk
}

//DEBUGGING
template <>
struct std::formatter<TOKEN_TYPE> : std::formatter<std::string_view> {
    auto format(TOKEN_TYPE token, std::format_context& ctx) const {
        std::string_view name = "Unknown";
        switch (token) {
            case TOKEN_TYPE::TOKEN_TANTRUMS:   name = "TOKEN_TANTRUMS"; break;
            case TOKEN_TYPE::TOKEN_VOID: name = "TOKEN_VOID"; break;
            case TOKEN_TYPE::TOKEN_UNKNOWN:  name = "TOKEN_UNKNOWN"; break;
            default: name = "TOKEN_UNKNOWN";
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

std::vector<Token> Lexer::getTokens()
{
    return tokens;
}

void Lexer::lexize(const std::filesystem::path& filePath)
{
    // std::println("Lexing file: {}", filePath.string());
    if (!std::filesystem::exists(filePath))
    {
        std::println("[I/O Error] File does not exist: {}", filePath.string());
        return;
    }

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::println("[I/O Error] Cant open file: {}", filePath.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string fileContent = buffer.str();
    file.close();

    std::string word = "";
    
    for (char const& character : fileContent)
    {
        if (std::isalnum(static_cast<unsigned char>(character)) || isQuoteOpen)
        {
            if (isQuoteOpen)
            {
                if (character == '\"')
                {
                    isQuoteOpen = !isQuoteOpen;
                    continue;
                }
            }
            if (toSkipLine)
            {
                if (character == '\n')
                {
                    toSkipLine = false;
                    currentLine++;
                }
                continue;
            }

            word += character;
        }
        else if (character == '{' 
                || character == '}'
                || character == '('
                || character == ')'
                || character == ' '
                || character == '\''
                || character == '\"'
                || character == '['
                || character == ']'
                || character == ';'
                || character == '/')
        {
            if (toSkipLine)
            {
                if (character == '\n')
                {
                    toSkipLine = false;
                }
                continue;
            }
            else if (character == '/')
            {
                if (isCommentFirstSlash)
                {
                    toSkipLine = true;
                    isCommentFirstSlash = false;
                    continue;
                }
                else
                {
                    isCommentFirstSlash = false;
                }
                isCommentFirstSlash = true;
            }
            else if (!word.empty())
            {
                isCommentFirstSlash = false;
                TOKEN_TYPE tokenType = getTokenType(word);
                Token token = { tokenType, word, currentLine };
                tokens.push_back(token);
                std::println("keyword: {}  at: {}", word, currentLine);
                word.clear();

                if (character == '{' 
                    || character == '}'
                    || character == '('
                    || character == ')'
                    || character == '['
                    || character == ']')
                {   //Delimeters
                    std::string delimiter = "";
                    delimiter.push_back(character);
                    TOKEN_TYPE tokenType = getTokenType(delimiter);
                    Token token = { tokenType, delimiter, currentLine };
                    tokens.push_back(token);
                    std::println("[STRICT] delimiter: {}  at: {}", delimiter, currentLine);
                }
            }
            else
            {
                isCommentFirstSlash = false;
                if (character == '\"')
                {
                    isQuoteOpen = !isQuoteOpen;
                    // std::string delimiter = "";
                    // delimiter.push_back(character);
                    // TOKEN_TYPE tokenType = getTokenType(delimiter);
                    // Token token = { tokenType, delimiter };
                    // tokens.push_back(token);
                    // std::println("delimiter: {}", delimiter);
                    continue;
                }
                if (character == ' ') continue;
                std::string delimiter = "";
                delimiter.push_back(character);
                TOKEN_TYPE tokenType = getTokenType(delimiter);
                Token token = { tokenType, delimiter, currentLine };
                tokens.push_back(token);
                std::println("delimiter: {}  at: {}", delimiter, currentLine);
            }
        }
        else if (character == '\n')
        {
            currentLine++;
            toSkipLine = false;
        }
    }

    if (!word.empty())
    {
        TOKEN_TYPE tokenType = getTokenType(word);
        Token token = { tokenType, word, currentLine };
        tokens.push_back(token);
        std::println("keyword (EOF trailing): {}  at: {}", word, currentLine);
        word.clear(); 
    }

    Token token =  { TOKEN_TYPE::TOKEN_END_OF_FILE, "", currentLine };
    tokens.push_back(token);
    std::println("Reached End-Of-File successfully.");

}

TOKEN_TYPE Lexer::getTokenType(std::string& word)
{
    if (word == "tantrums") return TOKEN_TYPE::TOKEN_TANTRUMS;
    else if (word == "void") return TOKEN_TYPE::TOKEN_VOID;
    else if (word == "(") return TOKEN_TYPE::TOKEN_LEFT_PARANTHESIS;
    else if (word == ")") return TOKEN_TYPE::TOKEN_RIGHT_PARANTHESIS;
    else if (word == "{") return TOKEN_TYPE::TOKEN_LEFT_CURLY_BRACES;
    else if (word == "}") return TOKEN_TYPE::TOKEN_RIGHT_CURLY_BRACES;
    else if (word == ";") return TOKEN_TYPE::TOKEN_SEMI_COLON;
    else if (word == "\'") return TOKEN_TYPE::TOKEN_SINGLE_QUOTE;
    else if (word == "\"") return TOKEN_TYPE::TOKEN_DOUBLE_QUOTE;
    else return TOKEN_TYPE::TOKEN_UNKNOWN;
}