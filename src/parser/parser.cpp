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

bool Parser::isDeclKeyword(TokenType type)
{
    switch (type)
    {
        case TokenType::TOKEN_INT8:
        case TokenType::TOKEN_INT16:
        case TokenType::TOKEN_INT32:
        case TokenType::TOKEN_INT64:
        case TokenType::TOKEN_UINT8:
        case TokenType::TOKEN_UINT16:
        case TokenType::TOKEN_UINT32:
        case TokenType::TOKEN_UINT64:
        case TokenType::TOKEN_FLOAT32:
        case TokenType::TOKEN_FLOAT64:
        case TokenType::TOKEN_BOOL:
        case TokenType::TOKEN_AUTO:
        case TokenType::TOKEN_MUT:
        case TokenType::TOKEN_HEAP:
            return true;
        default:
            return false;
    }
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
    hadError = true;
    std::string expected = "";

    for (size_t i = 0; i < expectedTypes.size(); ++i)
    {
        if (i > 0) expected += " | ";
        expected += tokenTypeToString(expectedTypes[i]);
    }
    std::println(
        "[Parser Error]: Expected one of [{}] , Got a '{}'. Line: {}",
        expected,
        tokenTypeToString(tokens[currentPosition].type),
        tokens[currentPosition].line
    );
    return advance();
}

Token& Parser::expect(TokenType type)
{
    if (tokens[currentPosition].type != type)
    {
        hadError = true;
        std::println(
            "[Parser Error]: Expected a '{}' , Got a '{}'. Line: {}",
            tokenTypeToString(type),
            tokenTypeToString(tokens[currentPosition].type),
            tokens[currentPosition].line
        );
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

void Parser::synchronize()
{
    while (current().type != TokenType::TOKEN_END_OF_FILE)
    {
        // stop at semicolon
        if (current().type == TokenType::TOKEN_SEMICOLON)
        {
            advance(); // consume the semicolon
            return;
        }
        
        // stop when we hit something that looks like a new statement
        if (isDeclKeyword(current().type) ||
            (current().type == TokenType::TOKEN_IDENTIFIER && current().value == "print"))
        {
            return;
        }
        
        advance();
    }
}

std::optional<ASTNode> Parser::parsePrint()
{
    advance(); // Consume 'print'
    expect(TokenType::TOKEN_LEFT_PARENTHESIS);
    Token finalToken = expect(getPossibleTokens_Print());
    expect(TokenType::TOKEN_RIGHT_PARENTHESIS);
    expect(TokenType::TOKEN_SEMICOLON);

    if (finalToken.type == TokenType::TOKEN_STRING_LITERAL)
        return ASTNode( PrintNode { .value = LiteralNode { .value = finalToken.value } } );
    else if (finalToken.type == TokenType::TOKEN_INTEGER_LITERAL)
        return ASTNode( PrintNode { .value = LiteralNode { .value = std::stoi(finalToken.value) } } );
    else if (finalToken.type == TokenType::TOKEN_FLOAT_LITERAL)
        return ASTNode( PrintNode { .value = LiteralNode { .value = std::stod(finalToken.value) } } );

    hadError = true;
    std::println(
        "[Parser Error]: Expected a printable value, Got '{}'. Line: {}",
        tokenTypeToString(finalToken.type),
        finalToken.line
    );
    return std::nullopt;
}

std::optional<ASTNode> Parser::parseVariableDeclaration()
{
    // i know i am sometimes calling advance without capturing return

    //currently broken, i hadnt had time
    bool isMutable = false;
    bool isAuto = false;

    Token token = current();

    if (token.type == TokenType::TOKEN_MUT)
    {
        isMutable = true;
        // if we do this on outside, then type-tokens can get skipped if mut is not there
        advance();
    }   

    //Contains type/auto
    Token typeToken = current(); 
    if (typeToken.type == TokenType::TOKEN_AUTO) isAuto = true;

    // Now expect an identifier
    advance();
    Token nameToken = expect(TokenType::TOKEN_IDENTIFIER);

    expect(TokenType::TOKEN_ASSIGNMENT_OPERATOR);

    Token litVal = expect(getPossibleTokens_Print());

    expect(TokenType::TOKEN_SEMICOLON);

    return ASTNode(
        VariableDeclarationNode { 
            .isMutable = isMutable,
            .type = typeToken.value, 
            .isAuto = isAuto,
            .name = nameToken.value,
            .value = LiteralNode { .value = litVal.value }
        });
}

std::optional<ASTNode> Parser::parseStatement()
{
    Token token = current();
    if (token.type == TokenType::TOKEN_IDENTIFIER)
    {
        const std::string& value = token.value;
        if (value == "print")
        {
            return parsePrint();
        }
    }
    else if (isDeclKeyword(token.type))
    {
        return parseVariableDeclaration();
    }

    hadError = true;
    std::println(
        "[Parser Error]: Unknown statement '{}'. Line: {}",
        token.value,
        token.line
    );
    return std::nullopt;
}

std::vector<ASTNode> Parser::parse() 
{
    if (currentPosition >= tokens.size()) return {};
    while (current().type != TokenType::TOKEN_END_OF_FILE)
    {
        std::optional<ASTNode> node = parseStatement();
        if (node.has_value())
        {
            ast_Vector.push_back(std::move(*node));
        }
        else
        {
            synchronize();
        }
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

std::vector<TokenType> Parser::getPossibleTokens_Decl()
{
    // What can come? after mut
    // remember: mut type name = 6;
    std::vector<TokenType> possible = {
        TokenType::TOKEN_AUTO,
        TokenType::TOKEN_INT8,
        TokenType::TOKEN_INT16,
        TokenType::TOKEN_INT32,
        TokenType::TOKEN_INT64,

        TokenType::TOKEN_UINT8,
        TokenType::TOKEN_UINT16,
        TokenType::TOKEN_UINT32,
        TokenType::TOKEN_UINT64,

        TokenType::TOKEN_BOOL,
        TokenType::TOKEN_STRING,
        TokenType::TOKEN_STAR_OPERATOR
    };
    return possible;
}