cmake_minimum_required(VERSION 3.15)

project(tantrums 
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

add_executable(tantrums
    src/main.cpp
    src/lexer/lexer.cpp
)

target_include_directories(tantrums 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(tantrums PRIVATE)
else()
    target_compile_options(tantrums PRIVATE)
endif()