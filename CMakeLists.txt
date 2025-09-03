cmake_minimum_required(VERSION 3.11)
project(cloxpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} 
    cloxpp/src/main.cpp
    cloxpp/src/lox.cpp
    cloxpp/src/scanner.cpp
    cloxpp/src/tokens.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE cloxpp/include)