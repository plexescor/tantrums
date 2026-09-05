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
)

# Headers
target_include_directories(tantrums
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${LLVM_INCLUDE_DIRS}
)

# LLVM library location
target_link_directories(tantrums
    PRIVATE
        ${LLVM_LIBRARY_DIRS}
)

# LLVM 22 on your system is monolithic:
# llvm-config --libs core irreader support
# -> -lLLVM-22
target_link_libraries(tantrums
    PRIVATE
        LLVM-22
)

# LLVM compile definitions
target_compile_definitions(tantrums
    PRIVATE
        ${LLVM_DEFINITIONS}
)

if(MSVC)

    target_compile_options(tantrums PRIVATE)

else()

    target_compile_options(tantrums PRIVATE)

endif()