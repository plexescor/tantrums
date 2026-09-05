#include <optional>
#include <iostream>
#include <stdexcept>
#include <format>
#include <vector>
#include <print>

#include "parser.hpp"
#include "token.hpp"
#include "ast.hpp"

Parser::Parser(const std::vector<Token>& tokens)
{
    this->tokens = tokens;
}

Token& Parser::expect(std::vector<TokenType> expectedTypes)
{
    for (const auto& type : expectedTypes)
    {
        if (tokens[currentPosition].type == type)
        {
            return advance();
        }
    }
    std::string expected = "";

    for (const auto& type : expectedTypes)
    {
        expected += tokenTypeToString(type);
    }
    throw std::runtime_error(std::format(
                "[Parser Error]: Expected a '{}' , Got a '{}'. Line: {}",
                expected,
                tokenTypeToString(tokens[currentPosition].type),
                tokens[currentPosition].line
            ));
}

Token& Parser::expect(TokenType type)
{
    if (tokens[currentPosition].type != type)
    {
        throw std::runtime_error(std::format(
            "[Parser Error]: Expected a '{}' , Got a '{}'.\n Line: {}",
            tokenTypeToString(type),
            tokenTypeToString(tokens[currentPosition].type),
            tokens[currentPosition].line
        ));
    }
    return advance();
}

Token& Parser::advance()
{
    if (currentPosition < tokens.size())
    {
        // Equivalent of tantrums's:
        // return tokens[currentPosition] <~~ currentPosition++;
        return tokens[currentPosition++];
    }
    return tokens.back();
}

Token& Parser::peek()
{
    if (currentPosition + 1 < tokens.size())
    {
        return tokens[currentPosition + 1];
    }
    return tokens.back();
}

Token& Parser::current()
{
    return tokens[currentPosition];
}

ASTNode Parser::parsePrint()
{
    Token token = current();
    token = advance();
    token = expect(TokenType::TOKEN_LEFT_PARENTHESIS);
    token = expect(getPossibleTokens_Print());
    Token finalToken = token;
    token = expect(TokenType::TOKEN_RIGHT_PARENTHESIS);
    token = expect(TokenType::TOKEN_SEMICOLON);
    token = current();

    // std::println("Parse print called!");
    if (finalToken.type == TokenType::TOKEN_STRING_LITERAL)
        return ASTNode( PrintNode { .value = LiteralNode { .value = finalToken.value } } );
    else if (finalToken.type == TokenType::TOKEN_INTEGER_LITERAL)
        return ASTNode( PrintNode { .value = LiteralNode { .value = std::stoi(finalToken.value) } } );
    else if (finalToken.type == TokenType::TOKEN_FLOAT_LITERAL)
        return ASTNode( PrintNode { .value = LiteralNode { .value = std::stod(finalToken.value) } } );
}

ASTNode Parser::parseStatement()
{
    Token token = current();
    if (token.type == TokenType::TOKEN_IDENTIFIER)
    {
        const std::string& value = token.value;
        if (value == "print")
        {
            return parsePrint();
        }

        token = current();
    }
}

std::vector<ASTNode> Parser::parse() 
{
    if (currentPosition >= tokens.size()) return {};
    while (current().type != TokenType::TOKEN_END_OF_FILE)
    {
        ASTNode node = parseStatement();
        ast_Vector.push_back(std::move(node));
    }
    return ast_Vector;
}

std::vector<TokenType> Parser::getPossibleTokens_Print()
{
    std::vector<TokenType> possible = {
        TokenType::TOKEN_STRING_LITERAL,
        TokenType::TOKEN_INTEGER_LITERAL,
        TokenType::TOKEN_FLOAT_LITERAL
    };
    return possible;
}