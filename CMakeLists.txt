cmake_minimum_required(VERSION 3.15)

project(tantrums 
    LANGUAGES C CXX
)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

llvm_map_components_to_libnames(LLVM_LIBS core support irreader)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

add_executable(tantrums
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
)

target_include_directories(tantrums 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(tantrums PRIVATE ${LLVM_LIBS})
target_compile_definitions(tantrums PRIVATE ${LLVM_DEFINITIONS})

if(MSVC)
    target_compile_options(tantrums PRIVATE)
else()
    target_compile_options(tantrums PRIVATE)
endif()