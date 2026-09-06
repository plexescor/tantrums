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
        case TokenType::TOKEN_VOID:
        case TokenType::TOKEN_IO:        
        case TokenType::TOKEN_THROWS:   
        case TokenType::TOKEN_PURE:      
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

bool Parser::isAnnotationKeyword(TokenType type)
{
    switch (type)
    {
        case TokenType::TOKEN_IO:
        case TokenType::TOKEN_PURE:
        case TokenType::TOKEN_HEAP:
        case TokenType::TOKEN_MUT:
        return true; break;
        default: return false;
    }
}

std::optional<ASTNode> Parser::parseFunctionDeclaration()
{
    // When this gets called, the currentPosition is at a DeclKeyword, we need to check one by one
    bool isHeap = false;
    bool isIo = false;
    bool isThrows = false;
    bool isPure = false;
    bool isAuto = false;
    bool isMut = false;

    Token typeToken;
    // loop until we find an identifier(which wont happen as return) while respecting all annotations
    while (current().type != TokenType::TOKEN_IDENTIFIER)
    {
        switch (current().type)
        {
            case TokenType::TOKEN_MUT:    isMut    = true; break;
            case TokenType::TOKEN_HEAP:   isHeap   = true; break;
            case TokenType::TOKEN_IO:     isIo     = true; break;
            case TokenType::TOKEN_THROWS: isThrows = true; break;
            case TokenType::TOKEN_PURE:   isPure   = true; break;
            default: typeToken = current(); // return type keyword capture
        }
        advance();
    }
    // advance();
    // now its at type/auto
    Token token;
    // Token typeToken = current();
    if (typeToken.type == TokenType::TOKEN_AUTO) isAuto = true;

    // expect an identifier or name
    // advance();

    Token nameToken = expect(TokenType::TOKEN_IDENTIFIER);
    //No args currently
    expect(TokenType::TOKEN_LEFT_PARENTHESIS);
    expect(TokenType::TOKEN_RIGHT_PARENTHESIS);
    expect(TokenType::TOKEN_LEFT_BRACE);

    std::vector<ASTNode> body;
    while (current().type != TokenType::TOKEN_RIGHT_BRACE
        && current().type != TokenType::TOKEN_END_OF_FILE)
    {
        auto stmt = parseStatement();
        if (stmt.has_value())
            body.push_back(std::move(*stmt));
        else
            synchronize();
    }
    expect(TokenType::TOKEN_RIGHT_BRACE);

    // debug: dump what we parsed
    std::println("[FunctionDecl] name={} returnType={} mut={} heap={} io={} throws={} pure={}",
        nameToken.value,
        typeToken.value,
        isMut, isHeap, isIo, isThrows, isPure
    ); 

    return ASTNode {
    FunctionDeclarationNode {
        .type     = TypeNode { .name = typeToken.value, .isNullable = false },
        .name     = nameToken.value,
        .isHeap   = isHeap,
        .isIo     = isIo,
        .isThrows = isThrows,
        .isPure   = isPure,
        .isMut    = isMut,
        .isAuto   = isAuto,
        .body    = std::move(body)
    }};
}

std::optional<ASTNode> Parser::parseVariableDeclaration()
{
    // i know i am sometimes calling advance without capturing return

    //currently broken, i hadnt had time
    bool isMutable = false;
    bool isAuto = false;
    bool isNullable = false;

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

    // Now expect an identifier or nullable op
    advance();
    token = current();
    if (token.type == TokenType::TOKEN_NULLABLE_OPERATOR)
    {
        isNullable = true;
        advance();
    }
    if (isNullable) std::println("Nullable");

    Token nameToken = expect(TokenType::TOKEN_IDENTIFIER);

    expect(TokenType::TOKEN_ASSIGNMENT_OPERATOR);

    Token litVal = expect(getPossibleTokens_Print());

    expect(TokenType::TOKEN_SEMICOLON);

    return ASTNode(
        VariableDeclarationNode { 
            .isMutable = isMutable,
            .type = TypeNode { .name = typeToken.value, .isNullable = isNullable}, 
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
        size_t offset = 0;
    // std::println("[DEBUG] starting scan from: {} type: {}", 
        // tokens[currentPosition].value,
        // tokenTypeToString(tokens[currentPosition].type));

    while (tokens[currentPosition + offset].type != TokenType::TOKEN_IDENTIFIER)
    {
        // std::println("[DEBUG] offset={} token={}", offset, tokens[currentPosition + offset].value);
        offset++;
    }

    // std::println("[DEBUG] found identifier: {} next: {}",
        // tokens[currentPosition + offset].value,
        // tokenTypeToString(tokens[currentPosition + offset + 1].type));

    if (tokens[currentPosition + offset + 1].type == TokenType::TOKEN_LEFT_PARENTHESIS)
        return parseFunctionDeclaration();
    else
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
        TokenType::TOKEN_STRING
        // TokenType::TOKEN_STAR_OPERATOR
    };
    return possible;
}