add_library(cloxpp_lib
    src/core/lox.cpp
    src/core/scanner.cpp
    src/core/parser.cpp
    src/core/interpreter.cpp
    src/runtime/resolver.cpp
    src/callable/lox_functions.cpp
    src/callable/native_functions.cpp
    src/callable/lox_classes.cpp
    src/callable/lox_instance.cpp
)

find_package(magic_enum CONFIG QUIET)
target_link_libraries(cloxpp_lib PUBLIC magic_enum::magic_enum)

target_include_directories(cloxpp_lib 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
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