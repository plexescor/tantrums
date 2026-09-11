#include <iostream>

#pragma warning(push, 0)
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/Program.h>
#pragma warning(pop)
#include "compiler.hpp"
#include "codegen.hpp"

Compiler::Compiler(CodeGenerator* codegen)
{
	this->codegen = codegen;
}

void Compiler::compile(std::filesystem::path outputPath)
{
	outputPath_Object = outputPath;

	std::error_code ec;
	llvm::raw_fd_ostream output(outputPath_Object.string(), ec, llvm::sys::fs::OF_None);
	
	if (ec)
	{
		std::cerr << ec << std::endl;
		return;
	}

	llvm::legacy::PassManager passManager;
	
	codegen->getTargetMachine()->addPassesToEmitFile(
		passManager,
		output,
		nullptr,
		llvm::CodeGenFileType::ObjectFile
	);

	passManager.run(*codegen->getLlvmModule());
	output.flush();
}

void Compiler::link(std::filesystem::path outputPath)
{
	outputPath_Executable = outputPath;

#ifdef _WIN32
	auto linker = llvm::sys::findProgramByName("link.exe");
	if (!linker)
	{
		std::cerr << "link.exe not found" << std::endl;
		return;
	}

	std::string objPath = std::filesystem::absolute(outputPath_Object).string();
	std::string outArg = "/OUT:" + outputPath.string() + ".exe";

	std::vector<llvm::StringRef> args =
	{
		*linker,
		objPath,
		outArg.c_str(),
		"/DEFAULTLIB:msvcrt",
		"/DEFAULTLIB:ucrt",
		"/DEFAULTLIB:vcruntime",
		"/SUBSYSTEM:CONSOLE",
		"/MACHINE:X64"
	};

#else
	auto linker = llvm::sys::findProgramByName("ld.lld");
	if (!linker)
	{
		std::cerr << "lld not found" << std::endl;
		return;
	}

	std::string objPath = std::filesystem::absolute(outputPath_Object).string();
	std::string outPath = outputPath.string();

	std::vector<llvm::StringRef> args =
	{
		*linker,
		"--hash-style=gnu",
		"--build-id",
		"--eh-frame-hdr",
		"-m", "elf_x86_64",
		"-pie",
		"-dynamic-linker", "/lib64/ld-linux-x86-64.so.2",
		"-o", outPath,
		"/usr/lib64/Scrt1.o",
		"/usr/lib64/crti.o",
		"/usr/lib64/gcc/x86_64-pc-linux-gnu/16/crtbeginS.o",
		"-L/usr/lib64/gcc/x86_64-pc-linux-gnu/16",
		"-L/usr/lib64",
		"-L/usr/lib",
		objPath,
		"-lgcc",
		"--as-needed",
		"-lgcc_s",
		"--no-as-needed",
		"-lc",
		"/usr/lib64/gcc/x86_64-pc-linux-gnu/16/crtendS.o",
		"/usr/lib64/crtn.o"
	};
#endif

	std::string error;
	llvm::sys::ExecuteAndWait(*linker, args, std::nullopt, {}, 0, 0, &error);

	if (!error.empty())
		std::cerr << error << std::endl;
}