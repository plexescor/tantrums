#include <print>
#include "lexer.hpp"

Lexer::Lexer()
{
    //idk
}

Lexer::~Lexer()
{
    //idk
}

void Lexer::lexize(const std::filesystem::path& filePath)
{
    // std::println("Lexing file: {}", filePath.string());
    if (!std::filesystem::exists(filePath))
}