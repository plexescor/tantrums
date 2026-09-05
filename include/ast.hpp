#pragma once

#include <string>
#include <variant>
#include <cstdint>

struct LiteralNode
{
    std::variant<
        int8_t,
        int16_t,
        int32_t,
        int64_t,
        uint8_t,
        uint16_t,
        uint32_t,
        uint64_t,
        float,
        double,
        std::string,
        bool
    > value;
};

struct VariableDeclarationNode
{
    bool isMutable;
    std::string type;
    bool isAuto;
    std::string name;

    LiteralNode value;
};

// For printing/debugging until stdlib is implemnetd
struct PrintNode 
{
    LiteralNode value;
};

using ASTNode = std::variant<VariableDeclarationNode, PrintNode>;
