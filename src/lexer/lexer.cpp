#include <print>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
//NEW PARSER
#include "lexer.hpp"
#include "token.hpp"

bool Lexer::lexize(const std::filesystem::path& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::println("Error: Could not open file '{}'", filePath.string());
        return false;
    }

    std::string fileContent;

    std::stringstream buffer;
    buffer << file.rdbuf();

    fileContent = buffer.str();

    // std::println("Contents of '{}' are \n'{}'", filePath.string(), fileContent);

    file.close();

    std::string word;

    fileLength = fileContent.length();

    bool consumeChar = true;
    while (totalLengthRead < fileLength)
    {
        char currentChar = fileContent[totalLengthRead];
        // std::println("PreChar: {}", currentChar);

        switch (currentState)
        {
            case LexerState::LEXER_STATE_DEFAULT:
            {
                if (currentChar == '\n')
                {
                    currentLine++;
                    currentColumn = 1;
                }
                // else if (std::isspace(currentChar))
                // {
                //     // Ignore whitespace
                // }
                else if (std::isalpha(currentChar) || currentChar == '_')
                {
                    currentState = LexerState::LEXER_STATE_WORD;
                    consumeChar = false; // Don't consume the current character, as it is part of the identifier
                }
                else if (std::isdigit(currentChar) || currentChar == '.')
                {
                    currentState = LexerState::LEXER_STATE_NUMBER;
                    consumeChar = false; // Don't consume the current character, as it is part of the number
                }
                else if (currentChar == '"')
                {
                    currentState = LexerState::LEXER_STATE_STRING;
                }
                else if (currentChar == '/' && totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '/')
                {
                    currentState = LexerState::LEXER_STATE_COMMENT;
                }
                else if (currentChar == '+'
                    || currentChar == '-'
                    || currentChar == '*'
                    || currentChar == '!'
                    || currentChar == '/'
                    || currentChar == '='
                    || currentChar == '~'
                    || currentChar == '<'
                    || currentChar == '>'
                    || currentChar == '?'
                    || currentChar == '&')
                {
                    currentState = LexerState::LEXER_STATE_OPERATOR;
                    consumeChar = false; // Don't consume the current character, as it is part of the operator
                }
                else if (currentChar == '('
                    || currentChar == ')'
                    || currentChar == '{'
                    || currentChar == '}'
                    || currentChar == '['
                    || currentChar == ']'
                    || currentChar == ';'
                    || currentChar == ',')
                {
                    currentState = LexerState::LEXER_STATE_OTHER;
                    consumeChar = false; // Don't consume the current character, as it is part of another token
                }
                break;
            }

            case LexerState::LEXER_STATE_WORD:
            {
                if (!std::isalnum(currentChar) && currentChar != '_')
                {
                    // End of identifier
                    if (word == "bool")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "true" || word == "false")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_BOOL_LITERAL, word, currentLine));
                    }
                    else if (word == "int8")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "int16")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "int32")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "int64")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "int128")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "uint8")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "uint16")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "uint32")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "uint64")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "uint128")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "float32")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "float64")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_KEYWORD, word, currentLine));
                    }
                    else if (word == "if")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_IF, word, currentLine));
                    }
                    else if (word == "else")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_ELSE, word, currentLine));
                    }
                    else if (word == "for")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_FOR, word, currentLine));
                    }
                    else if (word == "while")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_WHILE, word, currentLine));
                    }
                    else if (word == "return")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_RETURN, word, currentLine));
                    }
                    else if (word == "break")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_BREAK, word, currentLine));
                    }
                    else if (word == "continue")
                    {
                        tokens.push_back(Token(TokenType::TOKEN_CONTINUE, word, currentLine));
                    }
                    else
                    {
                        tokens.push_back(Token(TokenType::TOKEN_IDENTIFIER, word, currentLine));
                    }

                    // tokens.push_back(Token(TokenType::TOKEN_IDENTIFIER, word, currentLine));
                    std::println("Keyword/identifier: '{}', Line: {}", word, currentLine);
                    word.clear();

                    currentState = LexerState::LEXER_STATE_DEFAULT;
                    consumeChar = false; // Don't consume the current character, as it may be part of another token
                }
                else
                {
                    word += currentChar;
                }

                break;
            }

            case LexerState::LEXER_STATE_NUMBER:
            {
                if (!std::isdigit(currentChar) && currentChar != '.')
                {
                    // End of number
                    if (word.contains('.'))
                    {
                        tokens.push_back(Token(TokenType::TOKEN_FLOAT_LITERAL, word, currentLine));
                    }
                    else
                    {
                        tokens.push_back(Token(TokenType::TOKEN_INTEGER_LITERAL, word, currentLine));
                    }
                    std::println("Int/Float: '{}', Line: {}", word, currentLine);
                    word.clear();

                    currentState = LexerState::LEXER_STATE_DEFAULT;
                    consumeChar = false; // Don't consume the current character, as it may be part of another token
                }
                else
                {
                    word += currentChar;
                }
                break;
            }

            case LexerState::LEXER_STATE_STRING:
            {
                // The 1st double quote has already been consumed, so we can just keep reading until we find the closing double quote
                // if next char is a quote, then we have reached the end of the string
                char nextChar = (totalLengthRead + 1 < fileLength) ? fileContent[totalLengthRead + 1] : '\0';
                if (nextChar == '"' && currentChar != '\\')
                {
                    word += currentChar; // Add the current character to the string
                    // End of string
                    tokens.push_back(Token(TokenType::TOKEN_STRING_LITERAL, word, currentLine));
                    std::println("String: '{}', Line: {}", word, currentLine);
                    word.clear();

                    totalLengthRead += 2; // Consume the closing double quote
                    currentColumn += 2; // Move to the next column after the closing double quote

                    currentState = LexerState::LEXER_STATE_DEFAULT;
                    consumeChar = false; // Don't consume the current character, as it may be part of another token
                }
                else
                {
                    if (currentChar !='\\')
                        word += currentChar;
                }

                break;
            }

            case LexerState::LEXER_STATE_COMMENT:
            {
                std::string comment = std::string(1, currentChar); // Start the comment with the first '/' character
                //skip until next line char
                while (totalLengthRead < fileLength && fileContent[totalLengthRead] != '\n')
                {
                    comment += fileContent[totalLengthRead];
                    totalLengthRead++;
                }
                std::println("Comment Detected, isolated capture case: '{}', Line: {}", comment, currentLine);

                currentLine++;
                currentColumn = 1;
                currentState = LexerState::LEXER_STATE_DEFAULT;
                break;
            }

            case LexerState::LEXER_STATE_OPERATOR:
            {
                // Check for Add/Subtract/Multiply/Divide/Assignment operators
                if (currentChar == '+')
                {
                    //compount add operator: +=
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_COMPOUND_ADD_OPERATOR, word, currentLine));
                        std::println("Compound Add: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '+')
                    {
                        word += currentChar;
                        word += '+';
                        tokens.push_back(Token(TokenType::TOKEN_INCREMENT_OPERATOR, word, currentLine));
                        std::println("Increment: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_PLUS_OPERATOR, word, currentLine));
                        std::println("Add: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }
                else if (currentChar == '-')
                {
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_COMPOUND_SUBTRACT_OPERATOR, word, currentLine));
                        std::println("Compound Subtract: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '-')
                    {
                        word += currentChar;
                        word += '-';
                        tokens.push_back(Token(TokenType::TOKEN_DECREMENT_OPERATOR, word, currentLine));
                        std::println("Decrement: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '>')
                    {
                        word += currentChar;
                        word += '>';
                        tokens.push_back(Token(TokenType::TOKEN_METHOD_MEMBER_ACCESS_OPERATOR, word, currentLine));
                        std::println("Method/Member Access: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_MINUS_OPERATOR, word, currentLine));
                        std::println("Subtract: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }
                else if (currentChar == '*')
                {
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_COMPOUND_MULTIPLY_OPERATOR, word, currentLine));
                        std::println("Compound Multiply: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_STAR_OPERATOR, word, currentLine));
                        std::println("Star: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }
                else if (currentChar == '/')
                {
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_COMPOUND_DIVIDE_OPERATOR, word, currentLine));
                        std::println("Compound Divide: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_DIVISION_OPERATOR, word, currentLine));
                        std::println("Divide: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }
                else if (currentChar == '?')
                {
                    // Check for null coalescing operator
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '?')
                    {
                        word += currentChar;
                        word += '?';
                        tokens.push_back(Token(TokenType::TOKEN_NULL_COALESCING_OPERATOR, word, currentLine));
                        std::println("Null Coalescing: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_NULLABLE_OPERATOR, word, currentLine));
                        std::println("Nullable: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }
                else if (currentChar == '=')
                {
                    // Check for assignment operator
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_EQUALITY_OPERATOR, word, currentLine));
                        std::println("Equality: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_ASSIGNMENT_OPERATOR, word, currentLine));
                        std::println("Assignment: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }

                else if (currentChar == '<')
                {
                    // Check for less than or equal to operator
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_LESS_THAN_OR_EQUAL_OPERATOR, word, currentLine));
                        std::println("Less Than or Equal: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    // Lazy return: <~~
                    else if (totalLengthRead + 2 < fileLength && fileContent[totalLengthRead + 1] == '~' && fileContent[totalLengthRead + 2] == '~')
                    {
                        word += currentChar;
                        word += '~';
                        word += '~';
                        tokens.push_back(Token(TokenType::TOKEN_LAZY_RETURN_OPERATOR, word, currentLine));
                        std::println("Lazy ReturnOperator: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead += 2; // Consume the next two characters as well
                    }
                    // Chain operator: <-->
                    else if (totalLengthRead + 3 < fileLength && fileContent[totalLengthRead + 1] == '-' && fileContent[totalLengthRead + 2] == '-' && fileContent[totalLengthRead + 3] == '>')
                    {
                        word += currentChar;
                        word += '-';
                        word += '-';
                        word += '>';
                        tokens.push_back(Token(TokenType::TOKEN_CHAIN_OPERATOR, word, currentLine));
                        std::println("Chain Operator: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead += 3; // Consume the next three characters as well
                    }

                    //less than op
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_LESS_THAN_OPERATOR, word, currentLine));
                        std::println("Less Than: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }
                else if (currentChar == '>')
                {
                    // Check for greater than or equal to operator
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_GREATER_THAN_OR_EQUAL_OPERATOR, word, currentLine));
                        std::println("Greater Than or Equal: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }
                    //check for greater than op
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_GREATER_THAN_OPERATOR, word, currentLine));
                        std::println("Greater Than: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }
                // Module namespace op: !->
                else if (currentChar == '!')
                {
                    // check for not equality operator
                    if (totalLengthRead + 1 < fileLength && fileContent[totalLengthRead + 1] == '=')
                    {
                        word += currentChar;
                        word += '=';
                        tokens.push_back(Token(TokenType::TOKEN_NOT_EQUALITY_OPERATOR, word, currentLine));
                        std::println("Not Equality Operator: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead++; // Consume the next character as well
                    }

                    else if (totalLengthRead + 2 < fileLength && fileContent[totalLengthRead + 1] == '-' && fileContent[totalLengthRead + 2] == '>')
                    {
                        word += currentChar;
                        word += '-';
                        word += '>';
                        tokens.push_back(Token(TokenType::TOKEN_MODULE_NAMESPACE_CROSS_OPERATOR, word, currentLine));
                        std::println("Module Namespace Cross Operator: '{}', Line: {}", word, currentLine);
                        word.clear();
                        totalLengthRead += 2; // Consume the next two characters as well
                    }
                    // the vanilla NOT
                    else
                    {
                        word += currentChar;
                        tokens.push_back(Token(TokenType::TOKEN_NOT_OPERATOR, word, currentLine));
                        std::println("NOT Operator: '{}', Line: {}", word, currentLine);
                        word.clear();
                    }
                }

                else if (currentChar == '&')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_AMPERSAND_OPERATOR, word, currentLine));
                    std::println("Ampersand: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                currentState = LexerState::LEXER_STATE_DEFAULT;
                break;
            }

            case LexerState::LEXER_STATE_OTHER:
            {
                if (currentChar == '(')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_LEFT_PARENTHESIS, word, currentLine));
                    std::println("Left Parenthesis: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                else if (currentChar == ')')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_RIGHT_PARENTHESIS, word, currentLine));
                    std::println("Right Parenthesis: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                else if (currentChar == '{')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_LEFT_BRACE, word, currentLine));
                    std::println("Left Brace: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                else if (currentChar == '}')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_RIGHT_BRACE, word, currentLine));
                    std::println("Right Brace: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                else if (currentChar == '[')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_LEFT_BRACKET, word, currentLine));
                    std::println("Left Bracket: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                else if (currentChar == ']')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_RIGHT_BRACKET, word, currentLine));
                    std::println("Right Bracket: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                else if (currentChar == ';')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_SEMICOLON, word, currentLine));
                    std::println("Semicolon: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                else if (currentChar == ',')
                {
                    word += currentChar;
                    tokens.push_back(Token(TokenType::TOKEN_COMMA, word, currentLine));
                    std::println("Comma: '{}', Line: {}", word, currentLine);
                    word.clear();
                }
                currentState = LexerState::LEXER_STATE_DEFAULT;
                break;
            }
        }

        if (consumeChar)
        {
            totalLengthRead++;
        }
        else
        {
            consumeChar = true;
        }
        // totalLengthRead++;
    }

    return true;
}