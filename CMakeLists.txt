cmake_minimum_required(VERSION 3.16)
project(cloxpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TREE_WALK "Build tree walker interpreter" OFF)
option(VM_BUILD "Build VM interpreter" OFF)

if(NOT TREE_WALK AND NOT VM_BUILD)
  message(FATAL_ERROR "Nothing to build: enable -TREE_WALK=ON or -VM_BUILD=ON")
endif()

if(TREE_WALK AND VM_BUILD)
  message(FATAL_ERROR "Choose only one of TREE_WALK or VM_BUILD")
endif()

if(TREE_WALK)
    add_subdirectory(cloxpptw)
endif()

if(VM_BUILD)
    add_subdirectory(cloxppvm)
endif()

target_compile_options(cloxpp 
    PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic 
    -Wconversion
    -Wpessimizing-move
    -Wredundant-move
    -Wmove
    -Wrange-loop-construct
)