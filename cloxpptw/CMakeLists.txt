add_library(cloxpp_lib
    loxlib/core/lox.cpp
    loxlib/core/scanner.cpp
    loxlib/core/parser.cpp
    loxlib/core/interpreter.cpp
    loxlib/runtime/resolver.cpp
    loxlib/callable/lox_functions.cpp
    loxlib/callable/native_functions.cpp
    loxlib/callable/lox_classes.cpp
    loxlib/callable/lox_instance.cpp
)

find_package(magic_enum CONFIG QUIET)
find_package(cxxopts CONFIG REQUIRED)
target_link_libraries(cloxpp_lib PUBLIC magic_enum::magic_enum cxxopts::cxxopts)

target_include_directories(cloxpp_lib 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/loxlib/
)

target_compile_options(cloxpp_lib
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

add_executable(cloxpptw
    app/main.cpp
)
target_link_libraries(cloxpptw PRIVATE cloxpp_lib)