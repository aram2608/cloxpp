find_package(fmt CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(magic_enum CONFIG QUIET)

add_library(loxlib
    loxlib/chunk/chunk.cpp
    loxlib/value/value.cpp
    loxlib/vm/vm.cpp
    loxlib/compiler/compiler.cpp
    loxlib/scanner/scanner.cpp
)

target_link_libraries(loxlib
    PUBLIC
    fmt::fmt
    cxxopts::cxxopts
)

target_include_directories(loxlib
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/loxlib/
)

target_compile_options(loxlib
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

add_executable(cloxppvm
    app/main.cpp
)

target_link_libraries(cloxppvm
    PRIVATE
    loxlib
)