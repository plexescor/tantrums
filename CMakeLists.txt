cmake_minimum_required(VERSION 3.15)

project(tantrums
	LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM_DIR: ${LLVM_DIR}")
message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM_LIBRARY_DIRS: ${LLVM_LIBRARY_DIRS}")

# Executable
add_executable(tantrums
	src/main.cpp
	src/lexer/lexer.cpp
	src/parser/parser.cpp
	src/llvm/codegen.cpp
	src/compiler/compiler.cpp
	src/symbol/symbolTable.cpp
	src/checkers/typeChecker.cpp
)

# Headers
target_include_directories(tantrums
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${LLVM_INCLUDE_DIRS}
)

# LLVM preprocessor definitions
target_compile_definitions(tantrums PRIVATE ${LLVM_DEFINITIONS})

if(WIN32)
	# Windows: split component libs exist, link individually
	llvm_map_components_to_libnames(LLVM_LIBS
		core support irreader native
		X86CodeGen X86AsmParser X86Desc X86Disassembler X86Info
	)
	target_link_directories(tantrums PRIVATE ${LLVM_LIBRARY_DIRS})
	target_link_libraries(tantrums PRIVATE ${LLVM_LIBS})
else()
	# Linux/Arch: unified lib (-lLLVM-22), use llvm-config directly
	execute_process(
		COMMAND llvm-config --libs all
		OUTPUT_VARIABLE LLVM_LIBS_RAW
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND llvm-config --ldflags
		OUTPUT_VARIABLE LLVM_LD_FLAGS_RAW
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND llvm-config --system-libs
		OUTPUT_VARIABLE LLVM_SYSTEM_LIBS_RAW
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	# Convert space-separated flags to cmake list
	separate_arguments(LLVM_LIBS_LIST UNIX_COMMAND "${LLVM_LIBS_RAW}")
	separate_arguments(LLVM_LD_FLAGS_LIST UNIX_COMMAND "${LLVM_LD_FLAGS_RAW}")
	separate_arguments(LLVM_SYSTEM_LIBS_LIST UNIX_COMMAND "${LLVM_SYSTEM_LIBS_RAW}")

	target_link_options(tantrums PRIVATE ${LLVM_LD_FLAGS_LIST})
	target_link_libraries(tantrums PRIVATE ${LLVM_LIBS_LIST} ${LLVM_SYSTEM_LIBS_LIST})
endif()

if(MSVC)
	target_compile_options(tantrums PRIVATE /W4)
else()
	target_compile_options(tantrums PRIVATE -Wall -Wextra -Wpedantic)
endif()
