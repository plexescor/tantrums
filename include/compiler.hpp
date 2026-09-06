#pragma once
#include <filesystem>

#include "codegen.hpp"

class Compiler
{
	public:
		Compiler(CodeGenerator* codegen);
		void compile(std::filesystem::path outputPath);
		void link(std::filesystem::path outputPath);

	private:
		std::filesystem::path outputPath_Object;
		std::filesystem::path outputPath_Executable;
		CodeGenerator* codegen;
};
	