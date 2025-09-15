cmake_minimum_required(VERSION 3.16)
project(cloxpp-monorepo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_CLOXPPTW "Build tree walker interpreter" OFF)
option(BUILD_CLOXPPVM "Build VM interpreter" OFF)

if(NOT BUILD_CLOXPPTW AND NOT BUILD_CLOXPPVM)
  message(FATAL_ERROR "Nothing to build: enable -BUILD_CLOXPPTW=ON or -BUILD_CLOXPPVM=ON")
endif()

if(BUILD_CLOXPPTW AND BUILD_CLOXPPVM)
  message(FATAL_ERROR "Choose only one of BUILD_CLOXPPTW or BUILD_CLOXPPVM")
endif()

if(BUILD_CLOXPPTW)
    add_subdirectory(cloxpptw)
endif()

if(BUILD_CLOXPPVM)
    add_subdirectory(cloxppvm)
endif()