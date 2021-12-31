
cmake_minimum_required(VERSION 3.12)
project(tinylang)

set(CMAKE_CXX_STANDARD 17)

set(SRC 
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/constprop.cpp
    src/main.cpp)

add_executable(tinylang ${SRC})

