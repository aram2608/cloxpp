cmake_minimum_required(VERSION 3.11)
project(cloxpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(magic_enum CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
    cloxpp/src/main.cpp
    cloxpp/src/lox.cpp
    cloxpp/src/scanner.cpp
    cloxpp/src/parser.cpp
    cloxpp/src/interpreter.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE cloxpp/src/ast)
target_include_directories(${PROJECT_NAME} PRIVATE cloxpp/src/utils)
target_include_directories(${PROJECT_NAME} PRIVATE cloxpp/src/runtime)
target_include_directories(${PROJECT_NAME} PRIVATE cloxpp/src/debug)

target_link_libraries(${PROJECT_NAME} PRIVATE magic_enum::magic_enum)

target_compile_options(${PROJECT_NAME} 
    PRIVATE -Wall -Wextra -Wpedantic -Wconversion)