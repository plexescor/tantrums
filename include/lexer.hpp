#pragma once
#include <filesystem>
#include <string>

class Lexer
{
    public:
        Lexer();
        ~Lexer();

    public:
        void lexize(const std::filesystem::path& filePath); //or lexise?
};